//
// "$Id: Fl_Choice.cxx,v 1.10.2.5.2.14 2003/08/24 13:09:06 easysw Exp $"
//
// Choice widget for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>
#include <FL/fl_draw.H>

// Emulates the Forms choice widget.  This is almost exactly the same
// as an Fl_Menu_Button.  The only difference is the appearance of the
// button: it draws the text of the current pick and a down-arrow.

extern char fl_draw_shortcut;

void Fl_Choice::draw() {
  int dx = Fl::box_dx(FL_DOWN_BOX);
  int dy = Fl::box_dy(FL_DOWN_BOX);
  int H = h() - 2 * dy;
  int W = (H > 20) ? 20 : H;
  int X = x() + w() - W - dx;
  int Y = y() + dy;
  int w1 = (W - 4) / 3; if (w1 < 1) w1 = 1;
  int x1 = X + (W - 2 * w1 - 1) / 2;
  int y1 = Y + (H - w1 - 1) / 2;

  if (Fl::scheme()) {
    draw_box(FL_UP_BOX, color());

    fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
    fl_polygon(x1, y1 + 3, x1 + w1, y1 + w1 + 3, x1 + 2 * w1, y1 + 3);
    fl_polygon(x1, y1 + 1, x1 + w1, y1 - w1 + 1, x1 + 2 * w1, y1 + 1);
  } else {
    draw_box(FL_DOWN_BOX, color());
    draw_box(FL_UP_BOX,X,Y,W,H,FL_GRAY);

    fl_color(active_r() ? labelcolor() : fl_inactive(labelcolor()));
    fl_polygon(x1, y1, x1 + w1, y1 + w1, x1 + 2 * w1, y1);
  }

  W += 2 * dx;

  if (mvalue()) {
    Fl_Menu_Item m = *mvalue();
    if (active_r()) m.activate(); else m.deactivate();

    // ERCO
    int xx = x() + dx, yy = y() + dy + 1, ww = w() - W, hh = H - 2;
    if ( Fl::scheme() )
    {
        Fl_Label l;
	l.value = m.text;
	l.image = 0;
	l.deimage = 0;
	l.type = m.labeltype_;
	l.font = m.labelsize_ || m.labelfont_ ? m.labelfont_ : uchar(textfont());
	l.size = m.labelsize_ ? m.labelsize_ : textsize();
	l.color= m.labelcolor_ ? m.labelcolor_ : textcolor();
	if (!m.active()) l.color = fl_inactive((Fl_Color)l.color);
	fl_draw_shortcut = 2; // hack value to make '&' disappear
	l.draw(xx+3, yy, ww>6 ? ww-6 : 0, hh, FL_ALIGN_LEFT);
	fl_draw_shortcut = 0;
	if ( Fl::focus() == this ) draw_focus(box(), xx, yy, ww, hh);
    }
    else
    {
	fl_clip(xx, yy, ww, hh);
	fl_draw_shortcut = 2; // hack value to make '&' disappear
	m.draw(xx, yy, ww, hh, this, Fl::focus() == this);
	fl_draw_shortcut = 0;
	fl_pop_clip();
    }
  }

  draw_label();
}

Fl_Choice::Fl_Choice(int X, int Y, int W, int H, const char *l)
: Fl_Menu_(X,Y,W,H,l) {
  align(FL_ALIGN_LEFT);
  when(FL_WHEN_RELEASE);
  textfont(FL_HELVETICA);
  box(FL_FLAT_BOX);
  down_box(FL_BORDER_BOX);
  color(FL_BACKGROUND2_COLOR);
}

int Fl_Choice::value(int v) {
  if (v < 0 || v >= (size() - 1)) return 0;
  if (!Fl_Menu_::value(v)) return 0;
  redraw();
  return 1;
}

int Fl_Choice::handle(int e) {
  if (!menu() || !menu()->text) return 0;
  const Fl_Menu_Item* v;
  switch (e) {
  case FL_ENTER:
  case FL_LEAVE:
    return 1;

  case FL_KEYBOARD:
    if (Fl::event_key() != ' ' ||
        (Fl::event_state() & (FL_SHIFT | FL_CTRL | FL_ALT | FL_META))) return 0;
  case FL_PUSH:
    if (Fl::visible_focus()) Fl::focus(this);
    Fl::event_is_click(0);
  J1:
    v = menu()->pulldown(x(), y(), w(), h(), mvalue(), this);
    if (!v || v->submenu()) return 1;
    if (v != mvalue()) redraw();
    picked(v);
    return 1;
  case FL_SHORTCUT:
    if (Fl_Widget::test_shortcut()) goto J1;
    v = menu()->test_shortcut();
    if (!v) return 0;
    if (v != mvalue()) redraw();
    picked(v);
    return 1;
  case FL_FOCUS:
  case FL_UNFOCUS:
    if (Fl::visible_focus()) {
      redraw();
      return 1;
    } else return 0;
  default:
    return 0;
  }
}

//
// End of "$Id: Fl_Choice.cxx,v 1.10.2.5.2.14 2003/08/24 13:09:06 easysw Exp $".
//
