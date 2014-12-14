
CFLAGS ?= -O2 -g -DENABLE_TRACING
WFLAGS ?= -Wall -Wextra -Wmissing-prototypes

SOURCES := $(wildcard *.c)
OBJECTS := $(SOURCES:%.c=%.o)

libltunify.a: $(OBJECTS)
	#$(CC) $(WFLAGS) $(CFLAGS) -o $@ $^ -lrt -c
	ar rcs $@ $^

clean:
	$(RM) libltunify.a $(OBJECTS)

%.o: %.c
	$(CC) -c $(WFLAGS) $(CFLAGS) -o $@ $<
