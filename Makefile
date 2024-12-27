CXX=g++
CXXFLAGS:=-Wall -Wextra -pedantic -std=c++20 -Iheader
# CXXFLAGS:=-pedantic -std=c++20 -Iheader

RELEASEFLAGS:=-O3

OUTPUT := trains

.PHONY: all clean

all: $(OUTPUT) 


$(OUTPUT): simulate.cc main.cc
	mpicxx $(CXXFLAGS) $(RELEASEFLAGS) -o $@ $^

bonus: $(OUTPUT)-bonus

$(OUTPUT)-bonus: simulate.cc main.cc
	mpicxx $(CXXFLAGS) $(RELEASEFLAGS) -o $@ $^
	
clean:
	$(RM) *.o $(OUTPUT) $(OUTPUT)-bonus
