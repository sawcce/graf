INC = -Ilib/sokol -Ilib/sokol/util -Ilib/logc/src -Isrc/ -Ilib/cglm/include -Ilib/stc/include

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

stc:
	@git clone https://github.com/stclib/stc lib/stc 

.PHONY: build/stc
build/stc:
	$(CC) lib/stc/src/libstc.c -c -o build/libstc.o $(CFLAGS) $(INC)

.PHONY: bootstrap
bootstrap: sokol logc cglm stc build/stc
	mkdir build

build/main.o: src/main.c
	@echo "Building main..."
	$(CC) src/main.c -c -o build/main.o $(CFLAGS) $(INC) $(SOKOL)

build/scene.o: src/scene.c
	@echo "Building scene..."
	$(CC) src/scene.c -c -o build/scene.o $(CFLAGS) $(INC) $(SOKOL)

build/ecs.o: src/ecs.c
	@echo "Building ecs..."
	$(CC) src/ecs.c -c -o build/ecs.o $(CFLAGS) $(INC) $(SOKOL)

build/log.o: lib/logc/src/log.c
	$(CC) lib/logc/src/log.c -c -o build/log.o $(CFLAGS)

# I am fully aware that this is far from optimal and idiomatical
# but this is the best solution I could come up with for now
.PHONY: all $(src)
all: $(src)
$(src):
	echo "Building: $@" "$<"
	$(CC) $@ -c -o build/$(@:src/components/%.c=%.o) $(CFLAGS) $(INC) $(SOKOL)

exe: build/main.o build/scene.o build/ecs.o all build/log.o build/stc
	@echo "Outputs: " $(outputs)
	@echo "Outputs: " $(obj)
	$(CC) $(outputs) -o build/$(NAME) -Wall $(CFLAGS) $(INC) $(SOKOL)

build: exe
	@echo "Built!"


run:
	.\build\el.exe