// RSAsyncPropPage.cpp : Implementation of the CRSAsyncPropPage property page class.

#include "pch.h"
#include "framework.h"
#include "RSAsync.h"
#include "RSAsyncPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CRSAsyncPropPage, COlePropertyPage)

// Message map

BEGIN_MESSAGE_MAP(CRSAsyncPropPage, COlePropertyPage)
END_MESSAGE_MAP()

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CRSAsyncPropPage, "MFCACTIVEXCONT.RSAsyncPropPage.1",
	0x6b8e5d4a,0xd9ea,0x4ca9,0x90,0xe6,0x73,0x6b,0xbd,0x67,0x9d,0x01)

// CRSAsyncPropPage::CRSAsyncPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CRSAsyncPropPage

BOOL CRSAsyncPropPage::CRSAsyncPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_MFCACTIVEXDEMO_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, nullptr);
}

// CRSAsyncPropPage::CRSAsyncPropPage - Constructor

CRSAsyncPropPage::CRSAsyncPropPage() :
	COlePropertyPage(IDD, IDS_MFCACTIVEXDEMO_PPG_CAPTION)
{
}

// CRSAsyncPropPage::DoDataExchange - Moves data between page and properties

void CRSAsyncPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}

// CRSAsyncPropPage message handlers
