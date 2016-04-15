# Makefile for project 6
# make - Build p6_exe
#
# make clean - Delete the .o files.
#
# make real_clean - Delete the .o files and the two executables.

# Note how variables are used for ease of modification.

CC = g++
LD = g++

# specify compile and link options
CFLAGS = -c -std=c++14 -pedantic-errors -Wall -Wextra
LFLAGS = -Wall

SOURCES = Bridge_view.cpp Controller.cpp Cruiser.cpp Cruise_ship.cpp Geometry.cpp \
		  Grid_location_view.cpp Island.cpp Map_view.cpp Model.cpp Navigation.cpp \
		  p6_main.cpp Sailing_view.cpp Ship_component.cpp Ship.cpp Ship_factory.cpp \
		  Ship_group.cpp Sim_object.cpp Skimmer.cpp Tanker.cpp Torpedo_boat.cpp \
		  Track_base.cpp Utility.cpp View.cpp Warship.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EX = p6exe

all: $(SOURCES) $(EX)
    
$(EX): $(OBJECTS) 
	$(CC) $(LFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@


# other shell commands can appear as "things to do" - not just compilations, etc.
clean:
	rm -f *.o p6exe

