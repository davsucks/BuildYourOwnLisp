CC = cc
CFLAGS = -std=c99 -Wall -g
LFLAGS = -ledit -lm
FILES = hello_world parsing
PLATFORM = $(shell uname)
DEPS = lval.h lenv.h
OBJ = parsing.o constructors.o mpc.o

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	LFLAGS += -ledit -lm
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	LFLAGS += -ledit -lm
endif

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

parsing: $(OBJ)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

.PHONY: clean

clean:
	rm *.o
	rm $(FILES)

