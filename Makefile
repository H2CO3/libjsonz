TARGET = libjsonz.so
OBJECTS = jsonz.o jsonz-internal.o
HEADERS = jsonz.h

CC = gcc
LD = $(CC)

CFLAGS = -Wall -std=gnu99 -fPIC -I. -c

LDFLAGS = -fPIC -shared


all: $(TARGET)

prettify: install
	gcc -Wall -std=gnu99 -o jsonzprettify jsonzprettify.c -ljsonz

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o libjsonz.so jsonz.o jsonz-internal.o

install: $(TARGET)
	cp $(TARGET) /usr/lib
	cp -r $(HEADERS) /usr/include

clean:
	rm -f $(OBJECTS) $(TARGET) jsonzprettify

uninstall:
	rm /usr/lib/$(TARGET)
	rm /usr/include/$(HEADERS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $^

