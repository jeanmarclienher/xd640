//
// "$Id: fonts.cxx,v 1.5.2.3.2.3 2003/01/30 21:45:29 easysw Exp $"
//
// Font demo program for the Fast Light Tool Kit (FLTK).
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
#include <FL/Fl_Window.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Fl_Window *form;

class FontDisplay : public Fl_Widget {
  void draw();
public:
  int font, size;
  FontDisplay(Fl_Boxtype B, int X, int Y, int W, int H, const char* L = 0) :
    Fl_Widget(X,Y,W,H,L) {box(B); font = 0; size = 14;}
};
void FontDisplay::draw() {
  draw_box();
  fl_font((Fl_Font)font, size);
  fl_color(FL_BLACK);
  fl_draw(label(), x()+3, y()+3, w()-6, h()-6, align());
}

FontDisplay *textobj;

Fl_Hold_Browser *fontobj, *sizeobj;

int *sizes[1000];
int numsizes[1000];
int pickedsize = 14;

void font_cb(Fl_Widget *, long) {
  int fn = fontobj->value();
  if (!fn) return;
  fn--;
  textobj->font = fn;
  sizeobj->clear();
  int n = numsizes[fn];
  int *s = sizes[fn];
  if (!n) {
    // no sizes
  } else if (s[0] == 0) {
    // many sizes;
    int j = 1;
    for (int i = 1; i<64 || i<s[n-1]; i++) {
      char buf[20];
      if (j < n && i==s[j]) {sprintf(buf,"@b%d",i); j++;}
      else sprintf(buf,"%d",i);
      sizeobj->add(buf);
    }
    sizeobj->value(pickedsize);
  } else {
    // some sizes
    int w = 0;
    for (int i = 0; i < n; i++) {
      if (s[i]<=pickedsize) w = i;
      char buf[20];
      sprintf(buf,"@b%d",s[i]);
      sizeobj->add(buf);
    }
    sizeobj->value(w+1);
  }
  textobj->redraw();
}

void size_cb(Fl_Widget *, long) {
  int i = sizeobj->value();
  if (!i) return;
  const char *c = sizeobj->text(i);
  while (*c < '0' || *c > '9') c++;
  pickedsize = atoi(c);
  textobj->size = pickedsize;
  textobj->redraw();
}

char label[0x1000];

void create_the_forms() {
  int n = 0;
  form = new Fl_Window(550,370);

  strcpy(label, "Hello, world!\n");
  int i = strlen(label);
  ulong c;
  for (c = ' '+1; c < 127; c++) {if (!(c&0x1f)) label[i++]='\n'; label[i++]=c;}
  label[i++] = '\n';
  for (c = 0xA1; c < 0x600; c += 9) {if (!(++n&(0x1f))) label[i++]='\n'; 
                          i += fl_ucs2utf((unsigned int)c, label + i);}
  label[i] = 0;

  textobj = new FontDisplay(FL_FRAME_BOX,10,10,530,170,label);
  textobj->align(FL_ALIGN_TOP|FL_ALIGN_LEFT|FL_ALIGN_INSIDE|FL_ALIGN_CLIP);
  textobj->color(9,47);
  fontobj = new Fl_Hold_Browser(10, 190, 390, 170);
  fontobj->box(FL_FRAME_BOX);
  fontobj->color(53,3);
  fontobj->callback(font_cb);
  form->resizable(fontobj);
  sizeobj = new Fl_Hold_Browser(410, 190, 130, 170);
  sizeobj->box(FL_FRAME_BOX);
  sizeobj->color(53,3);
  sizeobj->callback(size_cb);
  form->end();
}

#include <FL/fl_ask.H>

int main(int argc, char **argv) {
  Fl::scheme(NULL);
  create_the_forms();
  int i = fl_choice("Which fonts:","-*","iso8859","All");
  int k = (int)Fl::set_fonts(i ? (i>1 ? "*" : 0) : "-*");
  for (i = 0; i < k; i++) {
    int t; const char *name = Fl::get_font_name((Fl_Font)i,&t);
    char buffer[128];
#if 1
    if (t) {
      char *p = buffer;
      if (t & FL_BOLD) {*p++ = '@'; *p++ = 'b';}
      if (t & FL_ITALIC) {*p++ = '@'; *p++ = 'i';}
      strcpy(p,name);
      name = buffer;
    }
#else // this is neat, but really slow on some X servers:
    sprintf(buffer, "@F%d@.%s", i, name);
    name = buffer;
#endif
    free(malloc(5));
    fontobj->add(name);
    int *s; int n = Fl::get_font_sizes((Fl_Font)i, s);
    numsizes[i] = n;
    if (n) {
      sizes[i] = new int[n];
      for (int j=0; j<n; j++) sizes[i][j] = s[j];
    }
  }
  fontobj->value(1);
  font_cb(fontobj,0);
  form->show(argc,argv);
  return Fl::run();
}

//
// End of "$Id: fonts.cxx,v 1.5.2.3.2.3 2003/01/30 21:45:29 easysw Exp $".
//
