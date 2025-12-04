EXE=L

# Main target
all: $(EXE)

#  Msys/MinGW
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
CLEAN=rm -f *.exe *.o *.a
else
#  OSX
ifeq "$(shell uname)" "Darwin"
CFLG=-O3 -Wall -Wno-deprecated-declarations
LIBS=-framework GLUT -framework OpenGL
#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall
LIBS=-lglut -lGLU -lGL -lm
endif
#  OSX/Linux/Unix/Solaris
CLEAN=rm -f $(EXE) *.o *.a
endif

# Dependencies
LunarOutpost.o: LunarOutpost.c CSCIx229.h
FloatingCrystalShard.o: FloatingCrystalShard.c CSCIx229.h shader_loader.h
shader_loader.o: shader_loader.c shader_loader.h
AlienScoutDrone.o: AlienScoutDrone.c CSCIx229.h shader_loader.h
# BioluminescentSporeCloud.o: BioluminescentSporeCloud.c CSCIx229.h
# GeometricAlienFlora.o: GeometricAlienFlora.c CSCIx229.h
LunarisVex.o: LunarisVex.c CSCIx229.h shader_loader.h
UFOSpacecraft.o: UFOSpacecraft.c CSCIx229.h
ArmoredTransportVehicle.o: ArmoredTransportVehicle.c CSCIx229.h
AlienFloraCollection.o: AlienFloraCollection.c CSCIx229.h
JeepWrangler.o: JeepWrangler.c CSCIx229.h
astronaut.o: astronaut.c CSCIx229.h astronaut.h

fatal.o: fatal.c CSCIx229.h
errcheck.o: errcheck.c CSCIx229.h
print.o: print.c CSCIx229.h
loadtexbmp.o: loadtexbmp.c CSCIx229.h
loadobj.o: loadobj.c CSCIx229.h
projection.o: projection.c CSCIx229.h

#  Create archive
CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
	ar -rcs $@ $^

# Compile rules
.c.o:
	gcc -c $(CFLG)  $<
.cpp.o:
	g++ -c $(CFLG)  $<


# L: LunarOutpost.o FloatingCrystalShard.o shader_loader.o AlienScoutDrone.o BioluminescentSporeCloud.o GeometricAlienFlora.o LunarisVex.o UFOSpacecraft.o ArmoredTransportVehicle.o AlienFloraCollection.o JeepWrangler.o CSCIx229.a

L: LunarOutpost.o FloatingCrystalShard.o shader_loader.o AlienScoutDrone.o LunarisVex.o UFOSpacecraft.o ArmoredTransportVehicle.o AlienFloraCollection.o JeepWrangler.o astronaut.o CSCIx229.a
	gcc $(CFLG) -o $@ $^  $(LIBS)
	

#  Clean
clean:
	$(CLEAN)