/******************************************************************************
 *   "$Id: $"
 *
 *
 *                 Copyright (c) 2000,2001  O'ksi'D
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 *   Author : Jean-Marc Lienher ( http://oksid.ch )
 *
 ******************************************************************************/


#include "Xd6Std.h"
#include "Xd6FindDialog.h"
#include <FL/Fl.h>
#include <FL/Fl_Button.h>
#include <FL/fl_utf8.h>

#define _(String) dgettext(PACKAGE, (String))


Xd6FindDialog::Xd6FindDialog(Xd6HtmlFrame *f) : Fl_Window(240, 240)
{
	frame = f;
	current_block = NULL;
	found = NULL;
	nb_found = 0;
	status = 0;
	pointer = 0;	
	is_rescan = 0;
	
	label(_("Find..."));

	i_find = new Fl_Input(5, 20, 230, 20, _("Find:"));
	i_find->align(FL_ALIGN_TOP);
	i_replace = new Fl_Input(5, 60, 230, 20, _("Replace by:"));
	i_replace->align(FL_ALIGN_TOP);

	b_replace_next = new Fl_Button(5, 155, 230, 25, 
		_("Replace and Find next"));
	b_next =  new Fl_Button(5, 100, 230, 25, _("Find next"));
	b_cancel = new Fl_Button(60, 215, 110, 25, _("Cancel"));

	b_replace_next->callback(cb_rn);
	b_next->callback(cb_n);
	b_cancel->callback(cb_c);
	
	set_modal();
	show();
}	


Xd6FindDialog::~Xd6FindDialog()
{
	free(found);
}


void Xd6FindDialog::cb_rn(Fl_Widget *w, void *d)
{
	Xd6FindDialog *self;
	self = (Xd6FindDialog*)w->parent();
	self->status = 1;
}

void Xd6FindDialog::cb_n(Fl_Widget *w, void *d)
{
	Xd6FindDialog *self;
	self = (Xd6FindDialog*)w->parent();
	self->hide();
	self->status = 2;
}

void Xd6FindDialog::cb_c(Fl_Widget *w, void *d)
{
	Xd6FindDialog *self;
	self = (Xd6FindDialog*)w->parent();
	self->hide();
	self->status = -1;
}

void Xd6FindDialog::scan(Xd6HtmlLine *l, Xd6HtmlSegment *s, int off)
{
	int i;
	const char *str;
	int ln;
	unsigned int point, point_l;

	str = i_find->value();
	ln = strlen(str);

	if (ln < 1) return;
	
	point_l = fl_utf2ucs((unsigned char*)str + pointer, 
		ln - pointer, &point);
	if (point_l < 1) point_l = 1;
	point = fl_tolower(point);

	for (i = off; i < s->len;) {
		unsigned int st;
		int ul;
		ul = fl_utf2ucs((unsigned char*)s->text + i, s->len - i, &st);
		if (ul < 1) ul = 1;
		if ((unsigned)fl_tolower(st) == point) {
			if (pointer == 0) {
				frame->cur_block = current_block;
				frame->cur_line = l;
				frame->cur_seg = s;
				frame->cur_chr = s->text + i;
				add_found(l, s, s->text + i);
			} else if (i == 0) {
				add_found(l, s, s->text);
			}		
			pointer += point_l;
			point_l = fl_utf2ucs((unsigned char*)str + pointer, 
				ln - pointer, &point);
			if (point_l < 1) point_l = 1;
			point = fl_tolower(point);
			if (pointer >= ln) {
				frame->sel_block = current_block;
				frame->sel_line = l;
				frame->sel_seg = s;
				frame->sel_chr = s->text + i + ul;
				frame->auto_scroll(-1, -1);
				frame->auto_scroll(1, 1);
				show();
				Fl::redraw();
				Fl::flush();
				while (status == 0) Fl::wait();
				if (status == 1 && cb_replace) {
					cb_replace(i_replace->value(), frame);
				}
				if (status != -1) status = 0;
				pointer = 0;
				if (!is_rescan) {
					nb_found = 0;
				}
				point_l = fl_utf2ucs( (unsigned char*)str, 
					ln, &point);
				if (point_l < 1) point_l = 1;
				point = fl_tolower(point);
			}
		} else {
			if (pointer > 0) {
				pointer = 0;
				rescan();
				if (!is_rescan) nb_found = 0;
				return;
			}
		}
		i += ul;
	}
}

void Xd6FindDialog::add_found(Xd6HtmlLine *l, Xd6HtmlSegment *s, char *c)
{
	if (is_rescan) {
		int i;
		for (i = 0; i < nb_found; i++) {
			if (found[i].s == s) {
				found[i].c = c - s->text;
				break;
			}
		}
		return;
	}

	found = (Xd6FindDialogFound*) realloc(found, 
		sizeof(Xd6FindDialogFound) * (nb_found + 1));
	found[nb_found].l = l;
	found[nb_found].s = s;
	found[nb_found].c = c - s->text;
	nb_found++;
}

int Xd6FindDialog::finder(Xd6HtmlBlock *b, Xd6HtmlLine *l, Xd6HtmlSegment *s)
{
	if (b != current_block) {
		pointer = 0;
		nb_found = 0;
	}
	current_block = b;

	is_rescan = 0;
	scan(l, s);
	return 0;
}

void Xd6FindDialog::rescan(void)
{
	int off = 0;
	int c;

	is_rescan++;

	while (off < nb_found && found[off].s == NULL) off++;
	if (off >= nb_found) {
		nb_found = 0;
		is_rescan--;
		return;
	}
	frame->cur_seg = NULL;
	found[off].c += 1;
	c = found[off].c;
	if (c < found[off].s->len) {
		scan(found[off].l, found[off].s, c);
	}
	if (!frame->cur_seg) found[off].s = NULL;
	off++;
	while (off < nb_found) {
		if (found[off].s) scan(found[off].l, found[off].s, 0);
		if (!frame->cur_seg) found[off].s = NULL;
		off++;
	}

	if (!frame->cur_seg) {
		nb_found = 0;
	}
	is_rescan--;
}

/*
 * "$Id: $"
 */

