#ifndef CFG_H_
#define CFG_H_

struct cfgopt {
	char *key;
	char *val;

	struct cfgopt *next;
};

struct cfgopt *gliar_load_cfg(const char *fname);

const char *gliar_find_opt(struct cfgopt *list, const char *name);

void gliar_print_opt(struct cfgopt *list);

#endif	/* CFG_H_ */
