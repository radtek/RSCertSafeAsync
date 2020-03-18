// RSAsyncCtrl.cpp : Implementation of the CRSAsyncCtrl ActiveX Control class.

#include "pch.h"
#include "framework.h"
#include "RSAsync.h"
#include "RSAsyncCtrl.h"
#include "RSAsyncPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CRSAsyncCtrl, COleControl)

// Message map

BEGIN_MESSAGE_MAP(CRSAsyncCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_EDIT, OnEdit)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

// Dispatch map

BEGIN_DISPATCH_MAP(CRSAsyncCtrl, COleControl)
	DISP_STOCKPROP_READYSTATE()
	DISP_FUNCTION_ID(CRSAsyncCtrl, "TestEvent", dispidTestEvent, TestEvent, VT_BOOL, VTS_UI2)
END_DISPATCH_MAP()

// Event map

BEGIN_EVENT_MAP(CRSAsyncCtrl, COleControl)
	EVENT_STOCK_READYSTATECHANGE()
	EVENT_CUSTOM_ID("RS_CloudSealAuthEvent", eventidRS_CloudSealAuthEvent, RS_CloudSealAuthEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	EVENT_CUSTOM_ID("RS_CloudGetSignResultEvent", eventidRS_CloudGetSignResultEvent, RS_CloudGetSignResultEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	EVENT_CUSTOM_ID("RS_CloudLoginAuthEvent", eventidRS_CloudLoginAuthEvent, RS_CloudLoginAuthEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	EVENT_CUSTOM_ID("RS_CloudEncAuthEvent", eventidRS_CloudEncAuthEvent, RS_CloudEncAuthEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	EVENT_CUSTOM_ID("RS_CloudDevAuthEvent", eventidRS_CloudDevAuthEvent, RS_CloudDevAuthEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	EVENT_CUSTOM_ID("RS_CloudGetCertAuthEvent", eventidRS_CloudGetCertAuthEvent, RS_CloudGetCertAuthEvent, VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
END_EVENT_MAP()

// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CRSAsyncCtrl, 1)
	PROPPAGEID(CRSAsyncPropPage::guid)
END_PROPPAGEIDS(CRSAsyncCtrl)

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRSAsyncCtrl, "MFCACTIVEXCONTRO.RSAsyncCtrl.1",
	0xa5104978, 0x463b, 0x4d42, 0xb4, 0x30, 0x06, 0x14, 0x14, 0xe1, 0x1f, 0x2f)

	// Type library ID and version

	IMPLEMENT_OLETYPELIB(CRSAsyncCtrl, _tlid, _wVerMajor, _wVerMinor)

	// Interface IDs

	const IID IID_DRSAsync = { 0x1b5c5acc,0xba09,0x4ee3,{0x9c,0xe1,0x87,0x75,0x97,0x57,0xe3,0xdf} };
const IID IID_DRSAsyncEvents = { 0xab96f9fe,0xc00b,0x4d1e,{0xbb,0x9c,0xb6,0x93,0x3d,0xc3,0x2f,0x4f} };

// Control type information

static const DWORD _dwRSAsyncOleMisc =
OLEMISC_ACTIVATEWHENVISIBLE |
OLEMISC_SETCLIENTSITEFIRST |
OLEMISC_INSIDEOUT |
OLEMISC_CANTLINKINSIDE |
OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CRSAsyncCtrl, IDS_MFCACTIVEXDEMO, _dwRSAsyncOleMisc)

// CRSAsyncCtrl::CRSAsyncCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CRSAsyncCtrl

BOOL CRSAsyncCtrl::CRSAsyncCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegInsertable | afxRegApartmentThreading to afxRegInsertable.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_MFCACTIVEXDEMO,
			IDB_MFCACTIVEXDEMO,
			afxRegInsertable | afxRegApartmentThreading,
			_dwRSAsyncOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// CRSAsyncCtrl::CRSAsyncCtrl - Constructor
CRSAsyncCtrl::CRSAsyncCtrl() :
	_pWSChannel(0)
{
	OutputDebugStringA("Begin CRSAsyncCtrl Constructor\n");
	InitializeIIDs(&IID_DRSAsync, &IID_DRSAsyncEvents);

	m_lReadyState = READYSTATE_LOADING;
	// TODO: Call InternalSetReadyState when the readystate changes.
	// TODO: Initialize your control's instance data here.
	try
	{
		_pWSChannel = new Reach::WebSocketChannel;
		if (_pWSChannel)
			_pWSChannel->open();

		InternalSetReadyState(READYSTATE_LOADED);
	}
	catch (...)
	{
		poco_unexpected();
	}

	OutputDebugStringA("End CRSAsyncCtrl Constructtion \n");
}

// CRSAsyncCtrl::~CRSAsyncCtrl - Destructor

CRSAsyncCtrl::~CRSAsyncCtrl()
{
	OutputDebugStringA("Begin CRSAsyncCtrl Deconstructor\n");
	// TODO: Cleanup your control's instance data here.
	if(_pWSChannel)
		_pWSChannel->close();

	InternalSetReadyState(READYSTATE_UNINITIALIZED);
	OutputDebugStringA("End CRSAsyncCtrl Deconstructor \n");
}

void CRSAsyncCtrl::OnSetClientSite()
{
	HWND hwnd = GetSafeHwnd();
	if (NULL == hwnd)
	{
		VERIFY(CreateControlWindow(::GetDesktopWindow(), CRect(0, 0, 0, 0), CRect(0, 0, 0, 0)));
		if(_pWSChannel)
			_pWSChannel->ready(m_hWnd);
		InternalSetReadyState(READYSTATE_COMPLETE);
	}

	COleControl::OnSetClientSite();
}

#include "Poco/JSON/Parser.h"
#include "Poco/JSON/Object.h"
#include "Poco/DynamicStruct.h"
#include "Poco/Dynamic/Var.h"

#define JSON_PARSE(DATA) \
	Poco::JSON::Parser ps;														\
	Poco::Dynamic::Var res = ps.parse(DATA);									\
	poco_assert(res.type() == typeid(Poco::JSON::Object::Ptr));					\
	Poco::JSON::Object::Ptr object = res.extract<Poco::JSON::Object::Ptr>();	\
	poco_assert(object);														\
	Poco::DynamicStruct ds = *object;

//static std::string GBKEncodingUTF8(const std::string& text);
//static std::string UTF8EncodingGBK(const std::string & inEncoding);
#include "Poco/UTF8Encoding.h"
#include "Poco/Windows936Encoding.h"
#include "Poco/TextConverter.h"

static std::string GBKEncodingUTF8(const std::string & inEncoding)
{
	std::string outstring;
	Poco::UTF8Encoding utf8;
	Poco::Windows936Encoding gbk;
	Poco::TextConverter coverter(gbk, utf8);
	coverter.convert(inEncoding, outstring);
	return outstring;
}

static std::string UTF8EncodingGBK(const std::string & inEncoding)
{
	std::string outstring;
	Poco::UTF8Encoding utf8;
	Poco::Windows936Encoding gbk;
	Poco::TextConverter coverter(utf8, gbk);
	coverter.convert(inEncoding, outstring);
	return outstring;
}

#include <comutil.h>

LRESULT CRSAsyncCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM Lparam)
{
	if (message == UM_EVENT)
	{
		OutputDebugStringA("\n##################### UM_EVENT #####################\n");
		OutputDebugStringA((char*)wParam);
		OutputDebugStringA("\n##################### UM_EVENT #####################\n");

		std::string data((char*)wParam, (int)Lparam);
		try {
			JSON_PARSE(data);
			if (ds["code"] == "0000")
			{
				OutputDebugStringA(Poco::format("json string : \n%s\n", ds.toString()).c_str());
				poco_assert(ds["data"]["action"]);
				int action = ds["data"]["action"].convert<int>();

				if (44 == action)
				{
					std::string signResult = ds["data"]["authResult"];
					std::string transid = ds["data"]["transid"];
					std::string msg = UTF8EncodingGBK(ds["msg"]);

					std::string signdMsg = ds["data"]["signdMsg"];
					std::string signdCert = ds["data"]["certBase64"];

					OutputDebugStringA(Poco::format("RS_CloudGetSignResultEvent json string : \n%s\n", ds.toString()).c_str());
					RS_CloudGetSignResultEvent(_bstr_t(signResult.data()), _bstr_t(signdMsg.data()), _bstr_t(transid.data()),
						_bstr_t(signdCert.data()), _bstr_t(msg.data()));
					return COleControl::WindowProc(message, wParam, Lparam);
				}

				std::string authResult = ds["data"]["authResult"];
				std::string transid = ds["data"]["transid"];
				std::string token = ds["data"]["token"];
				std::string msg = UTF8EncodingGBK(ds["msg"]);

				switch (action) {
				case 1:
				{
					std::string mobile = ds["data"]["mobile"];
					std::string userID = ds["data"]["userId"];
					std::string userName = UTF8EncodingGBK(ds["data"]["userName"]);
					OutputDebugStringA(Poco::format("%s,%s,%s,%s,%s,%s,%s\n", authResult, transid, token, mobile, userName, userID, msg).c_str());
					RS_CloudLoginAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(mobile.data()), _bstr_t(userName.data()), _bstr_t(userID.data()), _bstr_t(msg.data()));
				}
				break;
				case 2:
				{
					RS_CloudEncAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(msg.data()));
				}
				break;
				case 3:
				{
					RS_CloudDevAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(msg.data()));
				}
				break;
				case 4:
				{
					RS_CloudSealAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(msg.data()));
				}
				break;
				case 5:
				{
					RS_CloudSealAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(msg.data()));
				}
				break;
				case 6:
				{
					RS_CloudGetCertAuthEvent(_bstr_t(authResult.data()), _bstr_t(transid.data()), _bstr_t(token.data()),
						_bstr_t(msg.data()));
				}
				break;
				default:
					throw Poco::InvalidArgumentException("UM_EVENT", action);
				}
			}
		}
		catch (Poco::JSON::JSONException&)
		{
			poco_unexpected();
		}
	}
	return COleControl::WindowProc(message, wParam, Lparam);
}
// CRSAsyncCtrl::OnDraw - Drawing function

void CRSAsyncCtrl::OnDraw(
	CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */)
{
	if (!pdc)
		return;

	// TODO: Replace the following code with your own drawing code.
	pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	pdc->Ellipse(rcBounds);
	OutputDebugStringA("CRSyncControlCtrl::OnDraw()\n");
}

// CRSAsyncCtrl::DoPropExchange - Persistence support

void CRSAsyncCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}


// CRSAsyncCtrl::GetControlFlags -
// Flags to customize MFC's implementation of ActiveX controls.
//
//DWORD CRSAsyncCtrl::GetControlFlags()
//{
//	DWORD dwFlags = COleControl::GetControlFlags();
//
//
//	// The control can activate without creating a window.
//	// TODO: when writing the control's message handlers, avoid using
//	//		the m_hWnd member variable without first checking that its
//	//		value is non-null.
//	dwFlags |= windowlessActivate;
//	return dwFlags;
//}


// CRSAsyncCtrl::OnResetState - Reset control to default state

void CRSAsyncCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


// CRSAsyncCtrl message handlers
#include "Poco/Thread.h"

void CRSAsyncCtrl::InitializeWebSocket()
{
	OutputDebugStringA("Begin WebSocket ready \n");

	if(_pWSChannel)
		_pWSChannel->ready(m_hWnd);

	InternalSetReadyState(READYSTATE_COMPLETE);
	OutputDebugStringA("End WebSocket ready \n");
}

VARIANT_BOOL CRSAsyncCtrl::TestEvent(USHORT index)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: Add your dispatch handler code here
	switch (index) {
	case eventidRS_CloudGetCertAuthEvent:
		RS_CloudGetCertAuthEvent(_T("authResult"), _T("transid"), _T("token"), _T("msg"));
		break;
	case eventidRS_CloudDevAuthEvent:
		RS_CloudDevAuthEvent(_T("authResult"), _T("transid"), _T("token"), _T("msg"));
		break;
	case eventidRS_CloudEncAuthEvent:
		RS_CloudEncAuthEvent(_T("authResult"), _T("transid"), _T("token"), _T("msg"));
		break;
	case eventidRS_CloudLoginAuthEvent:
		RS_CloudLoginAuthEvent(_T("authResult"), _T("transid"), _T("token"), _T("mobile"), _T("userName"), _T("userID"), _T("msg"));
		break;
	case eventidRS_CloudGetSignResultEvent:
		RS_CloudGetSignResultEvent(_T("signResult"), _T("signdMsg"), _T("transid"), _T("signdCert"), _T("msg"));
		break;
	case eventidRS_CloudSealAuthEvent:
		RS_CloudSealAuthEvent(_T("authResult"), _T("transid"), _T("token"), _T("msg"));
		break;
	default:
		InitializeWebSocket();
		return VARIANT_FALSE;
	}

	return VARIANT_TRUE;
}
