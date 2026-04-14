
NAME_ALL=BCU_Simulation
NAME_UNITTEST=BCU_unittest

ifeq ($(OS), Windows_NT)
#TODO: Windows Compilation Flags

#Unix
else
  OS:= $(shell uname)
  # MacOS Compilation Flags
  ifeq ($(OS), Darwin)
    CFLAGS+=-I"/opt/homebrew/include"
    LDFLAGS+=-L"/opt/homebrew/lib" -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
  endif
  # Linux Compilation Flags
  ifeq ($(OS), Linux)
    CFLAGS+=-I"/usr/include" -I"/usr/local/include"
    LDFLAGS+=-L"/usr/lib" -I"/usr/local/lib" -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
  endif
endif

all:
	g++ $(CFLAGS) -c -o bcu_simulation.o bcu_simulation.cpp
	g++ $(LDFLAGS) bcu_simulation.o -o $(NAME_ALL)          

clean:
	-rm -rf *.o
	-rm $(NAME_ALL)
	-rm $(NAME_UNITTEST)