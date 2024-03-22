build:
	g++ -Wall -std=c++17 -I"./libs" src/*.cpp -lSDL2 -lSDL2_image -lSDL_ttf -lSDL2_mixer -llua -o gameengine
clean:
	rm gameengine
run:
	./gameengine
