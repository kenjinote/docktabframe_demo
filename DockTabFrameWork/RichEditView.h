#pragma once

#ifndef __RICHEDITVIEW_H__
#define __RICHEDITVIEW_H__

#ifndef __ATLCTRLS_H__
	#error RichEditView.h requires atlctrls.h to be included first
#endif

using namespace DockSplitTab;

class DockSplitTab::Frame;

class RichEdit
	: public CWindowImpl<RichEdit, WTL::CRichEditCtrl>{
protected:
	
	ClientViewListener* clientViewListener;
	
public:
	
	// constructor
	RichEdit( ClientViewListener* clientViewListener)
		: clientViewListener( clientViewListener)
	{}
	
	// public interface
	void create( HWND parentWnd) {
		
		this->Create( parentWnd
		            , rcDefault
		            , NULL
		            , WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
		            | WS_HSCROLL | WS_VSCROLL | WS_BORDER
		            | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE
		            );
		return;
	}
	
	// Message map handlers
	DECLARE_WND_SUPERCLASS( _T("DockSplitTab::RichEdit"), CRichEditCtrl::GetWndClassName())	
	
	BEGIN_MSG_MAP( RichEdit)
		
		MESSAGE_HANDLER( WM_CONTEXTMENU, OnContextMenu)
		MESSAGE_HANDLER( WM_CLOSE,       OnClose)
	
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_DOCKABLE, OnClientViewDockable)
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_HIDE,     OnClientViewHide)
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_FLOATING, OnClientViewFloating)
	
	END_MSG_MAP()
	
	LRESULT OnContextMenu( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		// DockSplitTab::Frame sends WM_CONTEXTMENU message to client view windows
		const ClientView* clientView = this->clientViewListener->get( this->m_hWnd);
		if ( NULL == clientView)
			return 0;
		
		CPoint point( LOWORD( lParam), HIWORD( lParam));
		
		CMenu loadMenu;
		loadMenu.LoadMenu( MAKEINTRESOURCE( IDR_CONTEXTMENU));
		CMenu contextMenu = loadMenu.GetSubMenu( 0);
		
		switch ( clientView->getState()) {
		case ClientView::stateDockable:
			contextMenu.CheckMenuItem( ID_CLIENTVIEWLIST_DOCKABLE, MF_CHECKED);
			break;
		case ClientView::stateFloating:
			contextMenu.CheckMenuItem( ID_CLIENTVIEWLIST_FLOATING, MF_CHECKED);
			break;
		}
		
		contextMenu.TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON
		                          , point.x, point.y
								  , this->m_hWnd, NULL
		                          );
		return 0;
	}
	
	// DockFloatSplitTab Framework sends WM_CLOSE message
	LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		// do not call default event handler
		return this->GetModify()
			&& IDYES != this->MessageBox( _T("Close the rich edit?")
			                            , _T("DockSplitTab Framework")
			                            , MB_YESNO | MB_TOPMOST | MB_ICONQUESTION | MB_APPLMODAL | MB_SETFOREGROUND
			                            )
			    ;
	}
	
	LRESULT OnClientViewDockable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->clientViewListener->clientViewDockable( this->m_hWnd);
		return 0;
	}
	
	LRESULT OnClientViewHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->clientViewListener->clientViewHide( this->m_hWnd);
		return 0;
	}
	
	LRESULT OnClientViewFloating(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->clientViewListener->clientViewFloating( this->m_hWnd);
		return 0;
	}
};
	

#endif // __RICHEDITVIEW_H__
