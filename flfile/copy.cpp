/******************************************************************************
 *   "$Id:  $"
 *
 *                 Copyright (c) 2000  O'ksi'D
 *
 *                      All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *      Neither the name of O'ksi'D nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *   Author : Jean-Marc Lienher ( http://oksid.ch )
 *
 ******************************************************************************/


#include "copy.h"
#include <libintl.h>
#include <time.h>

#define _(String) gettext((String))

char **Copy::list;
int Copy::nb_item;
int Copy::actual;
int Copy::ask;
Copy *Copy::self;

int Copy::setup(char **lst, int nb, char *path, int mode) 
{
	list = lst;
	nb_item = nb;
	ask = mode;
	actual = 0;
	self = this;
	directory = path;
	dlen = strlen(path);

	begin();
	label(list[0]);

	/*******************************************************************/
	grp = new Fl_Group(5, 5, 390, 95);
	text = new Fl_Box(10, 5, 380, 20, _("Copying :"));
	text->align(FL_ALIGN_INSIDE | FL_ALIGN_LEFT);
	file = new Fl_Output(10, 30, 380, 20, "");
	file->color(FL_GRAY);
	grp->end();
	Fl_Group::current()->resizable(grp);
	/*******************************************************************/

	cancel = new Fl_Button(240, 70, 80, 25, _("Cancel"));
	cancel->callback(cb_cancel, this);
	end();
	return 1;
}

int Copy::set_stat(const char *dest_name, struct stat *st)
{
        int ret = 0;
        struct utimbuf times;

        ret = chown(dest_name, st->st_uid, st->st_gid);
//      if (ret == -1) {
//              fl_alert("chown: %s", strerror(errno));
//              return -1;
//      }

        ret = chmod(dest_name, st->st_mode);
        if (ret == -1) {
                fl_alert("chmod: %s", strerror(errno));
                return -1;
        }

        times.actime = st->st_atime;
        times.modtime = st->st_mtime;
        ret = utime(dest_name, &times);
        if (ret == -1) {
                fl_alert("utime: %s", strerror(errno));
                return -1;
        }

        return 1;
}

int Copy::link_it(const char *src_name, const char *dest_name, struct stat *st)
{
        int ret = 0;
        char buffer[2048];

        ret = readlink(src_name, buffer, 2048);
        if (ret == -1) {
                fl_alert("readlink: %s", strerror(errno));
                return -1;
        }
        buffer[ret] = '\0';

        ret = symlink(buffer, dest_name);
        if (ret == -1) {
                fl_alert("symlink: %s", strerror(errno));
                return -1;
        }

        ret = lchown(dest_name, st->st_uid, st->st_gid);
//      if (ret == -1) {
//              fl_alert("lchown: %s", strerror(errno));
//              return -1;
//      }

        return 1;
}

int Copy::copy_it(const char *src_name, const char *dest_name, struct stat *st)
{
        int ret = 0;
        int dest, src, qty;
        char buffer[2048];

        src = open(src_name, O_RDONLY);
        if (src == -1) {
                fl_alert("open: %s", strerror(errno));
                return -1;
        }

        dest = open(dest_name, O_WRONLY | O_CREAT | O_EXCL, st->st_mode);
        if (dest == -1 && EEXIST == errno && ask) {
		ret = fl_ask(_("file \"%s\" already exists,\n"
			 	"do you want to overwrite it ?"), dest_name);
		Fl::check();
		Fl::redraw();
		if (ret != 1) {
			close(src);
			return -1;
		}
		dest = open(dest_name, O_WRONLY | O_CREAT, st->st_mode);
	}
        if (dest == -1) {
               	fl_alert("open: %s", strerror(errno));
               	close(src);
               	return -1;
        }
        while ((qty = read(src, buffer, 2048))) {
                char *ptr = buffer;
                while (qty > 0) {
                        ret = write(dest, ptr, qty);
                        if (ret == -1) {
                                close(src);
                                close(dest);
                                fl_alert("write: %s", strerror(errno));
                                return -1;
                        }
                        qty -= ret;
                        ptr += ret;
                }

        }
        close(src);
        close(dest);
        if (qty == -1) {
                fl_alert("read: %s", strerror(errno));
                return -1;
        }
        return set_stat(dest_name, st);
}

int Copy::fn(const  char  *src, const  struct  stat  *sb,
	int  flag,  struct FTW *s)
{
	int ret;
	struct stat st;
	const char *dest_file;

	self->file->value(src);
	Fl::flush();
	Fl::check();
	
	dest_file = src + strlen(src);

	while (dest_file > src) {
		if (*dest_file == '/') {
			break;
		}
		dest_file--;
	}
	if (flag != FTW_NS && dest_file > src) { 
		dest_file++;
                ret = lstat(src, &st);
                if (ret == -1) {
                        fl_alert("lstat: %s", strerror(errno));
                        return -1;
                }
		if (st.st_ctime >= self->begin_time) {
			// we are trying to copy infinitly a directory inside
			// itself.
			return 0;
		}
                if (S_ISDIR(st.st_mode)) {
                        ret = mkdir(dest_file, 0777777 ^ umask(0));
                        if (ret == -1) {
                                fl_alert("%s:\nmkdir: %s", dest_file,
                                        strerror(errno));
                                return -1;
                        }
                        if (self->set_stat(dest_file, &st) == -1) {
                                return -1;
                        }
			ret = chdir(dest_file);
			if (ret == -1) {
				fl_alert("%s:\nchdir: %s", dest_file,
					strerror(errno));
				return -1;
			}
			return 0;
               } else if (S_ISLNK(st.st_mode)) {
                        if (self->link_it(src, dest_file, &st) == -1) {
                                return -1;
                        }
                        return 0;
                } else if (S_ISREG(st.st_mode)) {
                        if (self->copy_it(src, dest_file, &st) == -1) {
                                return -1;
                        }
                        return 0;
                } else {
			fl_alert(_("Cannot copy non-regular file \n"
				"\"%s\"."), src);
                         return -1;
                }
	}
	return 0;
}

int Copy::exec(const Fl_Window* modal_for)
{
	struct stat st;
	int ret;

	show();
	Fl::flush();
	Fl::check();


	if (time(&begin_time) == (time_t) -1) {
		fl_alert("time: %s", strerror(errno));
		return -1;
	}
	
	usleep(500000);

	if (stat(directory, &st)) {
			fl_alert("%s:\nstat: %s", directory,
				strerror(errno));
			return -1;
	}

	while (nb_item > 0) {
		nb_item--;
		
		label(list[nb_item]);
		Fl::flush();
		Fl::check();
		ret = chdir(directory);
		if (ret == -1) {
			fl_alert("%s:\nchdir: %s", directory,
				strerror(errno));
			return -1;
		}
		if (lstat(list[nb_item], &st)) continue;
		if (S_ISDIR(st.st_mode)) {
			nftw(list[nb_item], fn, 20, FTW_MOUNT | FTW_PHYS);
		} else if (S_ISREG(st.st_mode)) {
			fn(list[nb_item], &st, FTW_F, NULL);
		} else if (S_ISLNK(st.st_mode)) {
			fn(list[nb_item], &st, FTW_SL, NULL);
		} else {
			fl_alert(_("Cannot copy non-regular file \n"
				"\"%s\"."), list[nb_item]);
		}
	}
	return 0;
}

void Copy::cb_cancel(Fl_Widget *, void *data)
{
	Copy *self = (Copy *)data;
	self->hide();
	Fl::flush();
	Fl::check();
	exit(0);
}

