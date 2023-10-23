sources := main.c
outputs = $(wildcard build/*.o)
INC = -Ilib/sokol -Ilib/sokol/util -Isrc/

NAME = el.exe

CFLAGS += -Wall
SOKOL = -DSOKOL_GLCORE33

sokol:
	git clone https://github.com/floooh/sokol lib/sokol

main.c:
	@$(CC) src/main.c -c -o build/main.o $(CFLAGS) $(INC) $(SOKOL)

scene.c:
	@$(CC) src/scene.c -c -o build/scene.o $(CFLAGS) $(INC) $(SOKOL)

exe: main.c scene.c sokol
	@echo $(outputs)
	$(CC) $(outputs) -o build/$(NAME) -Wall $(CFLAGS) $(INC) $(SOKOL)

build: main.c scene.c
	@echo "Built!"


run:
	.\build\el.exe