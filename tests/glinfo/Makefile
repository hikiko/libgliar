obj = glinfo.o
bin = glinfo

CFLAGS = -pedantic -Wall -g
LDFLAGS = -lGL -lglut -lGLEW

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)
