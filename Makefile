CC = cc
CFLAGS = -g -I$(libswftag_root)/include -Wall -std=c99 -pedantic

libswftag_root = ../libswftag
libswftag_lib_root = ../libswftag/lib

objs = \
	error.o \
	swfcheck.o \

libs = -lz

build/swfcheck: $(objs) $(libswftag_lib_root)/libswftag.a
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) $^ -o $@ $(libs)
	rm -f $(objs)

$(libswftag_lib_root)/libswftag.a: $(libswftag_root)
	$(MAKE) EXTRA_FLAGS=$(EXTRA_FLAGS) -C $(libswftag_root)

%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS) $(EXTRA_FLAGS) -c $< -o $@
