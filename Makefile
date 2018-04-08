CXX = g++
CXX_STD = -std=c++14 -O2 -Wall -Wextra -Werror

GSL_CFLAGS = -I/usr/include
GSL_LFLAGS = -L/usr/lib -lgsl -lgslcblas -lm

CXX_CFLAGS = ${CXX_STD} ${GSL_CFLAGS}
CXX_LFLAGS = ${GSL_LFLAGS}
#============================================================

integration : integration_c
	@echo "---- Linking $< -----"
	${CXX} -w $<.o ${CXX_LFLAGS} -o $@.out
	-${RM} $<.o
	@echo "==============="

integration_c : integration/example.cpp
	@echo "--- Compiling $< ---"
	${CXX} -c $< ${CXX_CFLAGS} -o $@.o


rootfinding : rootfinding_c
	@echo "---- Linking $< -----"
	${CXX} -w $<.o ${CXX_LFLAGS} -o $@.out
	-${RM} $<.o
	@echo "==============="

rootfinding_c : rootfinding/example.cpp
	@echo "--- Compiling $< ---"
	${CXX} -c $< ${CXX_CFLAGS} -o $@.o


interpolation : interpolation_c
	@echo "---- Linking $< -----"
	${CXX} -w $<.o ${CXX_LFLAGS} -o $@.out
	-${RM} $<.o
	@echo "==============="

interpolation_c : interpolation/example.cpp
	@echo "--- Compiling $< ---"
	${CXX} -c $< ${CXX_CFLAGS} -o $@.o


timestepping : timestepping_c
	@echo "---- Linking $< -----"
	${CXX} -w $<.o ${CXX_LFLAGS} -o $@.out
	-${RM} $<.o
	@echo "==============="

timestepping_c : timestepping/example.cpp
	@echo "--- Compiling $< ---"
	${CXX} -c $< ${CXX_CFLAGS} -o $@.o

######

clean:
	-@${RM} *.out *.gch *.o *.out
