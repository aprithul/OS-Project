sim_1: Simulator.o MetaData.o LinkedList.o Utils.o Logger.o Configuration.o Timer.o IO.o Memory.o Resource.o
	gcc Simulator.o MetaData.o LinkedList.o Utils.o Logger.o Configuration.o Timer.o IO.o Memory.o Resource.o -pthread -lpthread -o sim_1

Simulator.o: Simulator.c
	gcc -c Simulator.c

MetaData.o: MetaData.c MetaData.h
	gcc -c MetaData.c

LinkedList.o: LinkedList.c LinkedList.h
	gcc -c LinkedList.c

Utils.o: Utils.c Utils.h
	gcc -c Utils.c

Logger.o: Logger.c Logger.h
	gcc -c Logger.c

Configuration.o: Configuration.c Configuration.h
	gcc -c Configuration.c

Timer.o: Timer.c Timer.h
	gcc -c Timer.c

IO.o: IO.c IO.h
	gcc -c IO.c

Memory.o: Memory.c Memory.h
	gcc -c Memory.c

Resource.o: Resource.c Resource.h
	gcc -c Resource.c

clean:
	rm *.o sim_1
