#ifndef __NEWEDITVIEWDIALOG_H__
#define __NEWEDITVIEWDIALOG_H__

#pragma once

#ifndef __cplusplus
	#error NewEditViewDialog.h requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error NewEditViewDialog.h requires atlwin.h to be included first
#endif

#ifndef __ATLDDX_H__
	#error NewEditViewDialog.h requires atlddx.h to be included first
#endif

class NewEditViewDialog
	: public CDialogImpl< NewEditViewDialog>
	, public CWinDataExchange< NewEditViewDialog>
{
public:
	
	enum Choice {
		  choiceUnknown        = -1
		, choiceMainPane       = 0 //IDC_MAINPANE_RADIO
		, choiceLeftDockPane   = 1 //IDC_LEFTDOCK_RADIO
		, choiceTopDockPane    = 2 //IDC_TOPDOCK_RADIO
		, choiceRightDockPane  = 3 //IDC_RIGHTDOCK_RADIO
		, choiceBottomDockPane = 4 //IDC_BOTTOMDOCK_RADIO
		, choiceFloatFrame     = 5 //IDC_FLOATFRAME_RADIO
	};
	
private:

	int selection;
public:
	
	// public interface
	Choice getSelected() {
		
		return static_cast<Choice>( this->selection);
	}
	
	// Message event hanlers
	enum { IDD = IDD_NEWEDITVIEW };
	
	BEGIN_DDX_MAP( NewEditViewDialog)
		DDX_RADIO( IDC_MAINPANE_RADIO, selection)
	END_DDX_MAP()
	
	BEGIN_MSG_MAP( NewEditViewDialog)
		
		MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog)
		
		COMMAND_ID_HANDLER( IDOK,     OnOkClose)
		COMMAND_ID_HANDLER( IDCANCEL, OnCancelClose)
	END_MSG_MAP()
	
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		
		this->selection = choiceMainPane;
		this->DoDataExchange();
		this->CenterWindow( this->GetParent());
		return TRUE;
	}
	
	LRESULT OnCancelClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->selection = choiceUnknown;
		this->EndDialog(wID);
		return 0;
	}
	
	LRESULT OnOkClose(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->DoDataExchange( TRUE);
		this->EndDialog(wID);
		return 0;
	}
	LRESULT OnBnClickedFloatframeRadio(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
}; // class NewEditViewDialog


#endif // __NEWEDITVIEWDIALOG_H__
