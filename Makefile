CC = clang
LD = $(CC)
AR = ar

SYSROOT = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
SRCDIR = src
OBJDIR = bld
ARFLAGS = -cvrs

CFLAGS = -isysroot $(SYSROOT) -std=c99 -O0 -g -Wall -pedantic -pedantic-errors -c -Iinclude
LDFLAGS = -isysroot $(SYSROOT) -w

OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(wildcard $(SRCDIR)/*.c))

all: bld/libjsonz.a

bld/libjsonz.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $^

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJECTS) bld/libjsonz.a

