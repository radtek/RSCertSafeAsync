#pragma once

// RSAsyncPropPage.h : Declaration of the CRSAsyncPropPage property page class.


// CRSAsyncPropPage : See RSAsyncPropPage.cpp for implementation.

class CRSAsyncPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CRSAsyncPropPage)
	DECLARE_OLECREATE_EX(CRSAsyncPropPage)

// Constructor
public:
	CRSAsyncPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_MFCACTIVEXDEMO };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

