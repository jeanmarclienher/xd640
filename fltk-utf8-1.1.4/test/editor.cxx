//
// "$Id: editor.cxx,v 1.2.2.3.2.17 2003/01/30 21:45:24 easysw Exp $"
//
// A simple text editor program for the Fast Light Tool Kit (FLTK).
//
// This program is described in Chapter 4 of the FLTK Programmer's Guide.
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

//
// Include necessary headers...
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef __MWERKS__
# define FL_DLL
#endif

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>



int                changed = 0;
char               filename[256] = "";
char               title[256];
Fl_Text_Buffer     *textbuf = 0;


// Syntax highlighting stuff...
Fl_Text_Buffer     *stylebuf = 0;
Fl_Text_Display::Style_Table_Entry
                   styletable[] = {	// Style table
		     { FL_BLACK,      FL_COURIER,        14 }, // A - Plain
		     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // B - Line comments
		     { FL_DARK_GREEN, FL_COURIER_ITALIC, 14 }, // C - Block comments
		     { FL_BLUE,       FL_COURIER,        14 }, // D - Strings
		     { FL_DARK_RED,   FL_COURIER,        14 }, // E - Directives
		     { FL_DARK_RED,   FL_COURIER_BOLD,   14 }, // F - Types
		     { FL_BLUE,       FL_COURIER_BOLD,   14 }  // G - Keywords
		   };
const char         *code_keywords[] = {	// List of known C/C++ keywords...
		     "and",
		     "and_eq",
		     "asm",
		     "bitand",
		     "bitor",
		     "break",
		     "case",
		     "catch",
		     "compl",
		     "continue",
		     "default",
		     "delete",
		     "do",
		     "else",
		     "false",
		     "for",
		     "goto",
		     "if",
		     "new",
		     "not",
		     "not_eq",
		     "operator",
		     "or",
		     "or_eq",
		     "return",
		     "switch",
		     "template",
		     "this",
		     "throw",
		     "true",
		     "try",
		     "while",
		     "xor",
		     "xor_eq"
		   };
const char         *code_types[] = {	// List of known C/C++ types...
		     "auto",
		     "bool",
		     "char",
		     "class",
		     "const",
		     "const_cast",
		     "double",
		     "dynamic_cast",
		     "enum",
		     "explicit",
		     "extern",
		     "float",
		     "friend",
		     "inline",
		     "int",
		     "long",
		     "mutable",
		     "namespace",
		     "private",
		     "protected",
		     "public",
		     "register",
		     "short",
		     "signed",
		     "sizeof",
		     "static",
		     "static_cast",
		     "struct",
		     "template",
		     "typedef",
		     "typename",
		     "union",
		     "unsigned",
		     "virtual",
		     "void",
		     "volatile"
		   };


//
// 'compare_keywords()' - Compare two keywords...
//

int
compare_keywords(const void *a,
                 const void *b) {
  return (strcmp(*((const char **)a), *((const char **)b)));
}


//
// 'style_parse()' - Parse text and produce style data.
//

void
style_parse(const char *text,
            char       *style,
	    int        length) {
  char	     current;
  int	     col;
  int	     last;
  char	     buf[255],
             *bufptr;
  const char *temp;

  for (current = *style, col = 0, last = 0; length > 0; length --, text ++) {
    if (current == 'B') current = 'A';
    if (current == 'A') {
      // Check for directives, comments, strings, and keywords...
      if (col == 0 && *text == '#') {
        // Set style to directive
        current = 'E';
      } else if (strncmp(text, "//", 2) == 0) {
        current = 'B';
	for (; length > 0 && *text != '\n'; length --, text ++) *style++ = 'B';

        if (length == 0) break;
      } else if (strncmp(text, "/*", 2) == 0) {
        current = 'C';
      } else if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        current = 'D';
      } else if (!last && islower(*text)) {
        // Might be a keyword...
	for (temp = text, bufptr = buf;
	     islower(*temp) && bufptr < (buf + sizeof(buf) - 1);
	     *bufptr++ = *temp++);

        if (!islower(*temp)) {
	  *bufptr = '\0';

          bufptr = buf;

	  if (bsearch(&bufptr, code_types,
	              sizeof(code_types) / sizeof(code_types[0]),
		      sizeof(code_types[0]), compare_keywords)) {
	    while (text < temp) {
	      *style++ = 'F';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  } else if (bsearch(&bufptr, code_keywords,
	                     sizeof(code_keywords) / sizeof(code_keywords[0]),
		             sizeof(code_keywords[0]), compare_keywords)) {
	    while (text < temp) {
	      *style++ = 'G';
	      text ++;
	      length --;
	      col ++;
	    }

	    text --;
	    length ++;
	    last = 1;
	    continue;
	  }
	}
      }
    } else if (current == 'C' && strncmp(text, "*/", 2) == 0) {
      // Close a C comment...
      *style++ = current;
      *style++ = current;
      text ++;
      length --;
      current = 'A';
      col += 2;
      continue;
    } else if (current == 'D') {
      // Continuing in string...
      if (strncmp(text, "\\\"", 2) == 0) {
        // Quoted end quote...
	*style++ = current;
	*style++ = current;
	text ++;
	length --;
	col += 2;
	continue;
      } else if (*text == '\"') {
        // End quote...
	*style++ = current;
	col ++;
	current = 'A';
	continue;
      }
    }

    // Copy style info...
    if (current == 'A' && (*text == '{' || *text == '}')) *style++ = 'G';
    else *style++ = current;
    col ++;

    last = isalnum(*text) || *text == '.';

    if (*text == '\n') {
      // Reset column and possibly reset the style
      col = 0;
      if (current == 'B' || current == 'E') current = 'A';
    }
  }
}


//
// 'style_init()' - Initialize the style buffer...
//

void
style_init(void) {
  char *style = new char[textbuf->length() + 1];
  char *text = textbuf->text();
  

  memset(style, 'A', textbuf->length());
  style[textbuf->length()] = '\0';

  if (!stylebuf) stylebuf = new Fl_Text_Buffer(textbuf->length());

  style_parse(text, style, textbuf->length());

  stylebuf->text(style);
  delete[] style;
  free(text);
}


//
// 'style_unfinished_cb()' - Update unfinished styles.
//

void
style_unfinished_cb(int, void*) {
}


//
// 'style_update()' - Update the style buffer...
//

void
style_update(int        pos,		// I - Position of update
             int        nInserted,	// I - Number of inserted chars
	     int        nDeleted,	// I - Number of deleted chars
             int        /*nRestyled*/,	// I - Number of restyled chars
	     const char * /*deletedText*/,// I - Text that was deleted
             void       *cbArg) {	// I - Callback data
  int	start,				// Start of text
	end;				// End of text
  char	last;				// Last style on line
  char *style,				// Style data
	*text;				// Text data


  // If this is just a selection change, just unselect the style buffer...
  if (nInserted == 0 && nDeleted == 0) {
    stylebuf->unselect();
    return;
  }

  // Track changes in the text buffer...
  if (nInserted > 0) {
    // Insert characters into the style buffer...
    style = (char*)malloc(nInserted + 1);
    memset(style, 'A', nInserted);
    style[nInserted] = '\0';

    stylebuf->replace(pos, pos + nDeleted, style);
    free(style);
  } else {
    // Just delete characters in the style buffer...
    stylebuf->remove(pos, pos + nDeleted);
  }

  // Select the area that was just updated to avoid unnecessary
  // callbacks...
  stylebuf->select(pos, pos + nInserted - nDeleted);

  // Re-parse the changed region; we do this by parsing from the
  // beginning of the line of the changed region to the end of
  // the line of the changed region...  Then we check the last
  // style character and keep updating if we have a multi-line
  // comment character...
  start = textbuf->line_start(pos);
  end   = textbuf->line_end(pos + nInserted);
  text  = textbuf->text_range(start, end);
  style = stylebuf->text_range(start, end);
  last  = style[end - start - 1];

//  printf("start = %d, end = %d, text = \"%s\", style = \"%s\"...\n",
//         start, end, text, style);

  style_parse(text, style, end - start);

//  printf("new style = \"%s\"...\n", style);

  stylebuf->replace(start, end, style);
  ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);

  if (last != style[end - start - 1]) {
    // The last character on the line changed styles, so reparse the
    // remainder of the buffer...
    free(text);
    free(style);

    end   = textbuf->length();
    text  = textbuf->text_range(start, end);
    style = stylebuf->text_range(start, end);

    style_parse(text, style, end - start);

    stylebuf->replace(start, end, style);
    ((Fl_Text_Editor *)cbArg)->redisplay_range(start, end);
  }

  free(text);
  free(style);
}


// Editor window functions and class...
void save_cb();
void saveas_cb();
void find2_cb(Fl_Widget*, void*);
void replall_cb(Fl_Widget*, void*);
void replace2_cb(Fl_Widget*, void*);
void replcan_cb(Fl_Widget*, void*);

class EditorWindow : public Fl_Double_Window {
  public:
    EditorWindow(int w, int h, const char* t);
    ~EditorWindow();

    Fl_Window          *replace_dlg;
    Fl_Input           *replace_find;
    Fl_Input           *replace_with;
    Fl_Button          *replace_all;
    Fl_Return_Button   *replace_next;
    Fl_Button          *replace_cancel;

    Fl_Text_Editor     *editor;
    char               search[256];

    void resize(int X, int Y, int W, int H);
};

void EditorWindow::resize(int X, int Y, int W, int H)
{
	fl_set_status(25,  H - 25, W - 70, 25);
	Fl_Double_Window::resize(X, Y, W, H);	
}

EditorWindow::EditorWindow(int w, int h, const char* t) : Fl_Double_Window(w, h, t) {
  replace_dlg = new Fl_Window(300, 105, "Replace");
    replace_find = new Fl_Input(80, 10, 210, 25, "Find:");
    replace_find->align(FL_ALIGN_LEFT);

    replace_with = new Fl_Input(80, 40, 210, 25, "Replace:");
    replace_with->align(FL_ALIGN_LEFT);

    replace_all = new Fl_Button(10, 70, 90, 25, "Replace All");
    replace_all->callback((Fl_Callback *)replall_cb, this);

    replace_next = new Fl_Return_Button(105, 70, 120, 25, "Replace Next");
    replace_next->callback((Fl_Callback *)replace2_cb, this);

    replace_cancel = new Fl_Button(230, 70, 60, 25, "Cancel");
    replace_cancel->callback((Fl_Callback *)replcan_cb, this);
  replace_dlg->end();
  replace_dlg->set_non_modal();
  editor = 0;
  *search = (char)0;
  fl_set_status(25,  h - 25, w - 70, 25);
}

EditorWindow::~EditorWindow() {
  delete replace_dlg;
}

int check_save(void) {
  if (!changed) return 1;

  int r = fl_choice("The current file has not been saved.\n"
                    "Would you like to save it now?",
                    "Cancel", "Save", "Discard");

  if (r == 1) {
    save_cb(); // Save the file...
    return !changed;
  }

  return (r == 2) ? 1 : 0;
}

int loading = 0;
void load_file(char *newfile, int ipos) {
  loading = 1;
  int insert = (ipos != -1);
  changed = insert;
  if (!insert) strcpy(filename, "");
  int r;
  if (!insert) r = textbuf->loadfile(newfile);
  else r = textbuf->insertfile(newfile, ipos);
  if (r)
    fl_alert("Error reading from file \'%s\':\n%s.", newfile, strerror(errno));
  else
    if (!insert) strcpy(filename, newfile);
  loading = 0;
  textbuf->call_modify_callbacks();
}

void save_file(char *newfile) {
  if (textbuf->savefile(newfile))
    fl_alert("Error writing to file \'%s\':\n%s.", newfile, strerror(errno));
  else
    strcpy(filename, newfile);
  changed = 0;
  textbuf->call_modify_callbacks();
}

void copy_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_copy(0, e->editor);
}

void cut_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_cut(0, e->editor);
}

void delete_cb(Fl_Widget*, void*) {
  textbuf->remove_selection();
}

void find_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *val;

  val = fl_input("Search String:", e->search);
  if (val != NULL) {
    // User entered a string - go find it!
    strcpy(e->search, val);
    find2_cb(w, v);
  }
}

void find2_cb(Fl_Widget* w, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  if (e->search[0] == '\0') {
    // Search string is blank; get a new one...
    find_cb(w, v);
    return;
  }

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, e->search, &pos);
  if (found) {
    // Found a match; select and update the position...
    textbuf->select(pos, pos+strlen(e->search));
    e->editor->insert_position(pos+strlen(e->search));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", e->search);
}

void set_title(Fl_Window* w) {
  if (filename[0] == '\0') strcpy(title, "Untitled");
  else {
    char *slash;
    slash = strrchr(filename, '/');
#ifdef WIN32
    if (slash == NULL) slash = strrchr(filename, '\\');
#endif
    if (slash != NULL) strcpy(title, slash + 1);
    else strcpy(title, filename);
  }

  if (changed) strcat(title, " (modified)");

  w->label(title);
}

void changed_cb(int, int nInserted, int nDeleted,int, const char*, void* v) {
  if ((nInserted || nDeleted) && !loading) changed = 1;
  EditorWindow *w = (EditorWindow *)v;
  set_title(w);
  if (loading) w->editor->show_insert_position();
}

void new_cb(Fl_Widget*, void*) {
  if (!check_save()) return;

  filename[0] = '\0';
  textbuf->select(0, textbuf->length());
  textbuf->remove_selection();
  changed = 0;
  textbuf->call_modify_callbacks();
}

void open_cb(Fl_Widget*, void*) {
  if (!check_save()) return;

  char *newfile = fl_file_chooser("Open File?", "*", filename);
  if (newfile != NULL) load_file(newfile, -1);
}

void insert_cb(Fl_Widget*, void *v) {
  char *newfile = fl_file_chooser("Insert File?", "*", filename);
  EditorWindow *w = (EditorWindow *)v;
  if (newfile != NULL) load_file(newfile, w->editor->insert_position());
}

void paste_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  Fl_Text_Editor::kf_paste(0, e->editor);
}

int num_windows = 0;

void close_cb(Fl_Widget*, void* v) {
  Fl_Window* w = (Fl_Window*)v;
  if (num_windows == 1 && !check_save()) {
    return;
  }

  w->hide();
  textbuf->remove_modify_callback(changed_cb, w);
  delete w;
  num_windows--;
  if (!num_windows) exit(0);
}

void quit_cb(Fl_Widget*, void*) {
  if (changed && !check_save())
    return;

  exit(0);
}

void replace_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  e->replace_dlg->show();
}

void replace2_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  int pos = e->editor->insert_position();
  int found = textbuf->search_forward(pos, find, &pos);

  if (found) {
    // Found a match; update the position and replace text...
    textbuf->select(pos, pos+strlen(find));
    textbuf->remove_selection();
    textbuf->insert(pos, replace);
    textbuf->select(pos, pos+strlen(replace));
    e->editor->insert_position(pos+strlen(replace));
    e->editor->show_insert_position();
  }
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replall_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  const char *find = e->replace_find->value();
  const char *replace = e->replace_with->value();

  find = e->replace_find->value();
  if (find[0] == '\0') {
    // Search string is blank; get a new one...
    e->replace_dlg->show();
    return;
  }

  e->replace_dlg->hide();

  e->editor->insert_position(0);
  int times = 0;

  // Loop through the whole string
  for (int found = 1; found;) {
    int pos = e->editor->insert_position();
    found = textbuf->search_forward(pos, find, &pos);

    if (found) {
      // Found a match; update the position and replace text...
      textbuf->select(pos, pos+strlen(find));
      textbuf->remove_selection();
      textbuf->insert(pos, replace);
      e->editor->insert_position(pos+strlen(replace));
      e->editor->show_insert_position();
      times++;
    }
  }

  if (times) fl_message("Replaced %d occurrences.", times);
  else fl_alert("No occurrences of \'%s\' found!", find);
}

void replcan_cb(Fl_Widget*, void* v) {
  EditorWindow* e = (EditorWindow*)v;
  e->replace_dlg->hide();
}

void save_cb() {
  if (filename[0] == '\0') {
    // No filename - get one!
    saveas_cb();
    return;
  }
  else save_file(filename);
}

void saveas_cb() {
  char *newfile;

  newfile = fl_file_chooser("Save File As?", "*", filename);
  if (newfile != NULL) save_file(newfile);
}

Fl_Window* new_view();

void view_cb(Fl_Widget*, void*) {
  Fl_Window* w = new_view();
  w->show();
}

Fl_Menu_Item menuitems[] = {
  { "&File",              0, 0, 0, FL_SUBMENU },
    { "&New File",        0, (Fl_Callback *)new_cb },
    { "&Open File...",    FL_CTRL + 'o', (Fl_Callback *)open_cb },
    { "&Insert File...",  FL_CTRL + 'i', (Fl_Callback *)insert_cb, 0, FL_MENU_DIVIDER },
    { "&Save File",       FL_CTRL + 's', (Fl_Callback *)save_cb },
    { "Save File &As...", FL_CTRL + FL_SHIFT + 's', (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER },
    { "New &View", FL_ALT + 'v', (Fl_Callback *)view_cb, 0 },
    { "&Close View", FL_CTRL + 'w', (Fl_Callback *)close_cb, 0, FL_MENU_DIVIDER },
    { "E&xit", FL_CTRL + 'q', (Fl_Callback *)quit_cb, 0 },
    { 0 },

  { "&Edit", 0, 0, 0, FL_SUBMENU },
    { "Cu&t",        FL_CTRL + 'x', (Fl_Callback *)cut_cb },
    { "&Copy",       FL_CTRL + 'c', (Fl_Callback *)copy_cb },
    { "&Paste",      FL_CTRL + 'v', (Fl_Callback *)paste_cb },
    { "&Delete",     0, (Fl_Callback *)delete_cb },
    { 0 },

  { "&Search", 0, 0, 0, FL_SUBMENU },
    { "&Find...",       FL_CTRL + 'f', (Fl_Callback *)find_cb },
    { "F&ind Again",    FL_CTRL + 'g', find2_cb },
    { "&Replace...",    FL_CTRL + 'r', replace_cb },
    { "Re&place Again", FL_CTRL + 't', replace2_cb },
    { 0 },

  { 0 }
};

Fl_Window* new_view() {
  EditorWindow* w = new EditorWindow(660, 400, title);
    w->begin();
    Fl_Menu_Bar* m = new Fl_Menu_Bar(0, 0, 660, 30);
    m->copy(menuitems, w);
    w->editor = new Fl_Text_Editor(0, 30, 660, 370);
    w->editor->buffer(textbuf);
    w->editor->highlight_data(stylebuf, styletable,
                              sizeof(styletable) / sizeof(styletable[0]),
			      'A', style_unfinished_cb, 0);
    w->editor->textfont(FL_COURIER);
  w->end();
  w->resizable(w->editor);
  w->callback((Fl_Callback *)close_cb, w);

  textbuf->add_modify_callback(style_update, w->editor);
  textbuf->add_modify_callback(changed_cb, w);
  textbuf->call_modify_callbacks();
  num_windows++;
  return w;
}

int main(int argc, char **argv) {

  textbuf = new Fl_Text_Buffer;
  style_init();

  Fl_Window* window = new_view();

  Fl::set_font(FL_COURIER, 
   "-*-courier-medium-r-normal--*-iso8859-1, *-jisx0208.1983-*");

  window->show(1, argv);

  if (argc > 1) load_file(argv[1], -1);

  return Fl::run();
}

//
// End of "$Id: editor.cxx,v 1.2.2.3.2.17 2003/01/30 21:45:24 easysw Exp $".
//
