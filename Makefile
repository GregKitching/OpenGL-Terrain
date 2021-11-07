LDFLAGS = -lGL -lGLU -lglut
CFLAGS=-g -Wall -std=c++11
CC=g++
EXEEXT=
RM=rm
PROGRAM_NAME = Terrain
run: $(PROGRAM_NAME)
	./$(PROGRAM_NAME)$(EXEEXT)
$(PROGRAM_NAME): Terrain.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
clean:
	$(RM) *.o $(PROGRAM_NAME)$(EXEEXT)
