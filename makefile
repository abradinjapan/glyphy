debug:
	gcc src/main.c -Wall -fsanitize=address -lSDL2 -lGLEW -lGL -o bin/glyphy-debug

release:
	gcc src/main.c -Wall -lSDL2 -lGLEW -lGL -o bin/glyphy
