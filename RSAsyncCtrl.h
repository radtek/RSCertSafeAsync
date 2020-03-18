#pragma once

// RSAsyncCtrl.h : Declaration of the CRSAsyncCtrl ActiveX Control class.


// CRSAsyncCtrl : See RSAsyncCtrl.cpp for implementation.
#include "Poco/AutoPtr.h"
#include "WebSocketChannel.h"

class CRSAsyncCtrl : public COleControl
{
	DECLARE_DYNCREATE(CRSAsyncCtrl)

// Constructor
public:
	CRSAsyncCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//virtual DWORD GetControlFlags();

// Implementation
protected:
	~CRSAsyncCtrl();

	DECLARE_OLECREATE_EX(CRSAsyncCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CRSAsyncCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CRSAsyncCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CRSAsyncCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		eventidRS_CloudGetCertAuthEvent = 6L,
		eventidRS_CloudDevAuthEvent = 5L,
		eventidRS_CloudEncAuthEvent = 4L,
		eventidRS_CloudLoginAuthEvent = 3L,
		eventidRS_CloudGetSignResultEvent = 2L,
		eventidRS_CloudSealAuthEvent = 1L,
		dispidTestEvent = 1L
	};
protected:
	void InitializeWebSocket();
	VARIANT_BOOL TestEvent(USHORT index);
	virtual void OnSetClientSite();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM Lparam);
private:

	void RS_CloudSealAuthEvent(LPCTSTR authResult, LPCTSTR transid, LPCTSTR token, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudSealAuthEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), authResult, transid, token, msg);
	}

	void RS_CloudGetSignResultEvent(LPCTSTR signResult, LPCTSTR signdMsg, LPCTSTR transid, LPCTSTR signdCert, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudGetSignResultEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), signResult, signdMsg, transid, signdCert, msg);
	}

	void RS_CloudLoginAuthEvent(LPCTSTR authResult, LPCTSTR transid, LPCTSTR token, LPCTSTR mobile, LPCTSTR userName, LPCTSTR userID, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudLoginAuthEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), authResult, transid, token, mobile, userName, userID, msg);
	}

	void RS_CloudEncAuthEvent(LPCTSTR authResult, LPCTSTR transid, LPCTSTR token, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudEncAuthEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), authResult, transid, token, msg);
	}

	void RS_CloudDevAuthEvent(LPCTSTR authResult, LPCTSTR transid, LPCTSTR token, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudDevAuthEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), authResult, transid, token, msg);
	}

	void RS_CloudGetCertAuthEvent(LPCTSTR authResult, LPCTSTR transid, LPCTSTR token, LPCTSTR msg)
	{
		FireEvent(eventidRS_CloudGetCertAuthEvent, EVENT_PARAM(VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR), authResult, transid, token, msg);
	}

private:
	Poco::AutoPtr<Reach::WebSocketChannel> _pWSChannel;
};

