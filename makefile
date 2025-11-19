# # # # Example 13
# # # EXE=hw6

# # # # Main target
# # # all: $(EXE)

# # # #  Msys/MinGW
# # # ifeq "$(OS)" "Windows_NT"
# # # CFLG=-O3 -Wall -DUSEGLEW
# # # LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
# # # CLEAN=rm -f *.exe *.o *.a
# # # else
# # # #  OSX
# # # ifeq "$(shell uname)" "Darwin"
# # # CFLG=-O3 -Wall -Wno-deprecated-declarations
# # # LIBS=-framework GLUT -framework OpenGL
# # # #  Linux/Unix/Solaris
# # # else
# # # CFLG=-O3 -Wall
# # # LIBS=-lglut -lGLU -lGL -lm
# # # endif
# # # #  OSX/Linux/Unix/Solaris
# # # CLEAN=rm -f $(EXE) *.o *.a
# # # endif

# # # # Dependencies
# # # hw6.o: hw6.c CSCIx229.h
# # # fatal.o: fatal.c CSCIx229.h
# # # errcheck.o: errcheck.c CSCIx229.h
# # # print.o: print.c CSCIx229.h
# # # loadtexbmp.o: loadtexbmp.c CSCIx229.h
# # # loadobj.o: loadobj.c CSCIx229.h
# # # projection.o: projection.c CSCIx229.h

# # # #  Create archive
# # # CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
# # # 	ar -rcs $@ $^

# # # # Compile rules
# # # .c.o:
# # # 	gcc -c $(CFLG)  $<
# # # .cpp.o:
# # # 	g++ -c $(CFLG)  $<

# # # #  Link
# # # hw6:hw6.o   CSCIx229.a
# # # 	gcc $(CFLG) -o $@ $^  $(LIBS)

# # # #  Clean
# # # clean:
# # # 	$(CLEAN)




# # # Example 13
# # EXE=L

# # # Main target
# # all: $(EXE)

# # #  Msys/MinGW
# # ifeq "$(OS)" "Windows_NT"
# # CFLG=-O3 -Wall -DUSEGLEW
# # LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
# # CLEAN=rm -f *.exe *.o *.a
# # else
# # #  OSX
# # ifeq "$(shell uname)" "Darwin"
# # CFLG=-O3 -Wall -Wno-deprecated-declarations
# # LIBS=-framework GLUT -framework OpenGL
# # #  Linux/Unix/Solaris
# # else
# # CFLG=-O3 -Wall
# # LIBS=-lglut -lGLU -lGL -lm
# # endif
# # #  OSX/Linux/Unix/Solaris
# # CLEAN=rm -f $(EXE) *.o *.a
# # endif

# # # Dependencies
# # LunarOutpost.o: LunarOutpost.c CSCIx229.h
# # fatal.o: fatal.c CSCIx229.h
# # errcheck.o: errcheck.c CSCIx229.h
# # print.o: print.c CSCIx229.h
# # loadtexbmp.o: loadtexbmp.c CSCIx229.h
# # loadobj.o: loadobj.c CSCIx229.h
# # projection.o: projection.c CSCIx229.h

# # #  Create archive
# # CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
# # 	ar -rcs $@ $^

# # # Compile rules
# # .c.o:
# # 	gcc -c $(CFLG)  $<
# # .cpp.o:
# # 	g++ -c $(CFLG)  $<

# # #  Link
# # L: LunarOutpost.o   CSCIx229.a
# # 	gcc $(CFLG) -o $@ $^  $(LIBS)

# # #  Clean
# # clean:
# # 	$(CLEAN)


# # # Example 13
# # EXE=hw6

# # # Main target
# # all: $(EXE)

# # #  Msys/MinGW
# # ifeq "$(OS)" "Windows_NT"
# # CFLG=-O3 -Wall -DUSEGLEW
# # LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
# # CLEAN=rm -f *.exe *.o *.a
# # else
# # #  OSX
# # ifeq "$(shell uname)" "Darwin"
# # CFLG=-O3 -Wall -Wno-deprecated-declarations
# # LIBS=-framework GLUT -framework OpenGL
# # #  Linux/Unix/Solaris
# # else
# # CFLG=-O3 -Wall
# # LIBS=-lglut -lGLU -lGL -lm
# # endif
# # #  OSX/Linux/Unix/Solaris
# # CLEAN=rm -f $(EXE) *.o *.a
# # endif

# # # Dependencies
# # hw6.o: hw6.c CSCIx229.h
# # fatal.o: fatal.c CSCIx229.h
# # errcheck.o: errcheck.c CSCIx229.h
# # print.o: print.c CSCIx229.h
# # loadtexbmp.o: loadtexbmp.c CSCIx229.h
# # loadobj.o: loadobj.c CSCIx229.h
# # projection.o: projection.c CSCIx229.h

# # #  Create archive
# # CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
# # 	ar -rcs $@ $^

# # # Compile rules
# # .c.o:
# # 	gcc -c $(CFLG)  $<
# # .cpp.o:
# # 	g++ -c $(CFLG)  $<

# # #  Link
# # hw6:hw6.o   CSCIx229.a
# # 	gcc $(CFLG) -o $@ $^  $(LIBS)

# # #  Clean
# # clean:
# # 	$(CLEAN)




# # Example 13
# EXE=L

# # Main target
# all: $(EXE)

# #  Msys/MinGW
# ifeq "$(OS)" "Windows_NT"
# CFLG=-O3 -Wall -DUSEGLEW
# LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm
# CLEAN=rm -f *.exe *.o *.a
# else
# #  OSX
# ifeq "$(shell uname)" "Darwin"
# CFLG=-O3 -Wall -Wno-deprecated-declarations
# LIBS=-framework GLUT -framework OpenGL
# #  Linux/Unix/Solaris
# else
# CFLG=-O3 -Wall
# LIBS=-lglut -lGLU -lGL -lm
# endif
# #  OSX/Linux/Unix/Solaris
# CLEAN=rm -f $(EXE) *.o *.a
# endif

# # Dependencies
# LunarOutpost.o: LunarOutpost.c CSCIx229.h
# fatal.o: fatal.c CSCIx229.h
# errcheck.o: errcheck.c CSCIx229.h
# print.o: print.c CSCIx229.h
# loadtexbmp.o: loadtexbmp.c CSCIx229.h
# loadobj.o: loadobj.c CSCIx229.h
# projection.o: projection.c CSCIx229.h

# Objects/Floatingcrystalshards.o: Objects/Floatingcrystalshards.cpp CSCIx229.h

# #  Create archive
# CSCIx229.a:fatal.o errcheck.o print.o loadtexbmp.o loadobj.o projection.o
# 	ar -rcs $@ $^

# # Compile rules
# .c.o:
# 	gcc -c $(CFLG)  $<
# .cpp.o:
# 	g++ -c $(CFLG)  $<

# #  Link
# L: LunarOutpost.o   CSCIx229.a
# 	gcc $(CFLG) -o $@ $^  $(LIBS)

# #  Clean
# clean:
# 	$(CLEAN)




# Example 13
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
Floatingcrystalshards.o: Floatingcrystalshards.c CSCIx229.h
AlienScoutDrone.o: AlienScoutDrone.c CSCIx229.h
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

#  Link - IMPORTANT: Added Floatingcrystalshards.o here
L: LunarOutpost.o Floatingcrystalshards.o AlienScoutDrone.o CSCIx229.a
	gcc $(CFLG) -o $@ $^  $(LIBS)
	

#  Clean
clean:
	$(CLEAN)