#include <stdio.h>
#include <stdlib.h>
#define MAX_DURATION 9999999

// Create structs Process and ganttChart
typedef struct{
    int id;
    int arrivalTime;
    int duration;
    int priority;
    int turnTime;
    int waitTime;
}Process;

typedef struct{
  int id;
  int arrivalTime;
  int endTime;
}ganttChart;

// FCFS 
void FCFS(Process processes[], int);
void ganttFCFS(Process*, ganttChart*, int*);
void sortFCFS(Process[], int);

// SJF
void SJF(Process processes[], int);
void ganttSJF(Process*, ganttChart*, int);
Process* lowestBurst(Process[], Process[], int, int, Process*);

// PRIO 
void PRIO(Process processes[], int val);
Process* lowestPrio(Process processes[], Process curr[], int, int, Process*);
void ganttPrio(Process*, ganttChart*, int);

// MISC
void displayData(Process processes[], int val, ganttChart Rows[], int counter, char* filename);
int findArrivalTime(Process[], Process*, int);

int main(int argc, char **argv) {

    // check for valid amount of arguements 
    int totalProcesses = 0;
    	if (argc != 2) {
       		printf("Incorrect val of arguments. Useage ./Scheduler $inputfile \n");
       	 	return -1;
    	}
    // scan until end of file for chars and if newline then add to total processes
    FILE* file = fopen(argv[1], "r");
    while(!feof(file)) {
        char nextChar = fgetc(file);
        if (nextChar == '\n') {
            totalProcesses++;
        }
    }

    Process processes[totalProcesses];
    fseek(file, 0, SEEK_SET);
    totalProcesses = 0;

    // keep adding processes to file 
    while (fscanf(file, "%d,%d,%d,%d",&processes[totalProcesses].id,&processes[totalProcesses].arrivalTime,&processes[totalProcesses].duration,&processes[totalProcesses].priority) != EOF) {
        totalProcesses++;
    }

    // close the file! 
    fclose(file);
    FCFS(processes, totalProcesses);
    SJF(processes, totalProcesses);
    PRIO(processes, totalProcesses);
    return 0;
}
// Display data on a new file based on what algorithm to send to display 
void displayData(Process processes[], int val, ganttChart row[], int counter, char *filename) {

    float averageWait = 0;
    float averageTurnAround = 0;
    FILE* file;
    file = fopen(filename, "w");
    fprintf(file,"\t\t\tPROCESS ID    ARRIVAL TIME    END TIME\n");

    for (int i = 0; i < counter; i++)
        fprintf(file,"%20d%20d%20d\n", row[i].id, row[i].arrivalTime, row[i].endTime);


    for (int i = 0; i < val; i++){
          averageWait += processes[i].waitTime;
          averageTurnAround += processes[i].turnTime;
    }
    // print statements for turn around timel, wait time, and throughput 
    fprintf(file,"Average TurnAround Time=%f\n", averageTurnAround / val);
    fprintf(file,"Average Wait Time=%f\n", averageWait / val);
    fprintf(file,"Throughput=%f\n",((float)val / row[counter - 1].endTime));
    fclose(file);

}

// FCFS Algorithm
void FCFS(Process processes[], int val) {
    ganttChart gchart[200];
    int gindex = 0;
    Process curr[val];
    int last = 0;

    // initialize current processes through assigning a new processes variables
    for (int i = 0 ; i < val ; i++) {
        curr[i].waitTime = 0;
        curr[i].turnTime = 0;
        curr[i] = processes[i];
        curr[i].id = processes[i].id;
        curr[i].arrivalTime = processes[i].arrivalTime;
        curr[i].duration = processes[i].duration;
    }
    // send process to gantt chart and increase amount of total processes in gantt chart with index
    sortFCFS(curr, val);
    for (int i = 0 ; i < val ; i++) {
        ganttFCFS(&(curr[i]), &gchart[gindex], &last);
        gindex++;
    }
    // send to display 
    displayData(curr, val, gchart, val, "FCFS");

}

// FCFS Gantt Chart
// Assign gantt chart with processes and fill variables accordingly 
void ganttFCFS(Process* process, ganttChart* gantt, int* last){
    gantt->id = process->id;
    gantt->arrivalTime = *last;
    gantt->endTime = gantt->arrivalTime + process->duration;
    process->waitTime = *last - process->arrivalTime;
    process->turnTime = process->waitTime + process->duration;
    *last = gantt->endTime;
}

// FCFS Sorting Algorithm
void sortFCFS (Process processes[], int val) {
    int counter = 0;
    while (counter < val) {
        int current = counter;
        for (int i = counter ; i < val; i++) {
            if (processes[i].arrivalTime < processes[current].arrivalTime) {
                current = i;
            }

            if (processes[i].arrivalTime == processes[current].arrivalTime) {
                if (processes[i].id < processes[current].id) {
                    current = i;
                }
            }
        }
        // swap processes after finding new low
        Process currentLowest = processes[counter];
        processes[counter] = processes[current];
        processes[current] = currentLowest;
        counter++;
    }
}

// SJF Algorithm
void SJF(Process processes[], int val) {
    ganttChart gchart[200];
    int gindex = 0;
    Process curr[val];
    int totalTime = 0;

    // initalize temp processes
    Process* lowestProcess;
    Process currentLowest;
    Process* currentProcess;
    currentLowest.id = 0;
    currentLowest.duration = MAX_DURATION;
    currentLowest.priority = MAX_DURATION;
    currentProcess = &currentLowest;

    // initialize current processes through assigning a new processes variables
    for (int i = 0 ; i < val ; i++) {
        curr[i].waitTime = 0;
        curr[i].turnTime = 0;
        totalTime += processes[i].duration;
        curr[i].id = processes[i].id;
        curr[i].arrivalTime = processes[i].arrivalTime;
        curr[i].duration = processes[i].duration;
    }

    // compare processes and find lowest burst. if id matches, then add to gantt chart and modify wait time and arrival time with new values 
    // if not, check if id is greater than 0 or not null and send to gantt chart and assign current process arrival time before 
    // it is swapped with lowest process, then change arrival time. 
    for (int time = 0; time <= totalTime; time++) {
        lowestProcess = currentProcess;
        currentProcess->duration--;
        lowestProcess = lowestBurst(processes, curr, time, val, lowestProcess);
        if ((lowestProcess->id) == (currentProcess->id)) {
            if (currentProcess->duration == 0){
                ganttSJF(currentProcess, &gchart[gindex], time);
                gindex++;
                currentProcess = lowestBurst(processes, curr, time, val, &currentLowest);
                currentProcess->waitTime += time - currentProcess->arrivalTime;
                currentProcess->arrivalTime = time;
            }
        } 
        else {
            if ((currentProcess->id) > 0) {
                ganttSJF(currentProcess, &gchart[gindex], time);
                gindex++;
                currentProcess->arrivalTime = time;
            }
            currentProcess = lowestProcess;
            currentProcess->arrivalTime = time;
        }
    }
    displayData(curr, val, gchart, gindex, "SJFP");
}

// algorithm to find lowest burst duration. Compares all processes by arrival time and if same, find new lowest and assign, else if greater just assign new lowest 
Process* lowestBurst(Process processes[], Process curr[], int time, int val, Process* lowestProcess) {
    int counter = 0;
    while (counter < val) {
        if ((processes[counter].arrivalTime <= time) && (curr[counter].duration > 0)) {
            if (curr[counter].duration == lowestProcess->duration) {
                int currentLowest = findArrivalTime(processes, lowestProcess, val);
                if (processes[counter].arrivalTime < currentLowest) {
                    lowestProcess = &curr[counter];
                }
            }
            else if (lowestProcess->duration > curr[counter].duration ) {
                lowestProcess = &curr[counter];
            }
        }
        counter++;
    }
    return lowestProcess;
}
// cimpare id's of processes and return the arrival time of the process if match then return given process, else increase counter and find. 
int findArrivalTime(Process processes[], Process* curr, int val) {
    int counter = 0;
    while (val > counter) {
        if (processes[counter].id == curr->id) {
            return processes[counter].arrivalTime;
        }
        else {
            counter++;
        }
    }
    return 0;
}
// SJF gantt table. Slightly different than FCFS as it requires less information due to SJF vs FCFS. 
void ganttSJF(Process* process, ganttChart* gantt, int time) {
    process->turnTime = time - process->arrivalTime + process->waitTime;
    gantt->endTime = time;
    gantt->id = process->id;
    gantt->arrivalTime = process->arrivalTime;
}

// PRIORITY SCHEDULING Algorithm
void PRIO(Process processes[], int val) {
    ganttChart gchart[200];
    int gindex = 0;
    Process curr[val];
    int totalTime = 0;

    Process* lowestProcess;
    Process currentLowest;
    Process* currentProcess;
    currentLowest.id = 0;
    currentLowest.duration = MAX_DURATION;
    currentLowest.priority = MAX_DURATION;
    currentProcess = &currentLowest;

    // initialize current processes through assigning a new processes variables, exact same as SJF but with priority
    for (int i = 0 ; i < val ; i++) {
        curr[i].waitTime = 0;
        curr[i].turnTime = 0;
        totalTime += processes[i].duration;
        curr[i].id = processes[i].id;
        curr[i].arrivalTime = processes[i].arrivalTime;
        curr[i].duration = processes[i].duration;
        curr[i].priority = processes[i].priority;
    }

    // compare processes and find priority. if id matches, then add to gantt chart and modify wait time and arrival time with new values 
    // if not, check if id is greater than 0 or not null and send to gantt chart and assign current process arrival time before 
    // it is swapped with lowest process, then change arrival time. 
    for (int time = 0; time <= totalTime; time++) {
        lowestProcess = currentProcess;
        currentProcess->duration--;
        lowestProcess = lowestPrio(processes, curr, time, val, lowestProcess);
        if ((lowestProcess->id) == (currentProcess->id)) {
            if (currentProcess->duration == 0){
                ganttPrio(currentProcess, &gchart[gindex], time);
                gindex++;
                currentProcess = lowestPrio(processes, curr, time, val, &currentLowest);
                currentProcess->waitTime += time - currentProcess->arrivalTime;
                currentProcess->arrivalTime = time;
            }
        } 
        else {
            if ((currentProcess->id) > 0) {
                ganttPrio(currentProcess, &gchart[gindex], time);
                gindex++;
                currentProcess->arrivalTime = time;
            }
            currentProcess = lowestProcess;
            currentProcess->arrivalTime = time;
        }
    }
    displayData(curr, val, gchart, gindex, "PRIO");
}

// find lowest priority out of processes. Compare and check, if priority greater, then assign else set new based on time. Similar to finding burst duration for SJF
Process* lowestPrio(Process processes[], Process curr[], int time, int val, Process* lowestProcess) {
    int counter = 0; 
    while (counter < val){
        if ((processes[counter].arrivalTime) <= time && (curr[counter].duration >0)) {
            if (curr[counter].priority < lowestProcess->priority) {
                lowestProcess = &curr[counter];
            }
            else if (curr[counter].priority == lowestProcess->priority) {
                int currentLowest = findArrivalTime(processes, lowestProcess, val);
                if (processes[counter].arrivalTime < currentLowest) {
                    lowestProcess = &curr[counter];
                }
            }
        }
        counter++;
    }
    return lowestProcess;
}

// Prio gantt chart, identical to SJF as they use similar variables vs FCFS
void ganttPrio(Process* process, ganttChart* gantt, int time) {
    gantt->id = process->id;
    gantt->endTime = time;
    gantt->arrivalTime = process->arrivalTime;
    process->turnTime = time - process->arrivalTime + process->waitTime;
}