// RSAsync.cpp : Implementation of CRSAsyncApp and DLL registration.

#include "pch.h"
#include "framework.h"
#include "RSAsync.h"
#include "Cathelp.h"
#include "objsafe.h"
#include <cassert>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CRSAsyncApp theApp;

const GUID CDECL _tlid = {0x7eeb6853,0xeb79,0x4e03,{0xad,0xa7,0xb5,0x60,0x86,0xb2,0xf0,0xe1}};
const GUID CDECL _RSEvent = { 0xa5104978,0x463b,0x4d42,{0xb4,0x30,0x06,0x14,0x14,0xe1,0x1f,0x2f} };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CRSAsyncApp::InitInstance - DLL initialization

BOOL CRSAsyncApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
		AfxEnableControlContainer();
	}

	return bInit;
}



// CRSAsyncApp::ExitInstance - DLL termination

int CRSAsyncApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	RegisterControlsSafely();

	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	UnRegisterControlsSafely();

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


void RegisterControlsSafely()
{
	HRESULT hr;

	/// 将控件标记为可安全编写脚本

	hr = CreateComponentCategory(CATID_SafeForInitializing,
		L"Controls safely initializable from persistent data!");
	assert(SUCCEEDED(hr));

	hr = RegisterCLSIDInCategory(_RSEvent,
		CATID_SafeForInitializing);
	assert(SUCCEEDED(hr));

	/// 将控件标记为可安全编写脚本

	hr = CreateComponentCategory(CATID_SafeForScripting,
		L"Controls safely scriptable!");
	assert(SUCCEEDED(hr));

	hr = RegisterCLSIDInCategory(_RSEvent,
		CATID_SafeForScripting);
	assert(SUCCEEDED(hr));
}

void UnRegisterControlsSafely()
{
	HRESULT hr;

	/// 删除控件初始化安全入口

	hr = UnRegisterCLSIDInCategory(_RSEvent, CATID_SafeForInitializing);
	assert(SUCCEEDED(hr));

	/// 删除控件脚本安全入口 

	hr = UnRegisterCLSIDInCategory(_RSEvent, CATID_SafeForScripting);
	assert(SUCCEEDED(hr));
}