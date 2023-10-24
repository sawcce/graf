sources := main.c
outputs = $(wildcard build/*.o) $(wildcard lib/cglm/build/**.o)
INC = -Ilib/sokol -Ilib/sokol/util -Ilib/logc/src -Isrc/ -Ilib/cglm/include

cglmI = $(wildcard lib/cglm/src/**.c)

NAME = el.exe

CFLAGS += -Wall
SOKOL = -DSOKOL_GLCORE33

sokol:
	@git clone https://github.com/floooh/sokol lib/sokol

logc:
	@git clone https://github.com/rxi/log.c lib/logc 

cglm:
	@git clone https://github.com/recp/cglm lib/cglm

.PHONY: bootstrap
bootstrap: sokol logc cglm
	mkdir build

build/main.o: src/main.c
	@echo "Building main..."
	@$(CC) src/main.c -c -o build/main.o $(CFLAGS) $(INC) $(SOKOL)

build/scene.o: src/scene.c
	@echo "Building scene..."
	@$(CC) src/scene.c -c -o build/scene.o $(CFLAGS) $(INC) $(SOKOL)

build/log.o: lib/logc/src/log.c
	@$(CC) lib/logc/src/log.c -c -o build/log.o $(CFLAGS)

.PHONY: $(cglmI)
$(cglmI):
	@$(CC) $@ -c -o build/$(@:lib/cglm/src/%.c=%.o) $(CFLAGS) $(INC)

exe: build/main.o build/scene.o build/log.o
	@echo $(outputs)
	$(CC) $(outputs) -o build/$(NAME) -Wall $(CFLAGS) $(INC) $(SOKOL)

build: exe
	@echo "Built!"


run:
	.\build\el.exe