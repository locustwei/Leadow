#include "TdxTradSocket.h"
#include "winds\TDXMain.h"

void DoSocketProcedure(LPARAM param)
{
	PTDX_SOCKET_DATA pData = (PTDX_SOCKET_DATA)param;
	BOOL result = FALSE;
	PTDX_STOCK_BY pBy;

	switch(pData->ID){
	case TF_STOCKBY:
		pBy = (PTDX_STOCK_BY)pData->data;
		if(CTDXMain::WndHooker!=NULL){
			result = CTDXMain::WndHooker->DoStockBy(pBy->mark, pBy->Code, pBy->fPrice, pBy->dwVolume);
		}
		break;
	case TF_STOCKBY_RET:
		break;
	case TF_STOCKSEL:
		pBy = (PTDX_STOCK_BY)pData->data;
		if(CTDXMain::WndHooker!=NULL){
			result = CTDXMain::WndHooker->DoStockSell(pBy->mark, pBy->Code, pBy->fPrice, pBy->dwVolume);
		}
		break;
	case TF_STOCKSEL_RET:
		break;
	case TF_GETZJGF:
		if(CTDXMain::WndHooker!=NULL){
			result = CTDXMain::WndHooker->DoStockZjgf();
		}
		break;
	}

	free(pData);
}

class CTradSocketListenner: public ISocketListener
{
public:

	CTdxTradSocket* m_TradSocket;

	virtual void OnConnected(PLD_CLIENT_SOCKET pSocket)
	{
		int nSize = 0;
		PTDX_SOCKET_DATA pData = MakeStockData(NULL, TF_REGISTER, nSize);
		m_TradSocket->Send((char*)pData, nSize, pSocket);
	}

	virtual void OnRecv(PLD_CLIENT_SOCKET pSocket)
	{

		if(pSocket->nRecvSize==0 || pSocket->lpRecvedBuffer==NULL)
			return;

		if(((PTDX_SOCKET_DATA)pSocket->lpRecvedBuffer)->ID == TF_REGISTER){
			pSocket->tag = 1;  //��֤�ɹ� //todo
			m_TradSocket->m_bAvailable = TRUE;
			return;
		}

		PTDX_SOCKET_DATA pData = (PTDX_SOCKET_DATA)malloc(pSocket->nRecvSize);
		memcpy(pData, pSocket->lpRecvedBuffer, pSocket->nRecvSize);

		RunOnMainThread(&DoSocketProcedure, (LPARAM)pData);  //�н�������ķ��䵽���̣߳���ǰ�߳���Socekt select��

	}

	virtual void OnClosed(PLD_CLIENT_SOCKET)
	{
		
	}

	virtual void OnAccept(PLD_CLIENT_SOCKET)
	{
		
	}

	virtual void OnError(PLD_CLIENT_SOCKET, int)
	{
		
	}

};

CTdxTradSocket::CTdxTradSocket(void)
{
	m_bAvailable = FALSE;

	SetListener(new CTradSocketListenner());

	ConnectCtrlor();
}


CTdxTradSocket::~CTdxTradSocket(void)
{
	if(m_Listner)
		delete m_Listner;
}

void CTdxTradSocket::ConnectCtrlor()
{
	int i=0;
	do{ //����3������TDX���������
		if(ConnectTo("127.0.0.1", TDX_SOCKET_PORT)){
			m_bAvailable = TRUE;
			return;
		}
	}while(i++<3);

	if(GetStatus()==SS_NONE){ //����ʧ��
		//����������Ϊ�ǽ������û�����������Լ���Ϊ����״̬�ȴ����ӡ�
		Listen(TDX_SOCKET_PORT);
	}
}

PLD_CLIENT_SOCKET CTdxTradSocket::GetActiveSocket()
{
	if(GetStatus()==SS_LISTENING && GetClientHead()!=NULL){  //��Ϊ�����
		return GetClientHead();
	}else if(GetStatus()==SS_CONNECTED)     //��Ϊ�ͻ���
		return this;
	return NULL;
}

void CTdxTradSocket::SendStockByResult(DWORD htid)
{
	int nSize = sizeof(htid);
	PTDX_SOCKET_DATA pData = MakeStockData(&htid, TF_STOCKBY_RET, nSize);
	Send((char*)pData, nSize, GetActiveSocket());
}

void CTdxTradSocket::SendStockSellResult(DWORD htid)
{
	int nSize = sizeof(htid);
	PTDX_SOCKET_DATA pData = MakeStockData(&htid, TF_STOCKSEL_RET, nSize);
	Send((char*)pData, nSize, GetActiveSocket());
}

void CTdxTradSocket::SendStockZjgfResult(PTDX_STOCK_ZJGF pZjgf, int nSize)
{
	PTDX_SOCKET_DATA pData = MakeStockData(pZjgf, TF_GETZJGF_RET, nSize);
	Send((char*)pData, nSize, GetActiveSocket());;
}
