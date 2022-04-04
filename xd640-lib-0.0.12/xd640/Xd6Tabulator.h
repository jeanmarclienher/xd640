/*******************************************************************************
 *  "$Id: $"
 *
 *  	Xd6Tabulator definitions for XD640.
 *
 *   		Copyright (c) 2000-2001 O'ksi'D
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
 *   Author : Jean-Marc Lienher <oksid@bluewin.ch>
 *
 ******************************************************************************/

#ifndef Xd6Tabulator_h
#define Xd6Tabulator_h

#include <FL/Fl.H>
#include "Xd6ConfigFile.h"
#include "Xd6HtmlFrame.h"

class Xd6Tabulator {
public:
	Xd6Tabulator() { ;}
	~Xd6Tabulator() { ;}
	static void setup(void);
};

#endif // !Xd6Tabulator_h

/*
 *  End of "$Id:  $".
 */