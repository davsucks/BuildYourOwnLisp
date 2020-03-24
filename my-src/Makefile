CC = cc
CFLAGS = -std=c99 -Wall -g
LFLAGS = -ledit -lm
FILES = hello_world parsing
PLATFORM = $(shell uname)
DEPS = mpc.c

ifeq ($(findstring Linux,$(PLATFORM)),Linux)
	LFLAGS += -ledit -lm
endif

ifeq ($(findstring Darwin,$(PLATFORM)),Darwin)
	LFLAGS += -ledit -lm
endif

all: $(FILES)

%: %.c $(DEPS)
	$(CC) $(CFLAGS) $^ $(LFLAGS) -o $@

clean:
	rm $(FILES)

