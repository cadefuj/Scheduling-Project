CC=gcc

all: Scheduler.c
	$(CXX) -o Scheduler Scheduler.c
		
clean:
	rm -f Scheduler *.o *.zip
	
package:
	zip Fujitai-Cade-HW5.zip README.txt Makefile Scheduler.c Input.csv
