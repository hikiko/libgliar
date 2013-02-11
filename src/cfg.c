#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cfg.h"

static char *stripspace(char *s);


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
			}

			if((opt = malloc(sizeof *opt))) {
				if((opt->key = malloc(strlen(line) + 1))) {
					strcpy(opt->key, line);
					opt->val = 0;
				} else {
					free(opt);
					opt = 0;
				}
			}
		} else {
			char *tmp;
			int prev_len = opt->val ? strlen(opt->val) : 0;

			if(opt && (tmp = realloc(opt->val, prev_len + strlen(line) + 2))) {
				opt->val = tmp;
				if(prev_len) {
					strcat(opt->val, " ");
					strcat(opt->val, line);
				} else {
					strcpy(opt->val, line);
				}
			}
		}
	}

	if(opt) {
		opt->next = optlist;
		optlist = opt;
	}

	fclose(fp);
	return optlist;
}

const char *gliar_find_opt(struct cfgopt *list, const char *name)
{
	if(!list || !name) {
		return 0;
	}

	while(list) {
		if(strcmp(list->key, name) == 0) {
			return list->val;
		}
		list = list->next;
	}
	return 0;
}

void gliar_print_opt(struct cfgopt *list)
{
	printf("OPTIONS\n");
	while(list) {
		printf("\"%s\" -> \"%s\"\n", list->key, list->val);
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
