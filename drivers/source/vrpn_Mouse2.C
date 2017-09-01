/* file:	vrpn_Mouse.cpp
 * author:	Mike Weiblen mew@mew.cx 2004-01-14
 * copyright:	(C) 2003,2004 Michael Weiblen
 * license:	Released to the Public Domain.
 * depends:	gpm 1.19.6, VRPN 06_04
 * tested on:	Linux w/ gcc 2.95.4
 * references:  http://mew.cx/ http://vrpn.org/
 *              http://linux.schottelius.org/gpm/
*/

#include <stdio.h>                      // for NULL, fprintf, printf, etc
#include <string.h>                     // for strncpy

#include "../../../vrpn_BaseClass.h"             // for ::vrpn_TEXT_ERROR
#include "../../../vrpn_Mouse.h"


#if !( defined(_WIN32) && defined(VRPN_USE_WINSOCK_SOCKETS) )
#  include <sys/select.h>                 // for select, FD_ISSET, FD_SET, etc
#endif

#ifdef	_WIN32
#include <windows.h>

#pragma comment (lib, "user32.lib")

// Fix sent in by Andrei State to make this compile under Visual Studio 6.0.
// If you need this, you also have to copy multimon.h from the DirectX or
// another Windows SDK into a place where the compiler can find it.
#ifndef SM_XVIRTUALSCREEN
#define COMPILE_MULTIMON_STUBS
#include "multimon.h"
#endif

#endif

///////////////////////////////////////////////////////////////////////////

vrpn_Mouse::vrpn_Mouse( const char* name, vrpn_Connection * cxn ) :
	vrpn_Analog( name, cxn ),
	vrpn_Button_Filter( name, cxn )
{
    int i;

    // initialize the vrpn_Analog
    vrpn_Analog::num_channel = 2;
    for( i = 0; i < vrpn_Analog::num_channel; i++) {
	vrpn_Analog::channel[i] = vrpn_Analog::last[i] = 0;
    }

    // initialize the vrpn_Button_Filter
    vrpn_Button_Filter::num_buttons = 3;
    for( i = 0; i < vrpn_Button_Filter::num_buttons; i++) {
	vrpn_Button_Filter::buttons[i] = vrpn_Button_Filter::lastbuttons[i] = 0;
    }

}

///////////////////////////////////////////////////////////////////////////

vrpn_Mouse::~vrpn_Mouse() {

}

///////////////////////////////////////////////////////////////////////////

void vrpn_Mouse::mainloop() {
    get_report();
    server_mainloop();
}

///////////////////////////////////////////////////////////////////////////

int vrpn_Mouse::get_report() {

    const unsigned LEFT_MOUSE_BUTTON = 0x01;
    const unsigned RIGHT_MOUSE_BUTTON = 0x02;
    const unsigned MIDDLE_MOUSE_BUTTON = 0x04;

    // Find out if the mouse buttons are pressed.
    if (0x80000 & GetKeyState(LEFT_MOUSE_BUTTON)) {
      vrpn_Button::buttons[0] = 1;
    } else {
      vrpn_Button::buttons[0] = 0;
    }
    if (0x80000 & GetKeyState(MIDDLE_MOUSE_BUTTON)) {
      vrpn_Button::buttons[1] = 1;
    } else {
      vrpn_Button::buttons[1] = 0;
    }
    if (0x80000 & GetKeyState(RIGHT_MOUSE_BUTTON)) {
      vrpn_Button::buttons[2] = 1;
    } else {
      vrpn_Button::buttons[2] = 0;
    }

    // Find the position of the cursor in X,Y with range 0..1 across the screen
    POINT curPos;
    GetCursorPos(&curPos);
    vrpn_Analog::channel[0] = (vrpn_float64)(curPos.x - GetSystemMetrics(SM_XVIRTUALSCREEN)) / GetSystemMetrics(SM_CXVIRTUALSCREEN);
    vrpn_Analog::channel[1] = (vrpn_float64)(curPos.y - GetSystemMetrics(SM_YVIRTUALSCREEN)) / GetSystemMetrics(SM_CYVIRTUALSCREEN);

    vrpn_gettimeofday( &timestamp, NULL );
    report_changes();
    return 1;

}

///////////////////////////////////////////////////////////////////////////

void vrpn_Mouse::report_changes( vrpn_uint32 class_of_service ) {
    vrpn_Analog::timestamp = timestamp;
    vrpn_Button_Filter::timestamp = timestamp;

    vrpn_Analog::report_changes( class_of_service );
    vrpn_Button_Filter::report_changes();
}

///////////////////////////////////////////////////////////////////////////

void vrpn_Mouse::report( vrpn_uint32 class_of_service ) {
    vrpn_Analog::timestamp = timestamp;
    vrpn_Button_Filter::timestamp = timestamp;

    vrpn_Analog::report( class_of_service );
    vrpn_Button_Filter::report_changes();
}


/*EOF*/
