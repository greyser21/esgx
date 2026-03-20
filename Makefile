CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude \
          $(shell sdl-config --cflags)
LDFLAGS = $(shell sdl-config --libs) -lm

LIB_SRCS = src/esgx_window.c  \
           src/esgx_event.c   \
           src/esgx_anim.c    \
           src/esgx_draw.c    \
           src/esgx_entity.c

LIB_OBJS = $(LIB_SRCS:.c=.o)
LIB      = libesgx.a

.PHONY: all lib demos clean

all: lib demos

lib: $(LIB)

$(LIB): $(LIB_OBJS)
	ar rcs $@ $^
	@echo "[ESGX] Bibliothèque $(LIB) créée."

src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

demos: demo_turtle demo_entities demo_painter

demo_turtle: demos/demo_turtle.c $(LIB)
	$(CC) $(CFLAGS) $< -o $@ -L. -lesgx $(LDFLAGS)
	@echo "[ESGX] demo_turtle compilé."

demo_entities: demos/demo_entities.c $(LIB)
	$(CC) $(CFLAGS) $< -o $@ -L. -lesgx $(LDFLAGS)
	@echo "[ESGX] demo_entities compilé."

demo_painter: demos/demo_painter.c $(LIB)
	$(CC) $(CFLAGS) $< -o $@ -L. -lesgx $(LDFLAGS)
	@echo "[ESGX] demo_painter compilé."

clean:
	rm -f $(LIB_OBJS) $(LIB) demo_turtle demo_entities demo_painter
	@echo "[ESGX] Nettoyage terminé."
