// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include <DockTabFrame.h>
#include "ClientViewListener.h"
#include "RichEditView.h"
#include "ClientWindowListView.h"
#include "NewEditViewDialog.h"

using namespace DockSplitTab;

class CMainFrame
	: public CFrameWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
	, public FrameListener
	, public ClientViewListener
{

//------------ protected members
protected:
	
	CAtlMap< HWND, ClientView*> clientViews;
	CAtlMap< HWND, RichEdit*>   richEdits;
	Frame                       frame;
	CCommandBarCtrl             cmdBar;
	long                        counter; // client window counter
	ClientWindowListView        clientWindowListView;
	
	// WM_USER events
	static const int eventCloseClientView = WM_USER+1;
	
	ClientView* createClientWindow( const TCHAR* caption, HWND hWnd, const TCHAR* toolTip, int imageIndex = -1) {
		
		ClientView* result = new ClientView( caption, hWnd, toolTip, imageIndex);
		this->clientViews[ hWnd] = result;
		this->clientWindowListView.append( result);
		return result;
	}
	
	ClientView* lookUpClientWindow( HWND clientWnd) {
		
		CAtlMap< HWND, ClientView*>::CPair* pair = this->clientViews.Lookup( clientWnd);
		if ( NULL == pair)
			return NULL;
		
		return pair->m_value;
	}
	
	RichEdit* lookUpRichEdit( HWND richEditWnd) {
		
		CAtlMap< HWND, RichEdit*>::CPair* pair = this->richEdits.Lookup( richEditWnd);
		if ( NULL == pair)
			return NULL;
		
		return pair->m_value;
	}
	
public:
	
	// Constructor/destructor
	CMainFrame()
		: counter( 0)
		, frame( this)
		, clientWindowListView( this)
	{}
	
	~CMainFrame() {
		
		POSITION position = this->richEdits.GetStartPosition();
		if ( NULL != position)
			do
				delete this->richEdits.GetNextValue( position);
			while ( NULL != position);
		this->richEdits.RemoveAll();
		
		position = this->clientViews.GetStartPosition();
		if ( NULL != position)
			do
				delete this->clientViews.GetNextValue( position);
			while ( NULL != position);
		this->clientViews.RemoveAll();
	}
	
	// ClientViewListener interface
	const ClientView* get( HWND clientViewWnd) const {
		
		return this->frame.getClientView( clientViewWnd);
	}
	
	void clientViewHide( HWND clientViewWnd) {
		
		ATLASSERT( ::IsWindow( clientViewWnd));
		
		if ( 0 == ::SendMessage( clientViewWnd, WM_CLOSE, 0, 0)) {
			
			if ( clientViewWnd == this->clientWindowListView.m_hWnd) {
				
				// just hide client list view window
				ClientView* clientView = this->frame.getClientView( clientViewWnd);
				
				this->frame.detachClientView( clientViewWnd);
			} else
				// to destroy the client view we get out of client view call stack
				this->PostMessage( eventCloseClientView, 0, reinterpret_cast<LPARAM>( clientViewWnd));
		}
		return;
	}
	
	void clientViewDockable( HWND clientViewWnd) {
		
		ATLASSERT( ::IsWindow( clientViewWnd));
		
		this->frame.toggleClientViewState( clientViewWnd, ClientView::stateDockable);
		this->clientWindowListView.update( clientViewWnd);
		return;
	}
	
	void clientViewFloating( HWND clientViewWnd) {
		
		ATLASSERT( ::IsWindow( clientViewWnd));
		
		this->frame.toggleClientViewState( clientViewWnd, ClientView::stateFloating);
		this->clientWindowListView.update( clientViewWnd);
		return;
	}
	
	// FrameListener interface
	void clientDetached( ClientView* clientView) {
		
		RichEdit* richEdit = this->lookUpRichEdit( clientView->wnd);
		if ( NULL != richEdit) {
			
			// remove rich edit view
			this->clientViews.RemoveKey( richEdit->m_hWnd);
			this->clientWindowListView.remove( clientView);
			delete clientView;
			
			this->richEdits.RemoveKey( richEdit->m_hWnd);
			richEdit->DestroyWindow();
			delete richEdit;
		}
		return;
	}
	
	// Message handlers
	DECLARE_FRAME_WND_CLASS( "CMainFrameWindows", IDR_MAINFRAME)
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) {
		
		return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
	}
	
	virtual BOOL OnIdle() {
		
		this->UIEnable( ID_VIEW_TOPDOCKPANE,    this->frame.dockPaneExists( dockTOP));
		this->UIEnable( ID_VIEW_BOTTOMDOCKPANE, this->frame.dockPaneExists( dockBOTTOM));
		this->UIEnable( ID_VIEW_LEFTDOCKPANE,   this->frame.dockPaneExists( dockLEFT));
		this->UIEnable( ID_VIEW_RIGHTDOCKPANE,  this->frame.dockPaneExists( dockRIGHT));
		
		this->UISetCheck( ID_VIEW_TOPDOCKPANE,    this->frame.dockPaneVisible( dockTOP));
		this->UISetCheck( ID_VIEW_BOTTOMDOCKPANE, this->frame.dockPaneVisible( dockBOTTOM));
		this->UISetCheck( ID_VIEW_LEFTDOCKPANE,   this->frame.dockPaneVisible( dockLEFT));
		this->UISetCheck( ID_VIEW_RIGHTDOCKPANE,  this->frame.dockPaneVisible( dockRIGHT));
		
		this->UISetCheck( ID_VIEW_CLIENTWINDOWLIST, this->frame.clientViewAttached( this->clientWindowListView.m_hWnd) && this->clientWindowListView.IsWindowVisible());
		
		this->UIUpdateToolBar( TRUE);
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP( CMainFrame)
		
		UPDATE_ELEMENT( ID_VIEW_TOOLBAR,          UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_STATUS_BAR,       UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_TOPDOCKPANE,      UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_BOTTOMDOCKPANE,   UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_LEFTDOCKPANE,     UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_RIGHTDOCKPANE,    UPDUI_MENUPOPUP)
		UPDATE_ELEMENT( ID_VIEW_CLIENTWINDOWLIST, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()
	
	BEGIN_MSG_MAP(CMainFrame)
		
		MESSAGE_HANDLER( WM_CREATE,            OnCreate)
		MESSAGE_HANDLER( eventCloseClientView, OnCloseClientView)
		
		COMMAND_ID_HANDLER( ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER( ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER( ID_FILE_NEW_WINDOW, OnFileNewWindow)
		
		COMMAND_ID_HANDLER( ID_VIEW_TOOLBAR,        OnViewToolBar)
		COMMAND_ID_HANDLER( ID_VIEW_STATUS_BAR,     OnViewStatusBar)
		COMMAND_ID_HANDLER( ID_VIEW_TOPDOCKPANE,    OnViewTopDockPane)
		COMMAND_ID_HANDLER( ID_VIEW_BOTTOMDOCKPANE, OnViewBottomDockPane)
		COMMAND_ID_HANDLER( ID_VIEW_LEFTDOCKPANE,   OnViewLeftDockPane)
		COMMAND_ID_HANDLER( ID_VIEW_RIGHTDOCKPANE,  OnViewRightDockPane)
		
		COMMAND_ID_HANDLER( ID_VIEW_CLIENTWINDOWLIST, OnViewClientWindowList)
		
		COMMAND_ID_HANDLER( ID_APP_ABOUT, OnAppAbout)
		
		CHAIN_MSG_MAP( CUpdateUI< CMainFrame>)
		CHAIN_MSG_MAP( CFrameWindowImpl< CMainFrame>)
	END_MSG_MAP()
	
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/) {
		
		this->CreateSimpleReBar( ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		
		// Create command bar window
		HWND hWndCmdBar = this->cmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		this->cmdBar.AttachMenu(GetMenu());
		// load command bar images
		this->cmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		this->SetMenu(NULL);
		this->AddSimpleReBarBand( hWndCmdBar);
		
		// Create toolbar and add it to rebar
		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd, IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
		this->AddSimpleReBarBand( hWndToolBar, NULL, TRUE);
		
		// Status bar
		CreateSimpleStatusBar();
		
		this->UIAddToolBar(hWndToolBar);
		this->UISetCheck( ID_VIEW_TOOLBAR, 1);
		this->UISetCheck( ID_VIEW_STATUS_BAR, 1);
		
		// Create client window
		this->m_hWndClient = this->frame.create( m_hWnd, rcDefault);
	    
		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);
		
		// Create client window list view and add it into DockSplitFrame
		this->clientWindowListView.create( this->m_hWnd);
		return 0;
	}
	
	LRESULT OnCloseClientView(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/) {
		
		HWND clientViewWnd = reinterpret_cast< HWND>( lParam);
		ClientView* clientView = this->frame.getClientView( clientViewWnd);
		if ( NULL == clientView)
			return 0;
		
		this->frame.detachClientView( clientViewWnd);
		// DockSplitTab::detachClientView does not call FrameListener::clientDetached event handler
		// so need to do it explicitly
		this->clientDetached( clientView);
		return 0;
	}
	
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->PostMessage( WM_CLOSE);
		return 0;
	}
	
	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		NewEditViewDialog newEditDialog;
		newEditDialog.DoModal();
		
		NewEditViewDialog::Choice choice = newEditDialog.getSelected();
		if ( NewEditViewDialog::choiceUnknown == choice)
			return 0;
		
		RichEdit* richEdit = new RichEdit( this);
		richEdit->create( this->m_hWnd);
		this->richEdits[richEdit->m_hWnd] = richEdit;
		
		this->counter++;
		CString caption;
		caption.Format( _T("Client view #%d"), this->counter);
		CString toolTip;
		toolTip.Format( _T("Client view #%d tool tip"), this->counter);
		CString freeText;
		freeText.Format( _T("Client view #%d free text\n"), this->counter);
		richEdit->AppendText( freeText);
		richEdit->SetModify( FALSE);
		
		ClientView* clientView = this->createClientWindow( caption, richEdit->m_hWnd, toolTip);
		switch ( choice) {
		case NewEditViewDialog::choiceMainPane:
			
			this->frame.addClientView( clientView);
			break;
			
		case NewEditViewDialog::choiceLeftDockPane:
			
			this->frame.dockClientView( DockSplitTab::dockLEFT, clientView);
			break;
			
		case NewEditViewDialog::choiceTopDockPane:
			
			this->frame.dockClientView( DockSplitTab::dockTOP, clientView);
			break;
			
		case NewEditViewDialog::choiceRightDockPane:
			
			this->frame.dockClientView( DockSplitTab::dockRIGHT, clientView);
			break;
			
		case NewEditViewDialog::choiceBottomDockPane:
			
			this->frame.dockClientView( DockSplitTab::dockBOTTOM, clientView);
			break;
			
		case NewEditViewDialog::choiceFloatFrame:
			{
				CRect rect;
				this->frame.GetWindowRect( rect);
				this->frame.floatClientView( rect, clientView);
			}
			break;
			
		}
		this->clientWindowListView.update( clientView->wnd);
		return 0;
	}
	
	LRESULT OnFileNewWindow(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		::PostThreadMessage(_Module.m_dwMainThreadID, WM_USER, 0, 0L);
		return 0;
	}
	
	LRESULT OnViewTopDockPane( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->frame.showDockPane( dockTOP, !this->frame.dockPaneVisible( dockTOP));
		UISetCheck( ID_VIEW_TOPDOCKPANE, this->frame.dockPaneVisible( dockTOP));
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewBottomDockPane( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->frame.showDockPane( dockBOTTOM, !this->frame.dockPaneVisible( dockBOTTOM));
		UISetCheck( ID_VIEW_BOTTOMDOCKPANE, this->frame.dockPaneVisible( dockBOTTOM));
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewLeftDockPane( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->frame.showDockPane( dockLEFT, !this->frame.dockPaneVisible( dockLEFT));
		UISetCheck( ID_VIEW_LEFTDOCKPANE, this->frame.dockPaneVisible( dockLEFT));
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewRightDockPane( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		this->frame.showDockPane( dockRIGHT, !this->frame.dockPaneVisible( dockRIGHT));
		UISetCheck( ID_VIEW_RIGHTDOCKPANE, this->frame.dockPaneVisible( dockRIGHT));
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewClientWindowList( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		// check if client window list is detached
		HWND listViewWnd = this->clientWindowListView.m_hWnd;
		bool visible = this->frame.clientViewAttached( listViewWnd)
		            && this->frame.clientViewVisible( listViewWnd)
		             ;
		if ( visible)
			
			this->frame.detachClientView( listViewWnd);
		else {
			
			if ( this->frame.clientViewAttached( listViewWnd))
				
				this->frame.setFocusTo( listViewWnd);
			else {
				
				ClientView* clientView = this->lookUpClientWindow( listViewWnd);
				if ( NULL == clientView) {
					
					clientView = this->createClientWindow( "Client View list"
														, listViewWnd
														, "Client view list"
														, -1
														);
					this->frame.dockClientView( dockRIGHT, clientView);
					this->clientWindowListView.update( clientView->wnd);
				} else
					this->frame.attachClientView( clientView);
			}
		}
		this->UISetCheck( ID_VIEW_CLIENTWINDOWLIST, !visible);
		return 0;
	}
	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}
	
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}
};
