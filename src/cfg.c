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

/* linked list of valid value filters */
static struct cfgopt *valid_vals;

/* adds a new filter set of valid values for the key "key" */
void gliar_value_set(const char *key, char **valid, int vcount)
{
	int i;
	struct cfgopt *node;

	if(!(node = malloc(sizeof *node))) {
		return;
	}
	if(!(node->key = malloc(strlen(key) + 1))) {
		free(node);
		return;
	}
	strcpy(node->key, key);

	if(!(node->str_val = malloc(vcount * sizeof *node->str_val))) {
		free(node->key);
		free(node);
		return;
	}
	for(i=0; i<vcount; i++) {
		if(!(node->str_val[i] = malloc(strlen(valid[i]) + 1))) {
			for(; i>=0; i--) {
				free(node->str_val[i]);
				free(node->key);
				free(node);
				return;
			}
		}
		strcpy(node->str_val[i], valid[i]);
	}

	node->str_count = vcount;

	node->next = valid_vals;
	valid_vals = node;
}

struct cfgopt *gliar_load_cfg(const char *fname)
{
	FILE *fp;
	char buf[512];
	struct cfgopt *optlist = 0;
	struct cfgopt *opt = 0;
	const struct cfgopt *filter;

	if(!(fp = fopen(fname, "r"))) {
		return 0;
	}

	while(fgets(buf, sizeof buf, fp)) {
		char *line = stripspace(buf);

		if(!*line || *line == '#') {
			continue;
		}

		if(*line == '[') {
			/* found a new key */
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

			/* find the valid values for this particular key (if any) */
			filter = gliar_find_opt(valid_vals, line);

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
			/* found a value for the current key (opt->key) */
			int i, num, new_sz = opt->str_count + 1;
			char **tmp;
			char *end;

			if(filter) {
				for(i=0; i<filter->str_count; i++) {
					if(strcmp(line, filter->str_val[i]) == 0) {
						break;
					}
				}

				if(i == filter->str_count) {
					/* the string is not in the valid list, ignore it */
					fprintf(stderr, "GLIAR: extension %s not supported, ignoring\n", line);
					continue;
				}
			}

			num = strtol(line, &end, 10);
			if(!*end) {
				opt->num_val = num;
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

	if(!opt->str_count) {
		opt->conc_vals = 0;
		return;
	}

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
