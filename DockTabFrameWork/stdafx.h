// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER		0x0600
//#define WINVER		0x0400
//#define _WIN32_WINNT	0x0400
#define _WIN32_IE	0x0600
#define _RICHEDIT_VER	0x0100

#include <atlbase.h>
#include <atlcoll.h>
#include <atlstr.h>

#include <atlwin.h>

#include <atlapp.h>

#include <atlddx.h>
#include <atltypes.h>

extern CAppModule _Module;

#include <atlsplit.h>
#include <atlgdi.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlctrlw.h>
