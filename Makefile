src = $(wildcard src/*.c)
obj = $(src:.c=.o)
lib_so = libgliar.so

CFLAGS = -pedantic -Wall -g -fPIC -D_GNU_SOURCE

$(lib_so): $(obj)
	$(CC) -o $@ -shared $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(lib_so)
