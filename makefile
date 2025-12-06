EXE=final

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

# NEW HW6 object dependencies
House.o: House.c CSCIx229.h House.h
Tree.o: Tree.c CSCIx229.h Tree.h
Human.o: Human.c CSCIx229.h Human.h
Target.o: Target.c CSCIx229.h Target.h
BrokenTable.o: BrokenTable.c CSCIx229.h BrokenTable.h
OldCar.o: OldCar.c CSCIx229.h OldCar.h
Boulder.o: Boulder.c CSCIx229.h Boulder.h
Grenade.o: Grenade.c CSCIx229.h Grenade.h
HealthKit.o: HealthKit.c CSCIx229.h HealthKit.h
Primitives.o: Primitives.c CSCIx229.h Primitives.h
Helicopter.o: Helicopter.c CSCIx229.h Helicopter.h
Plane.o: Plane.c CSCIx229.h Plane.h
FrontPlane.o: FrontPlane.c CSCIx229.h Plane.h


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


# final: LunarOutpost.o FloatingCrystalShard.o shader_loader.o AlienScoutDrone.o BioluminescentSporeCloud.o GeometricAlienFlora.o LunarisVex.o UFOSpacecraft.o ArmoredTransportVehicle.o AlienFloraCollection.o JeepWrangler.o CSCIx229.a


final: LunarOutpost.o FloatingCrystalShard.o shader_loader.o AlienScoutDrone.o LunarisVex.o UFOSpacecraft.o ArmoredTransportVehicle.o AlienFloraCollection.o JeepWrangler.o astronaut.o House.o Tree.o Human.o Target.o BrokenTable.o OldCar.o Boulder.o Grenade.o HealthKit.o Primitives.o Helicopter.o Plane.o FrontPlane.o CSCIx229.a
	gcc $(CFLG) -o $@ $^  $(LIBS)
	

#  Clean
clean:
	$(CLEAN)