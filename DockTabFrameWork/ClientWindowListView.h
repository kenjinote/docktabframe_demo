#pragma once

#ifndef __CLIENTWINDOWLISTVIEW_H__
#define __CLIENTWINDOWLISTVIEW_H__

#ifndef __ATLCTRLS_H__
	#error ClientWindowListView.h requires atlctrls.h to be included first
#endif

#ifndef __DOCKTAB_FRAME_H__
	#error ClientWindowListView.h requires DockTabFrame.h to be included first
#endif

using namespace DockSplitTab;
using ClientView::stateDockable;
using ClientView::stateFloating;

class ClientWindowListView
	: public CWindowImpl< ClientWindowListView, CWindow> {
protected:
	
	typedef CWindowImpl< ClientWindowListView, CWindow> baseClass;
	
	enum SortDirection {
		  sortDesc = -1
		, sortNone = 0
		, sortAsc  = 1
	};
	
	CListViewCtrl        listView;
	SortDirection        sortDirection;
	ClientViewListener*  clientViewListener;
	
	static CBitmap bitmapUp;
	static CBitmap bitmapDown;
	static const TCHAR* const textFloating;
	static const TCHAR* const textDockable;
	
	void setColumnBitmap() {
		
		HDITEM item;
		item.mask=HDI_FORMAT;
		
		CHeaderCtrl header( this->listView.GetHeader());
		header.GetItem( 0, &item);
		
		if ( sortNone == this->sortDirection)
			
			item.fmt &= ~( HDF_BITMAP | HDF_BITMAP_ON_RIGHT);
		else {
			
			item.mask |= HDI_BITMAP;
			item.fmt  |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
			HBITMAP bitmap[] = { this->bitmapDown, NULL, this->bitmapUp};
			item.hbm = bitmap[ this->sortDirection+1];
		}
		
		header.SetItem( 0, &item);
	}
	
	static int CALLBACK comparer( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
		
		ClientWindowListView* thisInstance = reinterpret_cast< ClientWindowListView*>( lParamSort);
		
		CString text1;
		CString text2;
		
		thisInstance->listView.GetItemText( ( int) lParam1, 0, text1);
		thisInstance->listView.GetItemText( ( int) lParam2, 0, text2);
		
		switch ( thisInstance->sortDirection) {
		case sortAsc:  return  text1.CompareNoCase( text2);
		case sortDesc: return -text1.CompareNoCase( text2);
		}
		return 0;
	}
	
	HWND getClientViewWnd( int index) const {
		
		LVITEM item;
		item.iItem = index;
		item.mask  = LVIF_PARAM;
		if ( !this->listView.GetItem( &item))
			return NULL;
		return reinterpret_cast< HWND>( item.lParam);
	}
	
	const ClientView* getClientView( int index) const {
		
		LVITEM item;
		item.iItem = index;
		item.mask  = LVIF_PARAM;
		if ( !this->listView.GetItem( &item))
			return NULL;
		
		return this->clientViewListener->get( reinterpret_cast<HWND>( item.lParam));
	}
	
	int findItem( HWND clientViewWnd) const {
		
		LVFINDINFO findInfo;
		findInfo.flags       = LVFI_PARAM;
		findInfo.lParam      = reinterpret_cast<LPARAM>( clientViewWnd);
		findInfo.vkDirection = VK_DOWN;
		return this->listView.FindItem( &findInfo, -1);
	}
	
public:
	
	ClientWindowListView( ClientViewListener* clientViewListener)
		: sortDirection( sortNone)
		, clientViewListener( clientViewListener)
	{
		ATLASSERT( NULL != clientViewListener);
		
		if ( NULL == this->bitmapUp.m_hBitmap) {
			
			BOOL bFreeLibrary=FALSE;
			HINSTANCE hInstance=::GetModuleHandle(_T("shell32.dll"));
			if (!hInstance)	{ 
				
				// If there is no link to shell32, load it using LoadLibrary
				hInstance=::LoadLibrary(_T("shell32.dll"));
				bFreeLibrary=TRUE;
			}
			
			ATLASSERT(hInstance);
			
			if ( hInstance) {
				
				// Load bitmaps from shell32.dll
				this->bitmapUp.Attach( (HBITMAP)::LoadImage(hInstance,MAKEINTRESOURCE(133),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS));
				this->bitmapDown.Attach((HBITMAP)::LoadImage(hInstance,MAKEINTRESOURCE(134),IMAGE_BITMAP,0,0,LR_LOADMAP3DCOLORS));
				if (bFreeLibrary)
					::FreeLibrary( hInstance);
			}
		}
	}
	
	// public interface
	void create( HWND parentWnd) {
		
		this->Create( parentWnd);
		return;
	}
	
	void append( DockSplitTab::ClientView* clientView) {
		
		LVITEM item;
		item.mask     = LVIF_TEXT | LVIF_PARAM;
		item.iItem    = 0;
		item.iSubItem = 0;
		item.pszText  = (LPTSTR) LPCTSTR(  clientView->caption);
		item.lParam   = ( LPARAM) clientView->wnd;
		this->listView.InsertItem( &item);
		
		using DockSplitTab::ClientView::stateDockable;
		using DockSplitTab::ClientView::stateFloating;
		
		switch ( clientView->getState()) {
		case stateFloating:
		case stateDockable:
			this->listView.AddItem( 0, 1, clientView->getState() == stateFloating? textFloating: textDockable);
			break;
		}
		
		switch ( this->sortDirection) {
		case sortAsc:
		case sortDesc:
			this->listView.SortItemsEx( comparer, LPARAM( this));
			break;
		}
		return;
	}
	
	void update( HWND clientViewWnd) {
		
		int index = this->findItem( clientViewWnd);
		if ( -1 == index)
			return;
		
		const ClientView* clientView = this->clientViewListener->get( clientViewWnd);
		if ( NULL == clientView)
			return;
		
		const TCHAR* text = NULL;
		switch ( clientView->getState()) {
		case stateFloating:
		case stateDockable:
			text = clientView->getState() == stateFloating? textFloating: textDockable;
			break;
		}
		this->listView.SetItemText( index, 1, text);
	}
	
	void remove( DockSplitTab::ClientView* clientView) {
		
		int item = this->findItem( clientView->wnd);
		if ( item > -1)
			this->listView.DeleteItem( item);
		return;
	}
	
	// Message map handlers
	DECLARE_WND_CLASS( "ClientWindowListView")
	
	BEGIN_MSG_MAP( ClientWindowListView)
		
		MESSAGE_HANDLER( WM_CREATE,      OnCreate)
		MESSAGE_HANDLER( WM_CLOSE,       OnClose)
		MESSAGE_HANDLER( WM_SETFOCUS,    OnSetFocus)
		MESSAGE_HANDLER( WM_SIZE,        OnSize)
		MESSAGE_HANDLER( WM_CONTEXTMENU, OnContextMenu)
		
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_DOCKABLE, OnClientViewDockable)
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_HIDE,     OnClientViewHide)
		COMMAND_ID_HANDLER( ID_CLIENTVIEWLIST_FLOATING, OnClientViewFloating)
		
		NOTIFY_CODE_HANDLER( LVN_COLUMNCLICK, OnColumnClick)
	END_MSG_MAP()
	
	LRESULT OnSetFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		this->listView.SetFocus();
		return 0;
	}
	
	LRESULT OnContextMenu( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		HWND  contextWnd = reinterpret_cast<HWND>( wParam);
		CPoint point( LOWORD( lParam), HIWORD( lParam));
		
		CPoint pt = point;
		this->listView.ScreenToClient( &pt);
		int index = this->listView.HitTest( pt, NULL);
		if ( index == -1) {
			
			index = this->findItem( this->m_hWnd);
			if ( index == -1)
				return 0;
			this->listView.SelectItem( index);
		}
		
		const ClientView* clientView = this->getClientView( index);
		if ( NULL == clientView)
			return 0;
		
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
	
	LRESULT OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		WORD cx = LOWORD( lParam);
		WORD cy = HIWORD( lParam);
		
		this->listView.SetWindowPos( HWND_TOP, 0, 0, cx, cy, SWP_SHOWWINDOW);
		return 0;
	}
	
	LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		this->listView.Create( this->m_hWnd
		                     , rcDefault
		                     , NULL
		                     , WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
		                     | WS_HSCROLL | WS_VSCROLL | WS_BORDER
		                     | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL
		                     , LVS_EX_GRIDLINES | LVS_EX_HEADERDRAGDROP
		                     );
		this->listView.InsertColumn( 0, "Name",  LVCFMT_LEFT, 100, 0);
		this->listView.InsertColumn( 1, "State", LVCFMT_LEFT, 100, 1);
		return 0;
	}
	
	LRESULT OnClose( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled) {
		
		// override the default event handler to prevent from destroying window
		return 0;
	}
	
	LRESULT OnColumnClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled) {
		
		LPNMLISTVIEW pnmv = (LPNMLISTVIEW)pnmh;
		
		if ( 0 == pnmv->iSubItem) {
			
			switch ( this->sortDirection) {
			case sortNone: this->sortDirection = sortAsc;  break;
			case sortAsc:  this->sortDirection = sortDesc; break;
			case sortDesc: this->sortDirection = sortNone; break;
			}
			
			this->setColumnBitmap();
			this->listView.SortItemsEx( comparer, LPARAM( this));
		}
		bHandled = TRUE;
		return 0;
	}
	
	LRESULT OnClientViewDockable(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		int index = this->listView.GetSelectedIndex();
		if ( index == -1)
			return 0;
		HWND clientViewWnd = this->getClientViewWnd( index);
		if ( NULL == clientViewWnd)
			return 0;
		this->clientViewListener->clientViewDockable( clientViewWnd);
		return 0;
	}
	
	LRESULT OnClientViewHide(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		int index = this->listView.GetSelectedIndex();
		if ( index == -1)
			return 0;
		HWND clientViewWnd = this->getClientViewWnd( index);
		if ( NULL == clientViewWnd)
			return 0;
		this->clientViewListener->clientViewHide( clientViewWnd);
		return 0;
	}
	
	LRESULT OnClientViewFloating(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/) {
		
		int index = this->listView.GetSelectedIndex();
		if ( index == -1)
			return 0;
		HWND clientViewWnd = this->getClientViewWnd( index);
		if ( NULL == clientViewWnd)
			return 0;
		this->clientViewListener->clientViewFloating( clientViewWnd);
		return 0;
	}
};

CBitmap ClientWindowListView::bitmapUp   = NULL;
CBitmap ClientWindowListView::bitmapDown = NULL;
const TCHAR* const ClientWindowListView::textFloating = _T("Floating");
const TCHAR* const ClientWindowListView::textDockable = _T("Dockable");

#endif // __CLIENTWINDOWLISTVIEW_H__
