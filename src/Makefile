#!/usr/bin/make -f

CFLAGS = -Wall -O2
LDFLAGS = 

DEPDIR = ./.deps

OBJS = file_io.o color.o bitmapimg.o tgafile.o bmpfile.o \
	pgmfile.o error.o \
	tgatest.o

DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OBJS))
SRCS = $(patsubst %.o,%.c,$(OBJS))

BINARIES = tgatest

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

clean:
	rm -f $(OBJS) $(BINARIES)

distclean:
	rm -f $(OBJS) $(BINARIES)
	rm -rf $(DEPDIR)

# EOF
