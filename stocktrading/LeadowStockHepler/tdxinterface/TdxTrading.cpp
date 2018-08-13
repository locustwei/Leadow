#include "stdafx.h"
#include "TdxTrading.h"

class CTdxListenner: public ISocketListener
{
public:
	CTdxTrading* m_Trading;

	virtual void OnConnected(PLD_CLIENT_SOCKET pSocket)
	{
		m_Trading->SendStockData(TF_REGISTER, NULL, 0);
	}

	virtual void OnRecv(PLD_CLIENT_SOCKET pSocket)
	{
		if(pSocket->nRecvSize==0 || pSocket->lpRecvedBuffer==NULL)
			return;

		PTDX_SOCKET_DATA pData = (PTDX_SOCKET_DATA)pSocket->lpRecvedBuffer;
		switch(pData->ID){
		case TF_REGISTER:
			pSocket->tag = 1;  //��֤�ɹ� //todo
			m_Trading->m_bAvailable = TRUE;
			break;
		case TF_STOCKBY:
		case TF_STOCKBY_RET:
			SetEvent(m_Trading->m_hEvent);
			break;
		case TF_STOCKSEL:
			break;
		case TF_STOCKSEL_RET:
			SetEvent(m_Trading->m_hEvent);
			break;
		}
	}

	virtual void OnClosed(PLD_CLIENT_SOCKET)
	{
		
	}

	virtual void OnAccept(PLD_CLIENT_SOCKET pSocket)
	{
		pSocket->tag = 0;  //�ȴ���֤
	}

	virtual void OnError(PLD_CLIENT_SOCKET, int)
	{
		
	}

};

CTdxTrading::CTdxTrading(void)
{
	m_bAvailable = FALSE;

	m_Socket = new CLdSocket();
	CTdxListenner* listener = new CTdxListenner();
	listener->m_Trading = this;
	m_Socket->SetListener(listener);

	//���ڵȴ�Socket��Ӧ
	m_hEvent = CreateEvent(NULL, TRUE, FALSE, TEXT("CTdxTradingEvent")); 

	ConnectTdx();
}


CTdxTrading::~CTdxTrading(void)
{
	if(m_Socket->GetListener())
		delete m_Socket->GetListener();
	if(m_Socket)
		delete m_Socket;
	if(m_hEvent)
		CloseHandle(m_hEvent);
}

BOOL CTdxTrading::StockBy( STOCK_MARK mark, LPCSTR szSymbol, float fPrice, DWORD dwVolume )
{
	if(!IsAvailable())
		return FALSE;
	
	BOOL result = FALSE;
	TDX_STOCK_BY stb = {mark, "", fPrice, dwVolume};
	strcpy_s(stb.Code, szSymbol);
	result = SendStockDataWait(TF_STOCKBY, &stb, sizeof(stb));
	
	if(result){
		PTDX_SOCKET_DATA pRet = (PTDX_SOCKET_DATA)GetActiveSocket()->lpRecvedBuffer;
		result = pRet->data[0];
	}
	return result;
}

BOOL CTdxTrading::StockSell( STOCK_MARK mark, LPCSTR szSymbol, float fPrice, DWORD dwVolume )
{
	if(!IsAvailable())
		return FALSE;

	BOOL result = FALSE;
	TDX_STOCK_BY stb = {mark, "", fPrice, dwVolume};
	strcpy_s(stb.Code, szSymbol);
	result = SendStockDataWait(TF_STOCKSEL, &stb, sizeof(stb));

	if(result){
		PTDX_SOCKET_DATA pRet = (PTDX_SOCKET_DATA)GetActiveSocket()->lpRecvedBuffer;
		result = pRet->data[0];
	}

	return result;
}

BOOL CTdxTrading::IsAvailable()
{
	return m_bAvailable;
}

//���Ƴ����뽻�׳���˭������˭Listen�˿�
void CTdxTrading::ConnectTdx()
{
	int i=0;
	do{ //����3������TDX���������
		if(m_Socket->ConnectTo("127.0.0.1", TDX_SOCKET_PORT)){
			m_bAvailable = TRUE;
			return;
		}
	}while(i++<3);

	if(m_Socket->GetStatus()==SS_NONE){ //����ʧ��
		//����������Ϊ�ǽ������û�����������Լ���Ϊ����״̬�ȴ����ӡ�
		m_Socket->Listen(TDX_SOCKET_PORT);
	}
}

BOOL CTdxTrading::SendStockDataWait(TDX_TRAD_FUN fID, LPVOID pData, int nSize)
{
	BOOL result = FALSE;

	if(SendStockData(fID, pData, nSize)){
		result = WaitReturn(10000);
	}

	return result;
}

BOOL CTdxTrading::SendStockData(TDX_TRAD_FUN fID, LPVOID pData, int nSize)
{
	PTDX_SOCKET_DATA pTdxData = MakeStockData(pData, fID, nSize);

	PLD_CLIENT_SOCKET pSocket = GetActiveSocket();

	BOOL result = m_Socket->Send((char*)pTdxData, nSize, pSocket)==nSize;

	free(pTdxData);

	return result;
}

//�ȴ�ִ�н������
BOOL CTdxTrading::WaitReturn(DWORD msecond)
{
	if(msecond<=0)
		msecond = INFINITE;
	return WaitForSingleObject(m_hEvent, msecond) == WAIT_OBJECT_0;
}

PLD_CLIENT_SOCKET CTdxTrading::GetActiveSocket()
{
	if(m_Socket->GetStatus()==SS_LISTENING && m_Socket->GetClientHead()!=NULL){  //��Ϊ�����
		return m_Socket->GetClientHead();
	}else if(m_Socket->GetStatus()==SS_CONNECTED)     //��Ϊ�ͻ���
		return m_Socket;
	return NULL;
}
