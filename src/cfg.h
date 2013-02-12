/*
libgliar - a library that can fake the OpenGL context info returned by
the glGet OpenGL calls

Copyright (C) 2013 Canonical Ltd

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#ifndef CFG_H_
#define CFG_H_

enum TYPE {unknown, flt, dbl, boolean, integer, str};

struct cfgopt {
	char *key;
	char *str_val;

  double num_val;
  enum TYPE type;

	struct cfgopt *next;
};

struct cfgopt *gliar_load_cfg(const char *fname);

const char *gliar_find_opt(struct cfgopt *list, const char *name);

void gliar_print_opt(struct cfgopt *list);

#endif	/* CFG_H_ */
