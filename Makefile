CFLAGS = -std=c++17 -O2 -I ./include
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
SOURCES = src/main.cpp

.PHONY: clean uninstall

isometric: src/main.cpp install
	g++ ${CFLAGS} -o isometric ${SOURCES} ${LDFLAGS}

compile_commands.json:
	bear -- make

install:
	sudo pacman -S glfw-x11 glm shaderc --noconfirm
	touch install

uninstall: 
	make clean
	rm -f install

clean:
	rm -f isometric
