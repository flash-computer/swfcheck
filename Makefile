CC = cc
CFLAGS = -O2 -g -I$(libswftag_root)/include -Wall -std=c99 -pedantic

libswftag_root = ../libswftag

objs = \
	error.o \
	swfcheck.o \

swfcheck: $(objs) $(libswftag_root)/libswftag.a
	$(CC) $(CFLAGS) $^ -o $@

$(libswftag_root)/libswftag.a:
	$(MAKE) -C $(libswftag_root)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(objs)
