CFLAGS = -std=c++17 -O2 -I ./include
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
SOURCES = src/main.cpp

.PHONY: clean uninstall

isometric: src/main.cpp shader install
	g++ ${CFLAGS} -o isometric ${SOURCES} ${LDFLAGS} -O0 -g

shader: vert.spv frag.spv

vert.spv: shaders/shader.vert install
	glslc shaders/shader.vert -o vert.spv

frag.spv: shaders/shader.frag install
	glslc shaders/shader.frag -o frag.spv

compile_commands.json:
	bear -- make

install:
	sudo pacman -S glfw-x11 glm shaderc --noconfirm
	touch install

uninstall: 
	make clean
	rm -f install

clean:
	rm -f isometric vert.spv frag.spv
