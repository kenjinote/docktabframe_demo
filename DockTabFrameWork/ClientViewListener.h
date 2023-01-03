#pragma once

#ifndef __CLIENTVIEWLISTENER_H__
#define __CLIENTVIEWLISTENER_H__

using namespace DockSplitTab;

class ClientViewListener {
public:
	
	virtual const ClientView* get(   HWND clientViewWnd) const = 0;
	virtual void clientViewHide(     HWND clientViewWnd)       = 0;
	virtual void clientViewDockable( HWND clientViewWnd)       = 0;
	virtual void clientViewFloating( HWND clientViewWnd)       = 0;
};


#endif
