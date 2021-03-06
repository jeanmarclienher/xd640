//
// "$Id: fl_font_x.cxx,v 1.10.2.4 2003/01/30 21:43:52 easysw Exp $"
//
// Standard X11 font selection code for the Fast Light Tool Kit (FLTK).
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

Fl_FontSize::Fl_FontSize(const char* name) {
  font = XCreateUtf8FontStruct(fl_display, name);
  if (!font) {
//    Fl::warning("bad font: %s", name);
    font = XCreateUtf8FontStruct(fl_display, "fixed");
  }
#  if HAVE_GL
  listbase = 0;
  for (int u = 0; u < 64; u++) glok[u] = 0;
#  endif
}

Fl_FontSize* fl_fontsize;

Fl_FontSize::~Fl_FontSize() {
#  if HAVE_GL
// Delete list created by gl_draw().  This is not done by this code
// as it will link in GL unnecessarily.  There should be some kind
// of "free" routine pointer, or a subclass?
// if (listbase) {
//  int base = font->min_char_or_byte2;
//  int size = font->max_char_or_byte2-base+1;
//  int base = 0; int size = 256;
//  glDeleteLists(listbase+base,size);
// }
#  endif
  if (this == fl_fontsize) fl_fontsize = 0;
  XFreeUtf8FontStruct(fl_display, font);
}

////////////////////////////////////////////////////////////////

// WARNING: if you add to this table, you must redefine FL_FREE_FONT
// in Enumerations.H & recompile!!
static Fl_Fontdesc built_in_table[] = {
{"-*-helvetica-medium-r-normal--*"},
{"-*-helvetica-bold-r-normal--*"},
{"-*-helvetica-medium-o-normal--*"},
{"-*-helvetica-bold-o-normal--*"},
{"-*-courier-medium-r-normal--*"},
{"-*-courier-bold-r-normal--*"},
{"-*-courier-medium-o-normal--*"},
{"-*-courier-bold-o-normal--*"},
{"-*-times-medium-r-normal--*"},
{"-*-times-bold-r-normal--*"},
{"-*-times-medium-i-normal--*"},
{"-*-times-bold-i-normal--*"},
{"-*-symbol-*"},
{"-*-lucidatypewriter-medium-r-normal-sans-*"},
{"-*-lucidatypewriter-bold-r-normal-sans-*"},
{"-*-*zapf dingbats-*"}
};

Fl_Fontdesc* fl_fonts = built_in_table;

#define MAXSIZE 32767

// return dash number N, or pointer to ending null if none:
const char* fl_font_word(const char* p, int n) {
  while (*p) {if (*p=='-') {if (!--n) break;} p++;}
  return p;
}

// return a pointer to a number we think is "point size":
char* fl_find_fontsize(char* name) {
  char* c = name;
  // for standard x font names, try after 7th dash:
  if (*c == '-') {
    c = (char*)fl_font_word(c,7);
    if (*c++ && isdigit(*c)) return c;
    return 0; // malformed x font name?
  }
  char* r = 0;
  // find last set of digits:
  for (c++;* c; c++)
    if (isdigit(*c) && !isdigit(*(c-1))) r = c;
  return r;
}

const char* fl_encoding = "iso8859-1";

// return true if this matches fl_encoding:
int fl_correct_encoding(const char* name) {
  if (*name != '-') return 0;
  const char* c = fl_font_word(name,13);
  return (*c++ && !strcmp(c,fl_encoding));
}

static char *find_best_font(const char *fname, int size) {
  int cnt;
  static char **list = NULL;

  if (list) XFreeFontNames(list);
  list = XListFonts(fl_display, fname, 100, &cnt);
  if (!list) return "fixed";

 // search for largest <= font size:
  char* name = list[0]; int ptsize = 0;     // best one found so far
  int matchedlength = 32767;
  static char namebuffer[1024];        // holds scalable font name
  int found_encoding = 0;
  int m = cnt; if (m<0) m = -m;
  for (int n=0; n < m; n++) {
    char* thisname = list[n];
    if (fl_correct_encoding(thisname)) {
      if (!found_encoding) ptsize = 0; // force it to choose this
      found_encoding = 1;
    } else {
      if (found_encoding) continue;
    }
    char* c = (char*)fl_find_fontsize(thisname);
    int thissize = c ? atoi(c) : MAXSIZE;
    int thislength = strlen(thisname);
    if (thissize == size && thislength < matchedlength) {
      // exact match, use it:
      name = thisname;
      ptsize = size;
      matchedlength = thislength;
    } else if (!thissize && ptsize!=size) {
      // whoa!  A scalable font!  Use unless exact match found:
      int l = c-thisname;
      memcpy(namebuffer,thisname,l);
      l += sprintf(namebuffer+l,"%d",size);
      while (*c == '0') c++;
      strcpy(namebuffer+l,c);
      name = namebuffer;
      ptsize = size;
    } else if (!ptsize ||       // no fonts yet
               thissize < ptsize && ptsize > size || // current font too big
               thissize > ptsize && thissize <= size // current too small
      ) {
    //  int l = c-thisname;
     // memcpy(namebuffer,thisname,l);
      //l += sprintf(namebuffer+l,"%d-*%s",size, fl_font_word(thisname, 13));
      name = thisname;
      ptsize = thissize;
      matchedlength = thislength;
    }
  }
  return name;
};

static char *put_font_size(const char *n, int size)
{
        int i = 0;
        char *buf;
        const char *ptr;
        char *f;
        char *name;
        int nbf = 1;
        name = strdup(n);
        while (name[i]) {
                if (name[i] == ',') {nbf++; name[i] = '\0';}
                i++;
        }

        buf = (char*) malloc(nbf * 256);
        buf[0] = '\0';
        ptr = name;
        i = 0;
        while (ptr && nbf > 0) {
                f = find_best_font(ptr, size);
                while (*f) {
                        buf[i] = *f;
                        f++; i++;
                }
                nbf--;
                while (*ptr) ptr++;
                if (nbf) {
                        ptr++;
                        buf[i] = ',';
                        i++;
                }
                while(isspace(*ptr)) ptr++;
        }
        buf[i] = '\0';
        free(name);
        return buf;
}


char *fl_get_font_xfld(int fnum, int size) {
  Fl_Fontdesc* s = fl_fonts+fnum;
  if (!s->name) s = fl_fonts; // use font 0 if still undefined
  fl_open_display();
  return put_font_size(s->name, size);
}

// locate or create an Fl_FontSize for a given Fl_Fontdesc and size:
static Fl_FontSize* find(int fnum, int size) {
  char *name;
  Fl_Fontdesc* s = fl_fonts+fnum;
  if (!s->name) s = fl_fonts; // use font 0 if still undefined
  Fl_FontSize* f;
  for (f = s->first; f; f = f->next)
    if (f->minsize <= size && f->maxsize >= size) return f;
  fl_open_display();

  name = put_font_size(s->name, size);
  f = new Fl_FontSize(name);
  f->minsize = size;
  f->maxsize = size;
  f->next = s->first;
  s->first = f;
  free(name);
  return f;
}


////////////////////////////////////////////////////////////////
// Public interface:

int fl_font_ = 0;
int fl_size_ = 0;
XUtf8FontStruct* fl_xfont;
static GC font_gc;

void Fl_Fltk::font(int fnum, int size) {
  if (fnum == fl_font_ && size == fl_size_) return;
  fl_font_ = fnum; fl_size_ = size;
  Fl_FontSize* f = find(fnum, size);
  if (f != fl_fontsize) {
    fl_fontsize = f;
    fl_xfont = f->font;
    font_gc = 0;
  }
}

int Fl_Fltk::height() {
  return (fl_xfont->ascent + fl_xfont->descent);
}

int Fl_Fltk::descent() {
  return fl_xfont->descent;
}

double Fl_Fltk::width(const char* c, int n) {
  return (double) XUtf8TextWidth(fl_xfont, c, n);
}

double Fl_Fltk::width(unsigned int c) {
  return (double) XUtf8UcsWidth(fl_xfont, c);
}

void Fl_Fltk::draw(const char* c, int n, int x, int y) {
  if (font_gc != fl_gc) {
	  if (!fl_xfont) Fl_Fltk::font(FL_HELVETICA, 12);
    font_gc = fl_gc;
  }
  XUtf8DrawString(fl_display, fl_window, fl_xfont, fl_gc, x, y, c, n);
}

void Fl_Fltk::rtl_draw(const char* c, int n, int x, int y) {
  if (font_gc != fl_gc) {
	  if (!fl_xfont) Fl_Fltk::font(FL_HELVETICA, 12);
    font_gc = fl_gc;
  }
  XUtf8DrawRtlString(fl_display, fl_window, fl_xfont, fl_gc, x, y, c, n);
}

//
// End of "$Id: fl_font_x.cxx,v 1.10.2.4 2003/01/30 21:43:52 easysw Exp $".
//
