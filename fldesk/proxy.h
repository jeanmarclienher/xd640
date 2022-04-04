/******************************************************************************* *   $Id: proxy.h,v 1.1 2000/08/05 19:11:21 nickasil Exp $
 *
 *   This file is part of the FLE project.
 *
 *                 Copyright (c) 2000  O'ksi'D
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
 *   Author : Jean-Marc Lienher <nickasil@linuxave.net>
 *
 ******************************************************************************/

#ifndef proxy_h
#define proxy_h
#include "FL/Fl_Window.H"

class Proxy : public Fl_Window {
public:
	char is_drop;
	Proxy(void) : Fl_Window(10000,10000,32,32) { is_drop = 0;};
	int handle(int);
	void init(void);
	void drop_func(void);
};

#endif
