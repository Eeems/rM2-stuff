prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
bindir = ${exec_prefix}/bin
datadir = ${datarootdir}
pkgdatadir = ${datarootdir}/tilem2
mandir = ${datarootdir}/man

top_builddir = ..
top_srcdir = ..
srcdir = .



AR = ar
CC = gcc
CFLAGS = -g -O2 -W -Wall -Wwrite-strings
CPPFLAGS =
DEFS = -DHAVE_CONFIG_H
OPT_CFLAGS = -O3
RANLIB = ranlib
SHELL = /bin/sh

core_objects = calcs.o z80.o state.o rom.o flash.o link.o keypad.o lcd.o \
	cert.o md5.o timers.o monolcd.o graylcd.o grayimage.o graycolor.o

x7_objects = x7_init.o x7_io.o x7_memory.o x7_subcore.o
x1_objects = x1_init.o x1_io.o x1_memory.o x1_subcore.o
x2_objects = x2_init.o x2_io.o x2_memory.o x2_subcore.o
x3_objects = x3_init.o x3_io.o x3_memory.o x3_subcore.o
xp_objects = xp_init.o xp_io.o xp_memory.o xp_subcore.o
xs_objects = xs_init.o xs_io.o xs_memory.o xs_subcore.o
x4_objects = x4_init.o x4_io.o x4_memory.o x4_subcore.o
xz_objects = xz_init.o xz_io.o xz_memory.o xz_subcore.o
xn_objects = xn_init.o xn_io.o xn_memory.o xn_subcore.o
x5_objects = x5_init.o x5_io.o x5_memory.o x5_subcore.o
x6_objects = x6_init.o x6_io.o x6_memory.o x6_subcore.o

objects = $(core_objects) $(x7_objects) $(x1_objects) $(x2_objects) \
	$(x3_objects) $(xp_objects) $(xs_objects) $(x4_objects) $(xz_objects) \
	$(xn_objects) $(x5_objects) $(x6_objects)

compile = $(CC) -I$(top_builddir) -I$(srcdir) $(CFLAGS) $(CPPFLAGS) $(DEFS) $(OPT_CFLAGS)

all: libtilemcore.a

libtilemcore.a: $(objects)
	$(AR) cru libtilemcore.a $(objects)
	$(RANLIB) libtilemcore.a

# Main emulator core

calcs.o: calcs.c tilem.h z80.h ../config.h
	$(compile) -c $(srcdir)/calcs.c
z80.o: z80.c z80.h z80cmds.h z80main.h z80cb.h z80ddfd.h z80ed.h tilem.h ../config.h
	$(compile) -c $(srcdir)/z80.c
state.o: state.c tilem.h z80.h ../config.h
	$(compile) -c $(srcdir)/state.c
rom.o: rom.c tilem.h ../config.h
	$(compile) -c $(srcdir)/rom.c
flash.o: flash.c tilem.h ../config.h
	$(compile) -c $(srcdir)/flash.c
link.o: link.c tilem.h ../config.h
	$(compile) -c $(srcdir)/link.c
keypad.o: keypad.c tilem.h scancodes.h ../config.h
	$(compile) -c $(srcdir)/keypad.c
lcd.o: lcd.c tilem.h ../config.h
	$(compile) -c $(srcdir)/lcd.c
cert.o: cert.c tilem.h ../config.h
	$(compile) -c $(srcdir)/cert.c
md5.o: md5.c tilem.h ../config.h
	$(compile) -c $(srcdir)/md5.c
timers.o: timers.c tilem.h ../config.h
	$(compile) -c $(srcdir)/timers.c
monolcd.o: monolcd.c tilem.h ../config.h
	$(compile) -c $(srcdir)/monolcd.c
graylcd.o: graylcd.c tilem.h graylcd.h ../config.h
	$(compile) -c $(srcdir)/graylcd.c
grayimage.o: grayimage.c tilem.h graylcd.h ../config.h
	$(compile) -c $(srcdir)/grayimage.c
graycolor.o: graycolor.c tilem.h ../config.h
	$(compile) -c $(srcdir)/graycolor.c

# TI-73

x7_init.o: x7/x7_init.c x7/x7.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x7/x7_init.c
x7_io.o: x7/x7_io.c x7/x7.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x7/x7_io.c
x7_memory.o: x7/x7_memory.c x7/x7.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x7/x7_memory.c
x7_subcore.o: x7/x7_subcore.c x7/x7.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x7/x7_subcore.c

# TI-81

x1_init.o: x1/x1_init.c x1/x1.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x1/x1_init.c
x1_io.o: x1/x1_io.c x1/x1.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x1/x1_io.c
x1_memory.o: x1/x1_memory.c x1/x1.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x1/x1_memory.c
x1_subcore.o: x1/x1_subcore.c x1/x1.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x1/x1_subcore.c

# TI-82

x2_init.o: x2/x2_init.c x2/x2.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x2/x2_init.c
x2_io.o: x2/x2_io.c x2/x2.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x2/x2_io.c
x2_memory.o: x2/x2_memory.c x2/x2.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x2/x2_memory.c
x2_subcore.o: x2/x2_subcore.c x2/x2.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x2/x2_subcore.c

# TI-83

x3_init.o: x3/x3_init.c x3/x3.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x3/x3_init.c
x3_io.o: x3/x3_io.c x3/x3.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x3/x3_io.c
x3_memory.o: x3/x3_memory.c x3/x3.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x3/x3_memory.c
x3_subcore.o: x3/x3_subcore.c x3/x3.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x3/x3_subcore.c

# TI-83 Plus

xp_init.o: xp/xp_init.c xp/xp.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xp/xp_init.c
xp_io.o: xp/xp_io.c xp/xp.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xp/xp_io.c
xp_memory.o: xp/xp_memory.c xp/xp.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xp/xp_memory.c
xp_subcore.o: xp/xp_subcore.c xp/xp.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xp/xp_subcore.c

# TI-83 Plus SE

xs_init.o: xs/xs_init.c xs/xs.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xs/xs_init.c
xs_io.o: xs/xs_io.c xs/xs.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xs/xs_io.c
xs_memory.o: xs/xs_memory.c xs/xs.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xs/xs_memory.c
xs_subcore.o: xs/xs_subcore.c xs/xs.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xs/xs_subcore.c

# TI-84 Plus

x4_init.o: x4/x4_init.c x4/x4.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x4/x4_init.c
x4_io.o: x4/x4_io.c x4/x4.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x4/x4_io.c
x4_memory.o: x4/x4_memory.c x4/x4.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x4/x4_memory.c
x4_subcore.o: x4/x4_subcore.c x4/x4.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x4/x4_subcore.c

# TI-84 Plus SE

xz_init.o: xz/xz_init.c xz/xz.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xz/xz_init.c
xz_io.o: xz/xz_io.c xz/xz.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xz/xz_io.c
xz_memory.o: xz/xz_memory.c xz/xz.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xz/xz_memory.c
xz_subcore.o: xz/xz_subcore.c xz/xz.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xz/xz_subcore.c

# TI-Nspire 84 Plus emulator

xn_init.o: xn/xn_init.c xn/xn.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xn/xn_init.c
xn_io.o: xn/xn_io.c xn/xn.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xn/xn_io.c
xn_memory.o: xn/xn_memory.c xn/xn.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xn/xn_memory.c
xn_subcore.o: xn/xn_subcore.c xn/xn.h tilem.h ../config.h
	$(compile) -c $(srcdir)/xn/xn_subcore.c

# TI-85

x5_init.o: x5/x5_init.c x5/x5.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x5/x5_init.c
x5_io.o: x5/x5_io.c x5/x5.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x5/x5_io.c
x5_memory.o: x5/x5_memory.c x5/x5.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x5/x5_memory.c
x5_subcore.o: x5/x5_subcore.c x5/x5.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x5/x5_subcore.c

# TI-86

x6_init.o: x6/x6_init.c x6/x6.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x6/x6_init.c
x6_io.o: x6/x6_io.c x6/x6.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x6/x6_io.c
x6_memory.o: x6/x6_memory.c x6/x6.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x6/x6_memory.c
x6_subcore.o: x6/x6_subcore.c x6/x6.h tilem.h ../config.h
	$(compile) -c $(srcdir)/x6/x6_subcore.c


clean:
	rm -f *.o
	rm -f libtilemcore.a


Makefile: Makefile.in $(top_builddir)/config.status
	cd $(top_builddir) && $(SHELL) ./config.status

$(top_builddir)/config.status: $(top_srcdir)/configure
	cd $(top_builddir) && $(SHELL) ./config.status --recheck

.PRECIOUS: Makefile $(top_builddir)/config.status
.PHONY: clean all
