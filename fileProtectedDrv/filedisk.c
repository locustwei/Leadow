/*
    This is a virtual disk driver for Windows that uses one or more files to
    emulate physical disks.
    Copyright (C) 1999-2015 Bo Brant�n.
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <ntifs.h>
#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntstrsafe.h>
#include <wdmsec.h>
#include <mountmgr.h>
#include <ntddvol.h>
#include <ntddscsi.h>
#include "filedisk.h"
#include "CtrlDevice.h"
#include "..\..\WiseRegNotify\DirverUtils.h"


#define TOC_DATA_TRACK          0x04
#define FILE_DISK_POOL_TAG      'WisE'
#define FT_BALANCED_READ_MODE   0x66001b
#define PART_STARTOFFSET        1024

HANDLE dir_handle = NULL;

ULONG g_DeviceNumber = 1;

//PDEVICE_OBJECT g_FileDisObjects[FILE_DISK_DEVICE_COUNT] = {NULL};

typedef struct _FD_DEVICE_EXTENSION {
    BOOLEAN                     media_in_device;
	WCHAR                       device_letter;
    UNICODE_STRING              device_name;
    ULONG                       device_number;
    DEVICE_TYPE                 device_type;
    HANDLE                      file_handle;
    UNICODE_STRING              file_name;
    LARGE_INTEGER               file_size;
    BOOLEAN                     read_only;
    PSECURITY_CLIENT_CONTEXT    security_client_context;
    LIST_ENTRY                  list_head;
    KSPIN_LOCK                  list_lock;
    KEVENT                      request_event;
    PVOID                       thread_pointer;
    BOOLEAN                     terminate_thread;
} FD_DEVICE_EXTENSION, *PFD_DEVICE_EXTENSION;

#ifdef _PREFAST_
DRIVER_INITIALIZE DriverEntry;
__drv_dispatchType(IRP_MJ_CREATE) __drv_dispatchType(IRP_MJ_CLOSE) DRIVER_DISPATCH FileDiskCreateClose;
__drv_dispatchType(IRP_MJ_READ) __drv_dispatchType(IRP_MJ_WRITE) DRIVER_DISPATCH FileDiskReadWrite;
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL) DRIVER_DISPATCH FileDiskDeviceControl;
KSTART_ROUTINE FileDiskThread;
DRIVER_UNLOAD FileDiskUnload;
#endif // _PREFAST_

VOID
	FileDiskThread (
	IN PVOID Context
	);

#pragma code_seg("PAGE")

NTSTATUS CreateDeviceDirectory()
{
	NTSTATUS                    status;
	UNICODE_STRING              device_dir_name;
	OBJECT_ATTRIBUTES           object_attributes;
	
	if(dir_handle != NULL)
		return STATUS_SUCCESS;

	RtlInitUnicodeString(&device_dir_name, DEVICE_DIR_NAME);
	InitializeObjectAttributes(
		&object_attributes,
		&device_dir_name,
		OBJ_PERMANENT | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
		);

	status = ZwCreateDirectoryObject(
		&dir_handle,
		DIRECTORY_ALL_ACCESS,
		&object_attributes
		);

	if (!NT_SUCCESS(status))
	{
		return status;
	}

	ZwMakeTemporaryObject(dir_handle);

	return status;
}

NTSTATUS
FileDiskCreateDevice (
    IN PDRIVER_OBJECT   DriverObject,
    IN ULONG            Number,
    IN DEVICE_TYPE      DeviceType,
	OUT PDEVICE_OBJECT*      pDevice
    )
{
    UNICODE_STRING      device_name;
    NTSTATUS            status;
	PDEVICE_OBJECT      device_object;
    PFD_DEVICE_EXTENSION   device_extension;
    HANDLE              thread_handle;
    UNICODE_STRING      sddl;

    ASSERT(DriverObject != NULL);

    device_name.Buffer = (PWCHAR) ExAllocatePoolWithTag(PagedPool, MAXIMUM_FILENAME_LENGTH * 2, FILE_DISK_POOL_TAG);

    if (device_name.Buffer == NULL)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    device_name.Length = 0;
    device_name.MaximumLength = MAXIMUM_FILENAME_LENGTH * 2;

    if (DeviceType == FILE_DEVICE_CD_ROM)
    {
        RtlUnicodeStringPrintf(&device_name, DEVICE_NAME_PREFIX L"Cd" L"%u", Number);
    }
    else
    {
        RtlUnicodeStringPrintf(&device_name, DEVICE_NAME_PREFIX L"%u", Number);
    }

    RtlInitUnicodeString(&sddl, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;BU)");
	
    status = IoCreateDeviceSecure(
        DriverObject,
        sizeof(FD_DEVICE_EXTENSION),
        &device_name,
        DeviceType,
        0,
        FALSE,
        &sddl,
        NULL,
        &device_object
        );
	
    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_name.Buffer);
        return status;
    }

    device_object->Flags |= DO_DIRECT_IO;

    device_extension = (PFD_DEVICE_EXTENSION) device_object->DeviceExtension;

    device_extension->media_in_device = FALSE;

    device_extension->device_name.Length = device_name.Length;
    device_extension->device_name.MaximumLength = device_name.MaximumLength;
    device_extension->device_name.Buffer = device_name.Buffer;
    device_extension->device_number = Number;
    device_extension->device_type = DeviceType;

	//device_object->Characteristics |= FILE_REMOVABLE_MEDIA;
    if (DeviceType == FILE_DEVICE_CD_ROM)
    {
        device_object->Characteristics |= FILE_READ_ONLY_DEVICE;
        device_extension->read_only = TRUE;
    }

    InitializeListHead(&device_extension->list_head);

    KeInitializeSpinLock(&device_extension->list_lock);

    KeInitializeEvent(
        &device_extension->request_event,
        SynchronizationEvent,
        FALSE
        );

    device_extension->terminate_thread = FALSE;

    status = PsCreateSystemThread(
        &thread_handle,
        (ACCESS_MASK) 0L,
        NULL,
        NULL,
        NULL,
        FileDiskThread,
        device_object
        );

    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(device_object);
        ExFreePool(device_name.Buffer);
        return status;
    }

    status = ObReferenceObjectByHandle(
        thread_handle,
        THREAD_ALL_ACCESS,
        NULL,
        KernelMode,
        &device_extension->thread_pointer,
        NULL
        );

    if (!NT_SUCCESS(status))
    {
        ZwClose(thread_handle);

        device_extension->terminate_thread = TRUE;

        KeSetEvent(
            &device_extension->request_event,
            (KPRIORITY) 0,
            FALSE
            );

        IoDeleteDevice(device_object);

        ExFreePool(device_name.Buffer);

        return status;
    }

    ZwClose(thread_handle);
	
	//IoAttachDeviceToDeviceStack(device_object, DriverObject->DeviceObject);

	device_object->Flags ^= DO_DEVICE_INITIALIZING;

	*pDevice = device_object;

    return STATUS_SUCCESS;
}

/*
VOID CreateDosSybolic(ULONG Number, WCHAR Letter)
{
	NTSTATUS status;
	UNICODE_STRING deviceName, symLinkName;
	
	deviceName.Buffer = (PWCHAR) ExAllocatePoolWithTag(PagedPool, MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR), FILE_DISK_POOL_TAG);
	if(deviceName.Buffer == NULL)
		return;
	RtlZeroMemory(deviceName.Buffer, MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR));
	deviceName.Length = 0;
	deviceName.MaximumLength = MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR);

	symLinkName.Buffer = (PWCHAR) ExAllocatePoolWithTag(PagedPool, MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR), FILE_DISK_POOL_TAG);
	if(symLinkName.Buffer == NULL)
		return;
	RtlZeroMemory(symLinkName.Buffer, MAXIMUM_FILENAME_LENGTH * sizeof(WCHAR));
	symLinkName.Length = 0;
	symLinkName.MaximumLength = MAXIMUM_FILENAME_LENGTH * 2;

	RtlUnicodeStringPrintf(&deviceName, DEVICE_NAME_PREFIX L"%u", Number);
	RtlUnicodeStringPrintf(&symLinkName, SYMBOL_NAME_PREFIX, Letter);

	//status = IoCreateSymbolicLink(&symLinkName,&deviceName);

	ExFreePoolWithTag(deviceName.Buffer, FILE_DISK_POOL_TAG);
	ExFreePoolWithTag(symLinkName.Buffer, FILE_DISK_POOL_TAG);
}
*/

NTSTATUS CallDiskDriver(PDEVICE_OBJECT DeviceObject, ULONG ioCode, PVOID InpubBuffer, ULONG cb)
{
	NTSTATUS Status = STATUS_SUCCESS;
	IO_STATUS_BLOCK ioStatus;
	PIRP irp;
	KEVENT Event;
	KeInitializeEvent(&Event, KernelMode, FALSE);

	irp = IoBuildDeviceIoControlRequest(ioCode, DeviceObject, InpubBuffer, cb, NULL, 0, FALSE, &Event, &ioStatus);
	if(irp){
		Status = IoCallDriver(DeviceObject, irp);
		if(Status == STATUS_PENDING){
			KeWaitForSingleObject(&Event, Executive,KernelMode,FALSE,NULL);
			Status = STATUS_SUCCESS;//irp->IoStatus.Status;
		}
	}
	return Status;
}

PDEVICE_OBJECT FindDeviceObjectByName(PDRIVER_OBJECT DriverObject, PWCHAR FileName, ULONG NameLength)
{

	PFD_DEVICE_EXTENSION device_extension;
	PDEVICE_OBJECT DeviceObject;

	for (DeviceObject = DriverObject->DeviceObject; DeviceObject; DeviceObject = DeviceObject->NextDevice)
	{
		if(DeviceObject->DeviceType == FILE_DEVICE_DISK){
			device_extension = (PFD_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
			//DbgPrint("%wZ, %S, %d %d", device_extension->file_name, FileName, device_extension->file_name.Length, NameLength);
			if(device_extension->media_in_device && device_extension->file_name.Length == NameLength && _wcsnicmp(device_extension->file_name.Buffer, FileName, NameLength / sizeof(WCHAR)) == 0 ){
				return DeviceObject;
			}
		}
	}

	return NULL;
}

PDEVICE_OBJECT FindDeviceObject(PDRIVER_OBJECT DriverObject, ULONG DeviceNumber)
{

	PFD_DEVICE_EXTENSION device_extension;
	PDEVICE_OBJECT DeviceObject;

	for (DeviceObject = DriverObject->DeviceObject; DeviceObject; DeviceObject = DeviceObject->NextDevice)
	{
		if(DeviceObject->DeviceType == FILE_DEVICE_DISK){
			device_extension = (PFD_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
			if(device_extension->device_number == DeviceNumber ){
				return DeviceObject;
			}
		}
	}

	return NULL;
}

NTSTATUS RemoveFileDiskDevice (PDRIVER_OBJECT DriverObject, ULONG DeviceNumber)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT DeviceObject = FindDeviceObject(DriverObject, DeviceNumber);
	
	//DbgPrint("RemoveFileDiskDevice %x", DeviceObject);

	if(DeviceObject){
		Status = CallDiskDriver(DeviceObject, IOCTL_FILE_DISK_CLOSE_FILE, NULL, 0);
		if(NT_SUCCESS(Status)){
			Status = FileDiskDeleteDevice(DeviceObject);
		}
	}else
		Status = STATUS_DEVICE_NOT_CONNECTED;

	return Status;
}

NTSTATUS UmountFileDiskDevice (PDRIVER_OBJECT DriverObject, ULONG DeviceNumber)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PFD_DEVICE_EXTENSION device_extension;
	PDEVICE_OBJECT DeviceObject = FindDeviceObject(DriverObject, DeviceNumber);
	
	//DbgPrint("UmountFileDiskDevice %x", DeviceObject);

	if(DeviceObject){
		device_extension = (PFD_DEVICE_EXTENSION)DeviceObject->DeviceExtension;
		device_extension->device_letter = 0;
	}else
		Status = STATUS_DEVICE_NOT_CONNECTED;

	return Status;
}

NTSTATUS MountFileDiskDevice (PDRIVER_OBJECT DriverObject, POPEN_FILE_INFORMATION pInfo, OUT PULONG DeviceNumber)
{
	NTSTATUS Status = STATUS_DEVICE_NOT_CONNECTED;
	//ULONG i = g_DeviceNumber;
	PFD_DEVICE_EXTENSION device_extension;
	PDEVICE_OBJECT device_object;
	//POPEN_FILE_INFORMATION pInfoCpy = NULL;
	device_object = FindDeviceObjectByName(DriverObject, pInfo->FileName, pInfo->FileNameLength );
	if(device_object){
		device_extension = (PFD_DEVICE_EXTENSION)device_object->DeviceExtension;
		device_extension->device_letter = pInfo->DriveLetter;
		*DeviceNumber = device_extension->device_number;
		return STATUS_SUCCESS;
	}

	Status = FileDiskCreateDevice(DriverObject, g_DeviceNumber, FILE_DEVICE_DISK, &device_object);

	if(NT_SUCCESS(Status)){
		device_extension = (PFD_DEVICE_EXTENSION)device_object->DeviceExtension;
		
		//pInfoCpy = (POPEN_FILE_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, pInfo->cb, FILE_DISK_POOL_TAG);
		//if(pInfoCpy == NULL)
			//Status = STATUS_INSUFFICIENT_RESOURCES;
		//else{
			//RtlMoveMemory(pInfoCpy, pInfo, pInfo->cb);
			Status = CallDiskDriver(device_object, IOCTL_FILE_DISK_OPEN_FILE, pInfo, pInfo->cb);
			//ExFreePoolWithTag(pInfoCpy, FILE_DISK_POOL_TAG);
		//}

		if(NT_SUCCESS(Status)){
			(*DeviceNumber) = g_DeviceNumber;
			g_DeviceNumber++;
			
		}else
			FileDiskDeleteDevice(device_object);
	}

	return Status;;
}

NTSTATUS EnumDiskFiles(IN PDRIVER_OBJECT DriverObject, OUT PVOID Buffer, ULONG* BufferLength)
{
	PDEVICE_OBJECT device_object;
	PFD_DEVICE_EXTENSION               device_extension;
	ULONG Length = 0, l;
	POPEN_FILE_INFORMATION pTmp = (POPEN_FILE_INFORMATION)Buffer;

	for (device_object = DriverObject->DeviceObject; device_object; device_object = device_object->NextDevice){
		if(device_object->DeviceType == FILE_DEVICE_DISK && device_object->DeviceExtension){
			device_extension = (PFD_DEVICE_EXTENSION) device_object->DeviceExtension;
			if(device_extension->media_in_device){
				l = sizeof(OPEN_FILE_INFORMATION)+device_extension->file_name.Length + sizeof(WCHAR);
				Length += l;
				
				if(pTmp && Length <= *BufferLength){
					RtlZeroMemory(pTmp, l);
					pTmp->cb = l;
					pTmp->DeviceNumber = device_extension->device_number;
					pTmp->DriveLetter = device_extension->device_letter;
					pTmp->FileNameLength = device_extension->file_name.Length;
					pTmp->FileSize = device_extension->file_size;
					pTmp->ReadOnly = device_extension->read_only;
					RtlMoveMemory(pTmp->FileName, device_extension->file_name.Buffer, device_extension->file_name.Length);
					pTmp = (POPEN_FILE_INFORMATION)((PCHAR)Buffer + Length);
				}
			}
		}
	}

	*BufferLength = Length;
	
	if(Length>*BufferLength){
		return STATUS_BUFFER_OVERFLOW;
	}

	return STATUS_SUCCESS;
}

NTSTATUS SetDiskFileData(PDRIVER_OBJECT DriverObject, PSET_DISK_FILE_DATA pData)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT                     device_object;
	PFD_DEVICE_EXTENSION               device_extension;
	PVOID Buffer;
	LARGE_INTEGER offset;
	IO_STATUS_BLOCK ios;

	do 
	{
		device_object = FindDeviceObject(DriverObject, pData->DeviceNumber);
		if(!device_object){
			Status = STATUS_INVALID_PARAMETER;
			break;
		}
		device_extension = (PFD_DEVICE_EXTENSION)device_object->DeviceExtension;
		
		if(!device_extension->media_in_device){
			Status = STATUS_DEVICE_NOT_CONNECTED;
			break;
		}

		if(device_extension->file_size.QuadPart <= pData->FilePosition + PART_STARTOFFSET){
			Status = STATUS_INVALID_PARAMETER;
			break;
		}

		Status = CallDiskDriver(device_object, IOCTL_VHD_SET_DATA, pData, sizeof(SET_DISK_FILE_DATA) + pData->DataSize);

	} while (FALSE);

	return Status;
}

NTSTATUS GetDiskFileData(PDRIVER_OBJECT DriverObject, PSET_DISK_FILE_DATA pData, PVOID pOut, PULONG Outlen)
{
	NTSTATUS Status = STATUS_SUCCESS;
	PDEVICE_OBJECT                     device_object;
	PFD_DEVICE_EXTENSION               device_extension;

	LARGE_INTEGER offset;
	IO_STATUS_BLOCK ios;

	do 
	{
		device_object = FindDeviceObject(DriverObject, pData->DeviceNumber);
		if(!device_object){
			Status = STATUS_INVALID_PARAMETER;
			break;
		}
// 		if(pData->DeviceNumber >= FILE_DISK_DEVICE_COUNT){
// 			Status = STATUS_INVALID_PARAMETER;
// 			break;
// 		}

		//device_object = g_FileDisObjects[pData->DeviceNumber];
		device_extension = (PFD_DEVICE_EXTENSION)device_object->DeviceExtension;
		if(!device_extension->media_in_device){
			Status = STATUS_DEVICE_NOT_CONNECTED;
			break;
		}

		if(device_extension->file_size.QuadPart <= pData->FilePosition + PART_STARTOFFSET){
			Status = STATUS_INVALID_PARAMETER;
			break;
		}


		offset.QuadPart = pData->FilePosition;

		Status = ZwReadFile(
			device_extension->file_handle,
			NULL,
			NULL,
			NULL,
			&ios,
			pOut,
			pData->DataSize,
			&offset,
			NULL
			);

		if(NT_SUCCESS(Status))
			*Outlen = ios.Information;
	} while (FALSE);

	return Status;
}
// NTSTATUS CreateFileDiskDevice (
// 	IN PDRIVER_OBJECT   DriverObject,
// 	OUT PULONG DeviceNumber
// 	)
// {
// 	NTSTATUS                    status;
// 	
// 	InfoPrint("CreateFileDiskDevice");
// 	do 
// 	{
// 		status = FileDiskCreateDevice(DriverObject, nDeviceNumber, FILE_DEVICE_DISK);
// 		if(NT_SUCCESS(status))
// 			*DeviceNumber = nDeviceNumber ++;
// 
// 		InfoPrint("CreateFileDiskDevice status = 0x%x", status);
// 	} while (FALSE);
// 
// 	return status;
// }
/*
NTSTATUS CreateFileDiskDevices (
	IN PDRIVER_OBJECT   DriverObject
	)
{
	NTSTATUS                    status;
	ULONG i;

	do 
	{
		status = CreateDeviceDirectory();
		if(!NT_SUCCESS(status))
			break;
		for(i=0; i<FILE_DISK_DEVICE_COUNT; i++){
			status = FileDiskCreateDevice(DriverObject, i, FILE_DEVICE_DISK, &g_FileDisObjects[i]);
			if(!NT_SUCCESS(status))
				break;
		}

	} while (FALSE);

	return status;
}
*/
NTSTATUS FileDiskDeleteDevice (
    IN PDEVICE_OBJECT DeviceObject
    )
{
    PFD_DEVICE_EXTENSION   device_extension;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);

    device_extension = (PFD_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
	device_extension->terminate_thread = TRUE;

	KeSetEvent(
		&device_extension->request_event,
		(KPRIORITY) 0,
		FALSE
		);

	KeWaitForSingleObject(
		device_extension->thread_pointer,
		Executive,
		KernelMode,
		FALSE,
		NULL
		);

	ObDereferenceObject(device_extension->thread_pointer);

	if (device_extension->device_name.Buffer != NULL)
	{
		ExFreePool(device_extension->device_name.Buffer);
	}

	if (device_extension->security_client_context != NULL)
	{
		SeDeleteClientSecurity(device_extension->security_client_context);
		ExFreePool(device_extension->security_client_context);
	}
#pragma prefast( suppress: 28175, "allowed in unload" )

    IoDeleteDevice(DeviceObject);

    return STATUS_SUCCESS;
}

VOID
FileDiskUnload (
	IN PDRIVER_OBJECT DriverObject
	)
{
	PDEVICE_OBJECT device_object;
	ULONG i;
	PAGED_CODE();

	for (device_object = DriverObject->DeviceObject; device_object; device_object = device_object->NextDevice)
	{
		if(device_object->DeviceType == FILE_DEVICE_DISK){
			CallDiskDriver(device_object, IOCTL_FILE_DISK_CLOSE_FILE, NULL, 0);
			FileDiskDeleteDevice(device_object);
		}
	}


	ZwClose(dir_handle);
}

#pragma code_seg() // end "PAGE"

NTSTATUS
FileDiskCreateClose (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = FILE_OPENED;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS IoCompletion(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp,
	PVOID Context
	)
{
	PKEVENT Event = (PKEVENT)Context;
	KeSetEvent(Event, 0, FALSE);
	return STATUS_SUCCESS;
}

NTSTATUS
FileDiskReadWrite (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PFD_DEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION  io_stack;
	//KEVENT Event;
	
	//KeInitializeEvent(&Event, KernelMode, FALSE);

    device_extension = (PFD_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    if (!device_extension->media_in_device)
    {
        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }
    
	io_stack = IoGetCurrentIrpStackLocation(Irp);

    if (io_stack->Parameters.Read.Length == 0)
    {
        Irp->IoStatus.Status = STATUS_SUCCESS;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_SUCCESS;
    }

	//IoSetCompletionRoutineEx(DeviceObject, Irp, IoCompletion, &Event, TRUE, TRUE, TRUE);

    IoMarkIrpPending(Irp);

    ExInterlockedInsertTailList(
        &device_extension->list_head,
        &Irp->Tail.Overlay.ListEntry,
        &device_extension->list_lock
        );

    KeSetEvent(
        &device_extension->request_event,
        (KPRIORITY) 0,
        FALSE
        );
	
	//KeWaitForSingleObject(&Event, Executive,KernelMode,FALSE,NULL);

    return STATUS_PENDING;
}

NTSTATUS
FileDiskDeviceControl (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PFD_DEVICE_EXTENSION   device_extension;
    PIO_STACK_LOCATION     io_stack;
    NTSTATUS               status;


    device_extension = (PFD_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    io_stack = IoGetCurrentIrpStackLocation(Irp);

	if (!device_extension->media_in_device &&
        io_stack->Parameters.DeviceIoControl.IoControlCode != IOCTL_FILE_DISK_OPEN_FILE)
    {
        Irp->IoStatus.Status = STATUS_NO_MEDIA_IN_DEVICE;
        Irp->IoStatus.Information = 0;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);

        return STATUS_NO_MEDIA_IN_DEVICE;
    }

	switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
    {
    case IOCTL_FILE_DISK_OPEN_FILE:
        {
            SECURITY_QUALITY_OF_SERVICE security_quality_of_service;

            if (device_extension->media_in_device)
            {
                status = STATUS_INVALID_DEVICE_REQUEST;
                Irp->IoStatus.Information = 0;
                break;
            }

            if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
                sizeof(OPEN_FILE_INFORMATION))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }

            if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
                sizeof(OPEN_FILE_INFORMATION) +
                ((POPEN_FILE_INFORMATION)Irp->AssociatedIrp.SystemBuffer)->FileNameLength -
                sizeof(WCHAR))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }

            if (device_extension->security_client_context != NULL)
            {
                SeDeleteClientSecurity(device_extension->security_client_context);
            }
            else
            {
                device_extension->security_client_context =
                    ExAllocatePoolWithTag(NonPagedPool, sizeof(SECURITY_CLIENT_CONTEXT), FILE_DISK_POOL_TAG);
            }

            RtlZeroMemory(&security_quality_of_service, sizeof(SECURITY_QUALITY_OF_SERVICE));

            security_quality_of_service.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
            security_quality_of_service.ImpersonationLevel = SecurityImpersonation;
            security_quality_of_service.ContextTrackingMode = SECURITY_STATIC_TRACKING;
            security_quality_of_service.EffectiveOnly = FALSE;

            SeCreateClientSecurity(
                PsGetCurrentThread(),
                &security_quality_of_service,
                FALSE,
                device_extension->security_client_context
                );

			IoMarkIrpPending(Irp);

            ExInterlockedInsertTailList(
                &device_extension->list_head,
                &Irp->Tail.Overlay.ListEntry,
                &device_extension->list_lock
                );

            KeSetEvent(
                &device_extension->request_event,
                (KPRIORITY) 0,
                FALSE
                );

            status = STATUS_PENDING;

            break;
        }

    case IOCTL_FILE_DISK_CLOSE_FILE:
        {
			device_extension->media_in_device = FALSE;

			IoMarkIrpPending(Irp);

            ExInterlockedInsertTailList(
                &device_extension->list_head,
                &Irp->Tail.Overlay.ListEntry,
                &device_extension->list_lock
                );

            KeSetEvent(
                &device_extension->request_event,
                (KPRIORITY) 0,
                FALSE
                );

            status = STATUS_PENDING;

            break;
        }

    case IOCTL_FILE_DISK_QUERY_FILE:
        {
            POPEN_FILE_INFORMATION open_file_information;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(OPEN_FILE_INFORMATION) + device_extension->file_name.Length - sizeof(WCHAR))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            open_file_information = (POPEN_FILE_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

            open_file_information->FileSize.QuadPart = device_extension->file_size.QuadPart;
            open_file_information->ReadOnly = device_extension->read_only;
            open_file_information->FileNameLength = device_extension->file_name.Length;

            RtlCopyMemory(
                open_file_information->FileName,
                device_extension->file_name.Buffer,
                device_extension->file_name.Length
                );

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(OPEN_FILE_INFORMATION) +
                open_file_information->FileNameLength - sizeof(WCHAR);

            break;
        }
	case IOCTL_VHD_SET_DATA:

		IoMarkIrpPending(Irp);

		ExInterlockedInsertTailList(
			&device_extension->list_head,
			&Irp->Tail.Overlay.ListEntry,
			&device_extension->list_lock
			);

		KeSetEvent(
			&device_extension->request_event,
			(KPRIORITY) 0,
			FALSE
			);

		status = STATUS_PENDING;

		break;
    case IOCTL_DISK_CHECK_VERIFY:
    case IOCTL_CDROM_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY:
    case IOCTL_STORAGE_CHECK_VERIFY2:
        {
            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;
            break;
        }

    case IOCTL_DISK_GET_DRIVE_GEOMETRY:
    case IOCTL_CDROM_GET_DRIVE_GEOMETRY:
        {
            PDISK_GEOMETRY  disk_geometry;
            ULONGLONG       length;
            ULONG           sector_size;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(DISK_GEOMETRY))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            disk_geometry = (PDISK_GEOMETRY) Irp->AssociatedIrp.SystemBuffer;

            length = device_extension->file_size.QuadPart;

            if (device_extension->device_type != FILE_DEVICE_CD_ROM)
            {
                sector_size = 512;
            }
            else
            {
                sector_size = 2048;
            }

            disk_geometry->Cylinders.QuadPart = length / sector_size / 32 / 2;
            disk_geometry->MediaType = FixedMedia;
            disk_geometry->TracksPerCylinder = 2;
            disk_geometry->SectorsPerTrack = 32;
            disk_geometry->BytesPerSector = sector_size;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(DISK_GEOMETRY);

            break;
        }

    case IOCTL_DISK_GET_LENGTH_INFO:
        {
            PGET_LENGTH_INFORMATION get_length_information;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(GET_LENGTH_INFORMATION))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            get_length_information = (PGET_LENGTH_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

            get_length_information->Length.QuadPart = device_extension->file_size.QuadPart;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(GET_LENGTH_INFORMATION);

        break;
        }

    case IOCTL_DISK_GET_PARTITION_INFO:
        {
            PPARTITION_INFORMATION  partition_information;
            ULONGLONG               length;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(PARTITION_INFORMATION))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            partition_information = (PPARTITION_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

            length = device_extension->file_size.QuadPart;

            partition_information->StartingOffset.QuadPart = 0;
            partition_information->PartitionLength.QuadPart = length;
            partition_information->HiddenSectors = 1;
            partition_information->PartitionNumber = 0;
            partition_information->PartitionType = 0;
            partition_information->BootIndicator = FALSE;
            partition_information->RecognizedPartition = FALSE;
            partition_information->RewritePartition = FALSE;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION);

            break;
        }

    case IOCTL_DISK_GET_PARTITION_INFO_EX:
        {
            PPARTITION_INFORMATION_EX   partition_information_ex;
            ULONGLONG                   length;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(PARTITION_INFORMATION_EX))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            partition_information_ex = (PPARTITION_INFORMATION_EX) Irp->AssociatedIrp.SystemBuffer;

            length = device_extension->file_size.QuadPart;

            partition_information_ex->PartitionStyle = PARTITION_STYLE_MBR;
            partition_information_ex->StartingOffset.QuadPart = 0;
            partition_information_ex->PartitionLength.QuadPart = length;
            partition_information_ex->PartitionNumber = 0;
            partition_information_ex->RewritePartition = FALSE;
            partition_information_ex->Mbr.PartitionType = 0;
            partition_information_ex->Mbr.BootIndicator = FALSE;
            partition_information_ex->Mbr.RecognizedPartition = FALSE;
            partition_information_ex->Mbr.HiddenSectors = 1;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(PARTITION_INFORMATION_EX);

            break;
        }

    case IOCTL_DISK_IS_WRITABLE:
        {
            if (!device_extension->read_only)
            {
                status = STATUS_SUCCESS;
            }
            else
            {
                status = STATUS_MEDIA_WRITE_PROTECTED;
            }
            Irp->IoStatus.Information = 0;
            break;
        }

    case IOCTL_DISK_MEDIA_REMOVAL:
    case IOCTL_STORAGE_MEDIA_REMOVAL:
        {
            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;
            break;
        }
		
    case IOCTL_CDROM_READ_TOC:
        {
            PCDROM_TOC cdrom_toc;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(CDROM_TOC))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            cdrom_toc = (PCDROM_TOC) Irp->AssociatedIrp.SystemBuffer;

            RtlZeroMemory(cdrom_toc, sizeof(CDROM_TOC));

            cdrom_toc->FirstTrack = 1;
            cdrom_toc->LastTrack = 1;
            cdrom_toc->TrackData[0].Control = TOC_DATA_TRACK;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(CDROM_TOC);

            break;
        }
		
		
    case IOCTL_CDROM_GET_LAST_SESSION:
        {
            PCDROM_TOC_SESSION_DATA cdrom_toc_s_d;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(CDROM_TOC_SESSION_DATA))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            cdrom_toc_s_d = (PCDROM_TOC_SESSION_DATA) Irp->AssociatedIrp.SystemBuffer;

            RtlZeroMemory(cdrom_toc_s_d, sizeof(CDROM_TOC_SESSION_DATA));

            cdrom_toc_s_d->FirstCompleteSession = 1;
            cdrom_toc_s_d->LastCompleteSession = 1;
            cdrom_toc_s_d->TrackData[0].Control = TOC_DATA_TRACK;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(CDROM_TOC_SESSION_DATA);

            break;
        }
		
    case IOCTL_DISK_SET_PARTITION_INFO:
        {
            if (device_extension->read_only)
            {
                status = STATUS_MEDIA_WRITE_PROTECTED;
                Irp->IoStatus.Information = 0;
                break;
            }

            if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
                sizeof(SET_PARTITION_INFORMATION))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;

            break;
        }

    case IOCTL_DISK_VERIFY:
        {
            PVERIFY_INFORMATION verify_information;

            if (io_stack->Parameters.DeviceIoControl.InputBufferLength <
                sizeof(VERIFY_INFORMATION))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }

            verify_information = (PVERIFY_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = verify_information->Length;

            break;
        }

    case IOCTL_STORAGE_GET_DEVICE_NUMBER:
        {
            PSTORAGE_DEVICE_NUMBER number;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(STORAGE_DEVICE_NUMBER))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            number = (PSTORAGE_DEVICE_NUMBER) Irp->AssociatedIrp.SystemBuffer;

            number->DeviceType = device_extension->device_type;
            number->DeviceNumber = device_extension->device_number;
            number->PartitionNumber = (ULONG) -1;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(STORAGE_DEVICE_NUMBER);

            break;
        }

    case IOCTL_STORAGE_GET_HOTPLUG_INFO:
        {
            PSTORAGE_HOTPLUG_INFO info;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(STORAGE_HOTPLUG_INFO))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            info = (PSTORAGE_HOTPLUG_INFO) Irp->AssociatedIrp.SystemBuffer;

            info->Size = sizeof(STORAGE_HOTPLUG_INFO); 
            info->MediaRemovable = 0; 
            info->MediaHotplug = 0;
            info->DeviceHotplug = 0;
            info->WriteCacheEnableOverride = 0;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(STORAGE_HOTPLUG_INFO);

            break;
        }

    case IOCTL_VOLUME_GET_GPT_ATTRIBUTES:
        {
            PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION attr;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            attr = (PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

            attr->GptAttributes = 0;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(VOLUME_GET_GPT_ATTRIBUTES_INFORMATION);

            break;
        }

    case IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS:
        {
            PVOLUME_DISK_EXTENTS ext;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(VOLUME_DISK_EXTENTS))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }
/*
            // not needed since there is only one disk extent to return
            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(VOLUME_DISK_EXTENTS) + ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT)))
            {
                status = STATUS_BUFFER_OVERFLOW;
                Irp->IoStatus.Information = 0;
                break;
            }
*/
            ext = (PVOLUME_DISK_EXTENTS) Irp->AssociatedIrp.SystemBuffer;

            ext->NumberOfDiskExtents = 1;
            ext->Extents[0].DiskNumber = device_extension->device_number;
            ext->Extents[0].StartingOffset.QuadPart = 0;
            ext->Extents[0].ExtentLength.QuadPart = device_extension->file_size.QuadPart;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(VOLUME_DISK_EXTENTS) /*+ ((NumberOfDiskExtents - 1) * sizeof(DISK_EXTENT))*/;

            break;
        }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_DISK_IS_CLUSTERED CTL_CODE(IOCTL_DISK_BASE, 0x003e, METHOD_BUFFERED, FILE_ANY_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_DISK_IS_CLUSTERED:
        {
            PBOOLEAN clus;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(BOOLEAN))
            {
                status = STATUS_BUFFER_TOO_SMALL;
                Irp->IoStatus.Information = 0;
                break;
            }

            clus = (PBOOLEAN) Irp->AssociatedIrp.SystemBuffer;

            *clus = FALSE;

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = sizeof(BOOLEAN);

            break;
        }

    case IOCTL_MOUNTDEV_QUERY_DEVICE_NAME:
        {
            PMOUNTDEV_NAME name;

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                sizeof(MOUNTDEV_NAME))
            {
                status = STATUS_INVALID_PARAMETER;
                Irp->IoStatus.Information = 0;
                break;
            }

            name = (PMOUNTDEV_NAME) Irp->AssociatedIrp.SystemBuffer;
            name->NameLength = device_extension->device_name.Length * sizeof(WCHAR);

            if (io_stack->Parameters.DeviceIoControl.OutputBufferLength <
                name->NameLength + sizeof(USHORT))
            {
                status = STATUS_BUFFER_OVERFLOW;
                Irp->IoStatus.Information = sizeof(MOUNTDEV_NAME);
                break;
            }

            RtlCopyMemory(name->Name, device_extension->device_name.Buffer, name->NameLength);

            status = STATUS_SUCCESS;
            Irp->IoStatus.Information = name->NameLength + sizeof(USHORT);

            break;
        }
		
    case IOCTL_CDROM_READ_TOC_EX:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
		
    case IOCTL_DISK_GET_MEDIA_TYPES:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
	case FT_BALANCED_READ_MODE://0x66001b:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
    case IOCTL_SCSI_GET_CAPABILITIES:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
    case IOCTL_SCSI_PASS_THROUGH:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
    case IOCTL_STORAGE_MANAGE_DATA_SET_ATTRIBUTES:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
    case IOCTL_STORAGE_QUERY_PROPERTY:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }

#if (NTDDI_VERSION < NTDDI_VISTA)
#define IOCTL_VOLUME_QUERY_ALLOCATION_HINT CTL_CODE(IOCTL_VOLUME_BASE, 20, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#endif  // NTDDI_VERSION < NTDDI_VISTA

    case IOCTL_VOLUME_QUERY_ALLOCATION_HINT:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
            break;
        }
    default:
        {
            status = STATUS_INVALID_DEVICE_REQUEST;
            Irp->IoStatus.Information = 0;
        }
    }

    if (status != STATUS_PENDING)
    {
        Irp->IoStatus.Status = status;

        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return status;
}

#pragma code_seg("PAGE")

NTSTATUS
FileDiskOpenFile (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
    PFD_DEVICE_EXTENSION            device_extension;
    POPEN_FILE_INFORMATION          open_file_information;
    //UNICODE_STRING                  ufile_name;
    NTSTATUS                        status;
    OBJECT_ATTRIBUTES               object_attributes;
    FILE_END_OF_FILE_INFORMATION    file_eof;
    FILE_BASIC_INFORMATION          file_basic;
    FILE_STANDARD_INFORMATION       file_standard;
    FILE_ALIGNMENT_INFORMATION      file_alignment;
	//LARGE_INTEGER                   offset = {0};
	//PVOID                           Buffer = NULL;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    device_extension = (PFD_DEVICE_EXTENSION) DeviceObject->DeviceExtension;

    open_file_information = (POPEN_FILE_INFORMATION) Irp->AssociatedIrp.SystemBuffer;

	if (DeviceObject->DeviceType != FILE_DEVICE_CD_ROM)
    {
        device_extension->read_only = open_file_information->ReadOnly;
    }

    device_extension->file_name.Length = open_file_information->FileNameLength;
    device_extension->file_name.MaximumLength = open_file_information->FileNameLength;
    device_extension->file_name.Buffer = ExAllocatePoolWithTag(NonPagedPool, open_file_information->FileNameLength, FILE_DISK_POOL_TAG);
	device_extension->device_letter = open_file_information->DriveLetter;


	if (device_extension->file_name.Buffer == NULL)
	{
		return STATUS_INSUFFICIENT_RESOURCES;
	}

    RtlCopyMemory(
        device_extension->file_name.Buffer,
        open_file_information->FileName,
        open_file_information->FileNameLength
        );


    InitializeObjectAttributes(
        &object_attributes,
        &device_extension->file_name,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL
        );

    status = ZwCreateFile(
        &device_extension->file_handle,
        device_extension->read_only ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE,
        &object_attributes,
        &Irp->IoStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_OPEN,
        FILE_NON_DIRECTORY_FILE |
        FILE_RANDOM_ACCESS |
        FILE_NO_INTERMEDIATE_BUFFERING |
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        0
        );

    if (status == STATUS_OBJECT_NAME_NOT_FOUND || status == STATUS_NO_SUCH_FILE)
    {
        if (device_extension->read_only || open_file_information->FileSize.QuadPart == 0)
        {
            ExFreePool(device_extension->file_name.Buffer);
            //RtlFreeUnicodeString(&device_extension->file_name);
			RtlZeroMemory(&device_extension->file_name, sizeof(UNICODE_STRING));

            Irp->IoStatus.Status = STATUS_NO_SUCH_FILE;
            Irp->IoStatus.Information = 0;

            return STATUS_NO_SUCH_FILE;
        }
        else
        {
            status = ZwCreateFile(
                &device_extension->file_handle,
                GENERIC_READ | GENERIC_WRITE,
                &object_attributes,
                &Irp->IoStatus,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                FILE_SHARE_READ,
                FILE_OPEN_IF,
                FILE_NON_DIRECTORY_FILE |
                FILE_RANDOM_ACCESS |
                FILE_NO_INTERMEDIATE_BUFFERING |
                FILE_SYNCHRONOUS_IO_NONALERT,
                NULL,
                0
                );

            if (!NT_SUCCESS(status))
            {
                ExFreePool(device_extension->file_name.Buffer);
                //RtlFreeUnicodeString(&ufile_name);
                return status;
            }

            if (Irp->IoStatus.Information == FILE_CREATED)
            {
				
                status = ZwFsControlFile(
                    device_extension->file_handle,
                    NULL,
                    NULL,
                    NULL,
                    &Irp->IoStatus,
                    FSCTL_SET_SPARSE,
                    NULL,
                    0,
                    NULL,
                    0
                    );
				
                file_eof.EndOfFile.QuadPart = open_file_information->FileSize.QuadPart;

                status = ZwSetInformationFile(
                    device_extension->file_handle,
                    &Irp->IoStatus,
                    &file_eof,
                    sizeof(FILE_END_OF_FILE_INFORMATION),
                    FileEndOfFileInformation
                    );

                if (!NT_SUCCESS(status))
                {
                    ExFreePool(device_extension->file_name.Buffer);
                    //RtlFreeUnicodeString(&ufile_name);
                    ZwClose(device_extension->file_handle);
                    return status;
                }				
            }
        }
    }
    else if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        //RtlFreeUnicodeString(&ufile_name);
        return status;
    }

    //RtlFreeUnicodeString(&ufile_name);

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_basic,
        sizeof(FILE_BASIC_INFORMATION),
        FileBasicInformation
        );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    //
    // The NT cache manager can deadlock if a filesystem that is using the cache
    // manager is used in a virtual disk that stores its file on a filesystem
    // that is also using the cache manager, this is why we open the file with
    // FILE_NO_INTERMEDIATE_BUFFERING above, however if the file is compressed
    // or encrypted NT will not honor this request and cache it anyway since it
    // need to store the decompressed/unencrypted data somewhere, therefor we put
    // an extra check here and don't alow disk images to be compressed/encrypted.
    //
    if (file_basic.FileAttributes & (FILE_ATTRIBUTE_COMPRESSED | FILE_ATTRIBUTE_ENCRYPTED))
    {
       /*
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        Irp->IoStatus.Status = STATUS_ACCESS_DENIED;
        Irp->IoStatus.Information = 0;
        return STATUS_ACCESS_DENIED;
*/
    }

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_standard,
        sizeof(FILE_STANDARD_INFORMATION),
        FileStandardInformation
        );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    device_extension->file_size.QuadPart = file_standard.EndOfFile.QuadPart;

    status = ZwQueryInformationFile(
        device_extension->file_handle,
        &Irp->IoStatus,
        &file_alignment,
        sizeof(FILE_ALIGNMENT_INFORMATION),
        FileAlignmentInformation
        );

    if (!NT_SUCCESS(status))
    {
        ExFreePool(device_extension->file_name.Buffer);
        ZwClose(device_extension->file_handle);
        return status;
    }

    DeviceObject->AlignmentRequirement = file_alignment.AlignmentRequirement;

    if (device_extension->read_only)
    {
        DeviceObject->Characteristics |= FILE_READ_ONLY_DEVICE;
    }
    else
    {
        DeviceObject->Characteristics &= ~FILE_READ_ONLY_DEVICE;
    }

    device_extension->media_in_device = TRUE;
	device_extension->file_size.QuadPart -= PART_STARTOFFSET;

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    return STATUS_SUCCESS;
}

NTSTATUS
FileDiskCloseFile (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    )
{
	NTSTATUS status = STATUS_SUCCESS;
    PFD_DEVICE_EXTENSION device_extension;

    PAGED_CODE();

    ASSERT(DeviceObject != NULL);
    ASSERT(Irp != NULL);

    device_extension = (PFD_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
	do 
	{
		if(device_extension->file_handle){
			status = ZwClose(device_extension->file_handle);
			if(status != STATUS_SUCCESS)
				break;
		}

		device_extension->file_handle = NULL;
		ExFreePool(device_extension->file_name.Buffer);
		device_extension->file_name.Buffer = NULL;

		device_extension->media_in_device = FALSE;
	} while (FALSE);

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
	
    return status;
}

NTSTATUS
FileDiskAdjustPrivilege (
    IN ULONG    Privilege,
    IN BOOLEAN  Enable
    )
{
    NTSTATUS            status;
    HANDLE              token_handle;
    TOKEN_PRIVILEGES    token_privileges;

    PAGED_CODE();

    status = ZwOpenProcessToken(
        NtCurrentProcess(),
        TOKEN_ALL_ACCESS,
        &token_handle
        );

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    token_privileges.PrivilegeCount = 1;
    token_privileges.Privileges[0].Luid = RtlConvertUlongToLuid(Privilege);
    token_privileges.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;

    status = ZwAdjustPrivilegesToken(
        token_handle,
        FALSE,
        &token_privileges,
        sizeof(token_privileges),
        NULL,
        NULL
        );

    ZwClose(token_handle);

    return status;
}

VOID
	FileDiskThread (
	IN PVOID Context
	)
{
	PDEVICE_OBJECT      device_object;
	PFD_DEVICE_EXTENSION   device_extension;
	PLIST_ENTRY         request;
	PIRP                irp, closeIrp = NULL;
	PIO_STACK_LOCATION  io_stack;
	PUCHAR              system_buffer;
	PUCHAR              buffer;
	LARGE_INTEGER       offset;
	ULONG               i;
	PSET_DISK_FILE_DATA    pData;

	PAGED_CODE();

	ASSERT(Context != NULL);

	device_object = (PDEVICE_OBJECT) Context;

	device_extension = (PFD_DEVICE_EXTENSION) device_object->DeviceExtension;

	KeSetPriorityThread(KeGetCurrentThread(), HIGH_PRIORITY); 

	FileDiskAdjustPrivilege(SE_IMPERSONATE_PRIVILEGE, TRUE);

	for (;;)
	{
		KeWaitForSingleObject(
			&device_extension->request_event,
			Executive,
			KernelMode,
			FALSE,
			NULL
			);

		if (device_extension->terminate_thread)
		{
			PsTerminateSystemThread(STATUS_SUCCESS);
		}

		while ((request = ExInterlockedRemoveHeadList(
			&device_extension->list_head,
			&device_extension->list_lock
			)) != NULL)
		{
			irp = CONTAINING_RECORD(request, IRP, Tail.Overlay.ListEntry);

			io_stack = IoGetCurrentIrpStackLocation(irp);

			switch (io_stack->MajorFunction)
			{
			case IRP_MJ_READ:
				system_buffer = (PUCHAR) MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority);
				if (system_buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				buffer = (PUCHAR) ExAllocatePoolWithTag(PagedPool, io_stack->Parameters.Read.Length, FILE_DISK_POOL_TAG);
				if (buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				offset.QuadPart = io_stack->Parameters.Read.ByteOffset.QuadPart + PART_STARTOFFSET;
				ZwReadFile(
					device_extension->file_handle,
					NULL,
					NULL,
					NULL,
					&irp->IoStatus,
					buffer,
					io_stack->Parameters.Read.Length,
					//&io_stack->Parameters.Read.ByteOffset,
					&offset,
					NULL
					);
				for(i=0; i<irp->IoStatus.Information; i++)
					buffer[i] ^= (48 + io_stack->Parameters.Write.ByteOffset.LowPart + i);

				io_stack->Parameters.Read.ByteOffset.QuadPart = offset.QuadPart;

				RtlMoveMemory(system_buffer, buffer, io_stack->Parameters.Read.Length);
				ExFreePool(buffer);

				break;

			case IRP_MJ_WRITE:
				if ((io_stack->Parameters.Write.ByteOffset.QuadPart +
					io_stack->Parameters.Write.Length) >
					device_extension->file_size.QuadPart)
				{
					irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
					irp->IoStatus.Information = 0;
					break;
				}
				offset = io_stack->Parameters.Write.ByteOffset;
				offset.QuadPart += PART_STARTOFFSET;
				buffer = (PUCHAR) ExAllocatePoolWithTag(PagedPool, io_stack->Parameters.Read.Length, FILE_DISK_POOL_TAG);
				if (buffer == NULL)
				{
					irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
					irp->IoStatus.Information = 0;
					break;
				}
				RtlMoveMemory(buffer, MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority), io_stack->Parameters.Write.Length);

				for(i=0; i<io_stack->Parameters.Write.Length; i++)
					buffer[i] ^= (48 + io_stack->Parameters.Write.ByteOffset.LowPart + i);

				ZwWriteFile(
					device_extension->file_handle,
					NULL,
					NULL,
					NULL,
					&irp->IoStatus,
					//MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority),
					buffer,
					io_stack->Parameters.Write.Length,
					//&io_stack->Parameters.Write.ByteOffset,
					&offset,
					NULL
					);
				ExFreePool(buffer);
				io_stack->Parameters.Write.ByteOffset.QuadPart = offset.QuadPart;

				break;

			case IRP_MJ_DEVICE_CONTROL:
				switch (io_stack->Parameters.DeviceIoControl.IoControlCode)
				{
				case IOCTL_FILE_DISK_OPEN_FILE:

					SeImpersonateClient(device_extension->security_client_context, NULL);

					irp->IoStatus.Status = FileDiskOpenFile(device_object, irp);

					PsRevertToSelf();

					break;

				case IOCTL_FILE_DISK_CLOSE_FILE:
					irp->IoStatus.Status = FileDiskCloseFile(device_object, irp);
					//closeIrp = irp; //���������
					//continue;
					break;
				case IOCTL_VHD_SET_DATA:
					pData = (PSET_DISK_FILE_DATA)irp->AssociatedIrp.SystemBuffer;

					buffer = (PUCHAR) ExAllocatePoolWithTag(PagedPool, PART_STARTOFFSET, FILE_DISK_POOL_TAG);
					if (buffer == NULL)
					{
						irp->IoStatus.Status = STATUS_INSUFFICIENT_RESOURCES;
						irp->IoStatus.Information = 0;
						break;
					}
					offset.QuadPart = 0;
					ZwReadFile(device_extension->file_handle, NULL, NULL, NULL, &irp->IoStatus, buffer, PART_STARTOFFSET, &offset, NULL);
					RtlCopyMemory(buffer + pData->FilePosition, pData->Data, pData->DataSize);
					
					irp->IoStatus.Status = ZwWriteFile(
						device_extension->file_handle,
						NULL,
						NULL,
						NULL,
						&irp->IoStatus,
						//MmGetSystemAddressForMdlSafe(irp->MdlAddress, NormalPagePriority),
						buffer,
						PART_STARTOFFSET,
						//&io_stack->Parameters.Write.ByteOffset,
						&offset,
						NULL
						);

					irp->IoStatus.Information = 0;
					ExFreePoolWithTag(buffer, FILE_DISK_POOL_TAG);

					break;
				default:
					irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
				}
				break;

			default:
				irp->IoStatus.Status = STATUS_DRIVER_INTERNAL_ERROR;
			}
				
			IoCompleteRequest(irp,(CCHAR) (NT_SUCCESS(irp->IoStatus.Status) ? IO_DISK_INCREMENT : IO_NO_INCREMENT));
		}

// 		if(closeIrp){
// 			closeIrp->IoStatus.Status = FileDiskCloseFile(device_object, closeIrp);
// 			IoCompleteRequest(closeIrp,(CCHAR) (NT_SUCCESS(closeIrp->IoStatus.Status) ? IO_DISK_INCREMENT : IO_NO_INCREMENT));
// 			closeIrp = NULL;
// 		}

	}
}

#pragma code_seg() // end "PAGE"
