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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.	If not, see <http://www.gnu.org/licenses/>.

Author: Eleni Maria Stea <elene.mst@gmail.com>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cfg.h"

static char *stripspace(char *s);
static void concat_values(struct cfgopt *opt);

struct cfgopt *gliar_load_cfg(const char *fname)
{
	FILE *fp;
	char buf[512];
	struct cfgopt *optlist = 0;
	struct cfgopt *opt = 0;

	if(!(fp = fopen(fname, "r"))) {
		return 0;
	}

	while(fgets(buf, sizeof buf, fp)) {
		char *line = stripspace(buf);

		if(!*line || *line == '#') {
			continue;
		}

		if(*line == '[') {
			char *end = strrchr(line, ']');
			if(!end) {
				fprintf(stderr, "invalid config %s: %s\n", fname, line);
				continue;
			}
			line++;
			*end = 0;

			if(opt) {
				opt->next = optlist;
				optlist = opt;
				concat_values(opt);
			}

			if((opt = malloc(sizeof *opt))) {
				if((opt->key = malloc(strlen(line) + 1))) {
					strcpy(opt->key, line);
					opt->str_val = 0;
					opt->num_val = 0;
					opt->str_count = 0;
					opt->type = GLIAR_STRING;
				} else {
					free(opt);
					opt = 0;
				}
			}
		} else {
			int new_sz = opt->str_count + 1;
			char **tmp;

			char *end;
			int num = strtol(line, &end, 10);

			if(!*end) {
				opt->num_val = line;
				opt->type = GLIAR_NUMBER;
			}

			if(opt && (tmp = realloc(opt->str_val, new_sz * sizeof(char*)))) {
				opt->str_val = tmp;
				if((opt->str_val[new_sz - 1] = malloc(strlen(line) + 1))) {
					strcpy(opt->str_val[new_sz -1], line);
					opt->str_count = new_sz;
				}
			}

			if(new_sz > 1) {
				opt->type = GLIAR_STRING;
			}
		}
	}

	if(opt) {
		opt->next = optlist;
		optlist = opt;
		concat_values(opt);
	}

	fclose(fp);
	return optlist;
}

const struct cfgopt *gliar_find_opt(struct cfgopt *list, const char *name)
{
	if(!list || !name) {
		return 0;
	}

	while(list) {
		if(strcmp(list->key, name) == 0) {
			return list;
		}
		list = list->next;
	}
	return 0;
}

void gliar_print_opt(struct cfgopt *list)
{
	printf("OPTIONS\n");
	while(list) {
		if(list->type == GLIAR_NUMBER) {
			printf("\"%s\" -> %d\n", list->key, list->num_val);
		}
		else {
			int i;
			for(i=0; i<list->str_count; i++) {
				printf("\"%s\" -> \"%s\"\n", list->key, list->str_val[i]);
			}
		}
		list = list->next;
	}
}

static char *stripspace(char *s)
{
	char *end = s + strlen(s) - 1;

	while(isspace(*s)) s++;

	while(isspace(*end)) {
		*end-- = 0;
	}
	return s;
}

static void concat_values(struct cfgopt *opt)
{
	int i;
	int sz = opt->str_count - 1;

	for(i=0; i<opt->str_count; i++) {
		sz += strlen(opt->str_val[i]);
	}

	if(!(opt->conc_vals = malloc(sz + 1))) {
		return;
	}

	*opt->conc_vals = 0;
	for(i=0; i<opt->str_count - 1; i++) {
		strcat(opt->conc_vals, opt->str_val[i]);
		strcat(opt->conc_vals, " ");
	}
	strcat(opt->conc_vals, opt->str_val[i]);
}
