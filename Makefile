TARGET = libjsonz.dylib
OBJECTS = jsonz.o jsonz-internal.o jsonz-object.o
HEADERS = jsonz

CC = gcc
LD = $(CC)

CFLAGS = -isysroot /User/sysroot -Wall -I. -c

LDFLAGS = -isysroot /User/sysroot -w -dynamiclib -install_name /usr/lib/$(TARGET)


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

install: $(TARGET)
	cp $^ /usr/lib
	cp $^ /User/sysroot/usr/lib
	cp -r $(HEADERS) /usr/include
	cp -r $(HEADERS) /User/sysroot/usr/include

clean:
	rm -f $(OBJECTS) $(TARGET)

uninstall:
	rm /usr/lib/$(TARGET)
	rm /usr/include/$(HEADERS)/*

%.o: %.c
	$(CC) $(CFLAGS) $^
