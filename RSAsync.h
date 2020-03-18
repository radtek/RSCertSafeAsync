#pragma once

// RSAsync.h : main header file for RSAsync.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols


// CRSAsyncApp : See RSAsync.cpp for implementation.

class CRSAsyncApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

extern void RegisterControlsSafely();
extern void UnRegisterControlsSafely();

