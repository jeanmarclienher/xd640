//
// "$Id: fl_set_fonts_win32.cxx,v 1.5.2.5.2.10 2003/03/09 00:22:18 spitzak Exp $"
//
// WIN32 font utilities for the Fast Light Tool Kit (FLTK).
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
// License along with this7 library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA.
//
// Please report all bugs and problems to "fltk-bugs@fltk.org".
//

// This function fills in the FLTK font table with all the fonts that
// are found on the X server.  It tries to place the fonts into families
// and to sort them so the first 4 in a family are normal, bold, italic,
// and bold italic.
#include <Fl/fl_utf8.H>
extern char *fl_utf82locale(const char *s, UINT codepage = 0);
extern char *fl_locale2utf8(const char *s, UINT codepage = 0);

// Bug: older versions calculated the value for *ap as a side effect of
// making the name, and then forgot about it. To avoid having to change
// the header files I decided to store this value in the last character
// of the font name array.
#define ENDOFBUFFER 127 // sizeof(Fl_Font.fontname)-1

// turn a stored font name into a pretty name:
const char* Fl::get_font_name(Fl_Font fnum, int* ap) {
  Fl_Fontdesc *f = fl_fonts + fnum;
  if (!f->fontname[0]) {
    const char* p = f->name;
    if (!p || !*p) {if (ap) *ap = 0; return "";}
    int type;
    switch (*p) {
    case 'B': type = FL_BOLD; break;
    case 'I': type = FL_ITALIC; break;
    case 'P': type = FL_BOLD | FL_ITALIC; break;
    default:  type = 0; break;
    }
    strlcpy(f->fontname, p+1, ENDOFBUFFER);
    if (type & FL_BOLD) strlcat(f->fontname, " bold", ENDOFBUFFER);
    if (type & FL_ITALIC) strlcat(f->fontname, " italic", ENDOFBUFFER);
    f->fontname[ENDOFBUFFER] = (char)type;
  }
  if (ap) *ap = f->fontname[ENDOFBUFFER];
  return f->fontname;

}

static int fl_free_font = FL_FREE_FONT;

static int CALLBACK 
  enumcbw(CONST LOGFONTW    *lpelf,
       CONST TEXTMETRICW * /*lpntm*/,
       DWORD            /*FontType*/,
         LPARAM           p) {
  if (!p && lpelf->lfCharSet != ANSI_CHARSET) return 1;
  char *n;
  int l = wcslen(lpelf->lfFaceName);
  n = (char*) malloc(l * 6 + 1);
  n[fl_unicode2utf((xchar*)lpelf->lfFaceName, l, n)] = 0;
  for (int i=0; i<FL_FREE_FONT; i++) // skip if one of our built-in fonts
	  if (!strcmp(Fl::get_font_name((Fl_Font)i),n)) {free(n);return 1;}
  char buffer[LF_FACESIZE + 1];
  strcpy(buffer+1, n);
  buffer[0] = ' '; Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  if (lpelf->lfWeight <= 400)
    buffer[0] = 'B', Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  buffer[0] = 'I'; Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  if (lpelf->lfWeight <= 400)
    buffer[0] = 'P', Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  free(n);
  return 1;
}

static int CALLBACK 
  enumcb(CONST LOGFONT    *lpelf,
       CONST TEXTMETRIC * /*lpntm*/,
       DWORD            /*FontType*/,
         LPARAM           p) {
  if (!p && lpelf->lfCharSet != ANSI_CHARSET) return 1;
  const char *n = lpelf->lfFaceName;
  for (int i=0; i<FL_FREE_FONT; i++) // skip if one of our built-in fonts
    if (!strcmp(Fl::get_font_name((Fl_Font)i),n)) return 1;
  char buffer[LF_FACESIZE + 1];
  strcpy(buffer+1, fl_locale2utf8(n));
  buffer[0] = ' '; Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  if (lpelf->lfWeight <= 400)
    buffer[0] = 'B', Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  buffer[0] = 'I'; Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  if (lpelf->lfWeight <= 400)
    buffer[0] = 'P', Fl::set_font((Fl_Font)(fl_free_font++), strdup(buffer));
  return 1;

}

Fl_Font Fl::set_fonts(const char* xstarname) {
  if (fl_free_font == FL_FREE_FONT) {// if not already been called
    if (!fl_gc) fl_GetDC(0);
	if (fl_is_nt4()) {
      EnumFontFamiliesW(fl_gc, NULL, (FONTENUMPROCW)enumcbw, xstarname != 0);
	} else {
      EnumFontFamiliesA(fl_gc, NULL, (FONTENUMPROCA)enumcb, xstarname != 0);
	}
  }
  return (Fl_Font)fl_free_font;
}


static int nbSize;
static int cyPerInch;
static int sizes[128];
static int CALLBACK

EnumSizeCbW(CONST LOGFONTW    * /*lpelf*/,
           CONST TEXTMETRICW *lpntm,
           DWORD            fontType,
           LPARAM           /*p*/) {
  if ((fontType & RASTER_FONTTYPE) == 0) {
    sizes[0] = 0;
    nbSize = 1;

    // Scalable font
    return 0;
  }

  int add = lpntm->tmHeight - lpntm->tmInternalLeading;
  add = MulDiv(add, 72, cyPerInch);

  int start = 0;
  while ((start < nbSize) && (sizes[start] < add)) {
    start++;
  }

  if ((start < nbSize) && (sizes[start] == add)) {
    return 1;
  }

  for (int i=nbSize; i>start; i--) sizes[i] = sizes[i - 1];

  sizes[start] = add;
  nbSize++;

  // Stop enum if buffer overflow
  return nbSize < 128;
}

static int CALLBACK
EnumSizeCb(CONST LOGFONT    * /*lpelf*/,
           CONST TEXTMETRIC *lpntm,
           DWORD            fontType,
           LPARAM           /*p*/) {
  if ((fontType & RASTER_FONTTYPE) == 0) {
    sizes[0] = 0;
    nbSize = 1;

    // Scalable font
    return 0;
  }

  int add = lpntm->tmHeight - lpntm->tmInternalLeading;
  add = MulDiv(add, 72, cyPerInch);

  int start = 0;
  while ((start < nbSize) && (sizes[start] < add)) {
    start++;
  }

  if ((start < nbSize) && (sizes[start] == add)) {
    return 1;
  }

  for (int i=nbSize; i>start; i--) sizes[i] = sizes[i - 1];

  sizes[start] = add;
  nbSize++;

  // Stop enum if buffer overflow
  return nbSize < 128;
}


int
Fl::get_font_sizes(Fl_Font fnum, int*& sizep) {
  nbSize = 0;
  Fl_Fontdesc *s = fl_fonts+fnum;
  if (!s->name) s = fl_fonts; // empty slot in table, use entry 0

  if (!fl_gc) fl_GetDC(0);
  cyPerInch = GetDeviceCaps(fl_gc, LOGPIXELSY);
  if (cyPerInch < 1) cyPerInch = 1;
  if (fl_is_nt4()) {
    int l = fl_utf_nb_char((unsigned char*)s->name+1, strlen(s->name+1));
    unsigned short *b = (unsigned short*) malloc((l + 1) * sizeof(short));
    fl_utf2unicode((unsigned char*)s->name+1, l, (xchar*)b);
    b[l] = 0;
    EnumFontFamiliesW(fl_gc, (WCHAR*)b, (FONTENUMPROCW)EnumSizeCbW, 0);
	free(b);  
  } else {
    EnumFontFamiliesA(fl_gc, fl_utf82locale(s->name+1), (FONTENUMPROCA)EnumSizeCb, 0);
  }
  sizep = sizes;
  return nbSize;
}


//
// End of "$Id: fl_set_fonts_win32.cxx,v 1.5.2.5.2.10 2003/03/09 00:22:18 spitzak Exp $".
//
