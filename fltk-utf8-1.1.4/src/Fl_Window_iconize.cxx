//
// "$Id: Fl_Window_iconize.cxx,v 1.5.2.3.2.5 2003/01/30 21:43:00 easysw Exp $"
//
// Window minification code for the Fast Light Tool Kit (FLTK).
//
// Copyright 1998-2003 by Bill Spitzak and others.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

#include <FL/x.H>

extern char fl_show_iconic; // in Fl_x.cxx

void Fl_Window::iconize() {
  if (!shown()) {
    fl_show_iconic = 1;
    show();
  } else {
#ifdef WIN32
    ShowWindow(i->xid, SW_SHOWMINNOACTIVE);
#elif defined(__MACOS__)
    CollapseWindow( i->xid, true );
#elif defined(NANO_X)

#elif DJGPP

#else
    XIconifyWindow(fl_display, i->xid, fl_screen);
#endif
  }
}

//
// End of "$Id: Fl_Window_iconize.cxx,v 1.5.2.3.2.5 2003/01/30 21:43:00 easysw Exp $".
//
