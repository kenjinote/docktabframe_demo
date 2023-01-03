// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

class CAboutDlg
	: public CDialogImpl< CAboutDlg>
	, public CWinDataExchange< CAboutDlg>
{
private:
	
	CHyperLink igorKatraevLink;
	CHyperLink bjarkeViksoelLink;
	CHyperLink danielBowenLink;

public:
	
	enum { IDD = IDD_ABOUTBOX};
	
	BEGIN_DDX_MAP( CAboutDlg)
		DDX_CONTROL( IDC_IGORKATRAYEVLINK, this->igorKatraevLink)
		DDX_CONTROL( IDC_BJARKEVIKSOELINK, this->bjarkeViksoelLink)
		DDX_CONTROL( IDC_DANIELBOWENLINK,  this->danielBowenLink)
	END_DDX_MAP()
	
	BEGIN_MSG_MAP(CAboutDlg)
		
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		
		COMMAND_ID_HANDLER(IDOK,     OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		
		this->DoDataExchange();
		
		this->igorKatraevLink.SetHyperLink( _T("http://www.katraev.ru"));
		this->bjarkeViksoelLink.SetHyperLink( _T("mailto://bjarke@viksoe.dk") );
		this->danielBowenLink.SetHyperLink( _T("mailto://dbowen@es.com"));
		this->CenterWindow( this->GetParent());
		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		EndDialog(wID);
		return 0;
	}
};
