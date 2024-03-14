#!/usr/bin/make -f

CFLAGS = -std=gnu99 -Wall -Og -g -ggdb `pkg-config --cflags libpng`
LDFLAGS = `pkg-config --libs libpng`

DEPDIR = ./.deps

OBJS = file_io.o color.o bitmapimg.o tgafile.o bmpfile.o \
	pgmfile.o pngfile.o error.o \
	tgatest.o pngtest.o

DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OBJS))
SRCS = $(patsubst %.o,%.c,$(OBJS))

BINARIES = tgatest pngtest

.PHONY: all depend clean distclean

%.o: %.c
	@echo 'CC' $< '=>' $@
	@$(CC) $(CFLAGS) -c -o $@ $<

all: $(BINARIES)

$(DEPDIR)/%.d: %.c
	@mkdir -p $(DEPDIR)/
	@echo "  "$< '=>' $@
	@$(CC) $(CFLAGS) -MM -o $@ $<

ifeq ($(findstring $(MAKECMDGOALS), clean distclean),)
  -include $(DEPS)
endif

tgatest: tgatest.o bitmapimg.o color.o file_io.o tgafile.o pgmfile.o error.o
	@echo 'CC' $^
	@echo '    =>' $@
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

pngtest: pngtest.o bitmapimg.o color.o file_io.o tgafile.o pngfile.o error.o
	@echo 'CC' $^
	@echo '    =>' $@
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJS) $(BINARIES)

distclean:
	rm -f $(OBJS) $(BINARIES)
	rm -rf $(DEPDIR)

# EOF
