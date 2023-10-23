sources := main.c
outputs = $(wildcard build/*.o)
INC = -Ilib/sokol -Ilib/sokol/util -Ilib/logc -Isrc/

NAME = el.exe

CFLAGS += -Wall
SOKOL = -DSOKOL_GLCORE33

sokol:
	@git clone https://github.com/floooh/sokol lib/sokol

logc:
	@git clone https://github.com/rxi/log.c lib/logc 

.PHONY: bootstrap
bootstrap: sokol logc
	mkdir build

build/main.o: src/main.c
	@$(CC) src/main.c -c -o build/main.o $(CFLAGS) $(INC) $(SOKOL)

build/scene.o: src/main.c
	@$(CC) src/scene.c -c -o build/scene.o $(CFLAGS) $(INC) $(SOKOL)

exe: src/main.c src/scene.c
	@echo $(outputs)
	$(CC) $(outputs) -o build/$(NAME) -Wall $(CFLAGS) $(INC) $(SOKOL)

build: exe
	@echo "Built!"


run:
	.\build\el.exe