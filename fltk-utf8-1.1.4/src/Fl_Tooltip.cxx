//
// "$Id: Fl_Tooltip.cxx,v 1.38.2.27 2003/01/30 21:42:53 easysw Exp $"
//
// Tooltip source file for the Fast Light Tool Kit (FLTK).
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

#include <FL/Fl_Tooltip.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Menu_Window.H>

#include <stdio.h>

float		Fl_Tooltip::delay_ = 1.0f;
int		Fl_Tooltip::enabled_ = 1;
unsigned	Fl_Tooltip::color_ = fl_color_cube(FL_NUM_RED - 1,
		                                   FL_NUM_GREEN - 1,
						   FL_NUM_BLUE - 2);
unsigned	Fl_Tooltip::textcolor_ = FL_BLACK;
int		Fl_Tooltip::font_ = FL_HELVETICA;
int		Fl_Tooltip::size_ = FL_NORMAL_SIZE;

#define MAX_WIDTH 400

static const char* tip;

class Fl_TooltipBox : public Fl_Menu_Window {
public:
  Fl_TooltipBox() : Fl_Menu_Window(0, 0) {
    set_override();
    end();
  }
  void draw();
  void layout();
  void show() {
    if (tip) Fl_Menu_Window::show();
  }
};

Fl_Widget* Fl_Tooltip::widget_ = 0;
static Fl_TooltipBox *window = 0;
static int X,Y,W,H;

void Fl_TooltipBox::layout() {
  fl_font(Fl_Tooltip::font(), Fl_Tooltip::size());
  int ww, hh;
  ww = MAX_WIDTH;
  fl_measure(tip, ww, hh, FL_ALIGN_LEFT|FL_ALIGN_WRAP|FL_ALIGN_INSIDE);
  ww += 6; hh += 6;

  // find position on the screen of the widget:
  int ox = Fl::event_x_root();
  //int ox = X+W/2;
  int oy = Y + H+2;
  for (Fl_Widget* p = Fl_Tooltip::current(); p; p = p->window()) {
    //ox += p->x();
    oy += p->y();
  }
  if (ox+ww > Fl::w()) ox = Fl::w() - ww;
  if (ox < 0) ox = 0;
  if (H > 30) {
    oy = Fl::event_y_root()+13;
    if (oy+hh > Fl::h()) oy -= 23+hh;
  } else {
    if (oy+hh > Fl::h()) oy -= (4+hh+H);
  }
  if (oy < 0) oy = 0;

  resize(ox, oy, ww, hh);
}

void Fl_TooltipBox::draw() {
  draw_box(FL_BORDER_BOX, 0, 0, w(), h(), Fl_Tooltip::color());
  fl_color(Fl_Tooltip::textcolor());
  fl_font(Fl_Tooltip::font(), Fl_Tooltip::size());
  fl_draw(tip, 3, 3, w()-6, h()-6, Fl_Align(FL_ALIGN_LEFT|FL_ALIGN_WRAP));
}

static char recent_tooltip;

static void recent_timeout(void*) {
  recent_tooltip = 0;
}

static char recursion;

static void tooltip_timeout(void*) {
  if (recursion) return;
  recursion = 1;
  if (!tip || !*tip) {
    if (window) window->hide();
  } else {
    //if (Fl::grab()) return;
    if (!window) window = new Fl_TooltipBox;
    // this cast bypasses the normal Fl_Window label() code:
    ((Fl_Widget*)window)->label(tip);
    window->layout();
    window->redraw();
//    printf("tooltip_timeout: Showing window %p with tooltip \"%s\"...\n",
//           window, tip ? tip : "(null)");
    window->show();
  }

  Fl::remove_timeout(recent_timeout);
  recent_tooltip = 1;
  recursion = 0;
}

// Acts as though enter(widget) was done but does not pop up a
// tooltip.  This is useful to prevent a tooltip from reappearing when
// a modal overlapping window is deleted. Fltk does this automatically
// when you click the mouse button.
void Fl_Tooltip::current(Fl_Widget* w) {
  enter(0);
  // find the enclosing group with a tooltip:
  Fl_Widget* tw = w;
  for (;;) {
    if (!tw) return;
    if (tw->tooltip()) break;
    tw = tw->parent();
  }
  // act just like tt_enter() except we can remember a zero:
  widget_ = w;
}

// This is called when a widget is destroyed:
static void
tt_exit(Fl_Widget *w) {
#ifdef DEBUG
  printf("tt_exit(w=%p)\n", w);
  printf("    widget=%p, window=%p\n", Fl_Tooltip::current(), window);
#endif // DEBUG

  if (!Fl_Tooltip::current()) return;
  Fl_Tooltip::current(0);
  Fl::remove_timeout(tooltip_timeout);
  Fl::remove_timeout(recent_timeout);
  if (window) window->hide();
  if (recent_tooltip) {
    if (Fl::event_state() & FL_BUTTONS) recent_tooltip = 0;
    else Fl::add_timeout(.2f, recent_timeout);
  }
}

static void
tt_enter(Fl_Widget* wp) {
#ifdef DEBUG
  printf("tt_enter(wp=%p)\n", wp);
  printf("    window=%p\n", window);
#endif // DEBUG

  // find the enclosing group with a tooltip:
  Fl_Widget* w = wp;
  while (w && !w->tooltip()) {
    //if (w == window) return; // don't do anything if pointed at tooltip
    w = w->parent();
  }
  if (!w) {
    Fl_Tooltip::enter_area(0, 0, 0, 0, 0, 0);
  } else {
    Fl_Tooltip::enter_area(w,0,0,w->w(), w->h(), w->tooltip());
  }
}

void
Fl_Tooltip::enter_area(Fl_Widget* wid, int x,int y,int w,int h, const char* t)
{
#ifdef DEBUG
  printf("Fl_Tooltip::enter_area(wid=%p, x=%d, y=%d, w=%d, h=%d, t=\"%s\")\n",
         wid, x, y, w, h, t ? t : "(null)");
  printf("    recursion=%d, window=%p\n", recursion, window);
#endif // DEBUG

  if (recursion) return;
  if (!t || !*t || !enabled()) {
    if (window) window->hide();
    Fl::remove_timeout(tooltip_timeout);
    Fl::remove_timeout(recent_timeout);
    return;
  }
  // do nothing if it is the same:
  if (wid==widget_ && x==X && y==Y && w==W && h==H && t==tip) return;
  Fl::remove_timeout(tooltip_timeout);
  Fl::remove_timeout(recent_timeout);
  // remember it:
  widget_ = wid; X = x; Y = y; W = w; H = h; tip = t;
  // popup the tooltip immediately if it was recently up:
  if (recent_tooltip || Fl_Tooltip::delay() < .1) {
#ifdef WIN32
    // possible fix for the Windows titlebar, it seems to want the
    // window to be destroyed, moving it messes up the parenting:
    if (window) window->hide();
#endif
    tooltip_timeout(0);
  } else {
    if (window) window->hide();
    Fl::add_timeout(Fl_Tooltip::delay(), tooltip_timeout);
  }

#ifdef DEBUG
  printf("    tip=\"%s\", window->shown()=%d\n", tip ? tip : "(null)",
         window ? window->shown() : 0);
#endif // DEBUG
}

void Fl_Widget::tooltip(const char *tt) {
  static char beenhere = 0;
  if (!beenhere) {
    beenhere = 1;
    Fl_Tooltip::enter = tt_enter;
    Fl_Tooltip::exit = tt_exit;
  }
  tooltip_ = tt;
}

//
// End of "$Id: Fl_Tooltip.cxx,v 1.38.2.27 2003/01/30 21:42:53 easysw Exp $".
//
