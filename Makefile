INC = -Ilib/sokol -Ilib/sokol/util -Ilib/logc/src -Isrc/ -Ilib/cglm/include

src = $(wildcard src/components/*.c)
outputs = $(wildcard build/*.o)

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
	$(CC) src/main.c -c -o build/main.o $(CFLAGS) $(INC) $(SOKOL)

build/scene.o: src/scene.c
	@echo "Building scene..."
	$(CC) src/scene.c -c -o build/scene.o $(CFLAGS) $(INC) $(SOKOL)

build/log.o: lib/logc/src/log.c
	$(CC) lib/logc/src/log.c -c -o build/log.o $(CFLAGS)

# I am fully aware that this is far from optimal and idiomatical
# but this is the best solution I could come up with for now
.PHONY: all $(src)
all: $(src)
$(src):
	echo "Building: $@" "$<"
	$(CC) $@ -c -o build/$(@:src/components/%.c=%.o) $(CFLAGS) $(INC) $(SOKOL)

exe: build/main.o build/scene.o all build/log.o
	@echo "Outputs: " $(outputs)
	@echo "Outputs: " $(obj)
	$(CC) $(outputs) -o build/$(NAME) -Wall $(CFLAGS) $(INC) $(SOKOL)

build: exe
	@echo "Built!"


run:
	.\build\el.exe