CC = cc
CFLAGS = -O2 -g -I$(libswftag_root)/include -Wall -std=c99 -pedantic

libswftag_root = ../libswftag
libswftag_lib_root = ../libswftag/lib

objs = \
	error.o \
	swfcheck.o \

libs = -lz

swfcheck: $(objs) $(libswftag_lib_root)/libswftag.a
	$(CC) $(CFLAGS) $^ -o $@ $(libs)

$(libswftag_lib_root)/libswftag.a:
	$(MAKE) -C $(libswftag_root)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(objs)
