//
// "$Id: Fl_Box.cxx,v 1.4.2.3.2.4 2003/01/30 21:41:23 easysw Exp $"
//
// Box widget for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Widget.H>
#include <FL/Fl_Box.H>

void Fl_Box::draw() {
  draw_box();
  draw_label();
}

int Fl_Box::handle(int event) {
  if (event == FL_ENTER || event == FL_LEAVE) return 1;
  else return 0;
}


//
// End of "$Id: Fl_Box.cxx,v 1.4.2.3.2.4 2003/01/30 21:41:23 easysw Exp $".
//
