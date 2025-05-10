CC = cc
CFLAGS = -O2 -g -I$(libswftag_lib_root)/include -Wall -std=c99 -pedantic

libswftag_lib_root = ../libswftag/lib

objs = \
	error.o \
	swfcheck.o \

swfcheck: $(objs) $(libswftag_lib_root)/libswftag.a
	$(CC) $(CFLAGS) $^ -o $@

$(libswftag_lib_root)/libswftag.a:
	$(MAKE) -C $(libswftag_lib_root)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(objs)
