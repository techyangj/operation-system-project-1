Operation System Project 1 — Process Scheduling (FCFS / SJF)

A small C project for an Operating Systems course that implements two classic CPU scheduling algorithms: FCFS (First-Come, First-Served) and SJF (Shortest Job First).

Designed for Linux. Tasks/processes can be read from standard input or from a specified file.

⸻

Requirements
	•	Linux (tested on common distributions)
	•	gcc (7.0+ recommended)

⸻

Build

# In the project root
gcc operation_system.c -o operation_system

If your environment is older or shows warnings, adjust the compile flags as needed.

⸻

Usage

./operation_system [options]

Options:
  -a <both|fcfs|sjf>   Algorithm to run (default: both)
  -f <file>            Read task/process data from file
  -h                   Show help

Examples

./operation_system -h              # Show help
./operation_system                 # Run both algorithms (default)
./operation_system -a both         # Explicitly run both algorithms
./operation_system -a fcfs         # Run FCFS only
./operation_system -a sjf          # Run SJF only
./operation_system -f data.txt     # Read input from data.txt


⸻

Input File Format (Recommended)

Your program will parse files according to the logic in operation_system.c. If you haven’t fixed a format yet, the following is a simple option:
	•	One task per line; fields separated by whitespace
	•	Fields: PID  Arrival_Time  Burst_Time  Priority

Example (for reference; follow your actual parser):

# PID  Arrival_Time  Burst_Time  Priority
1    0             5           2
2    2             3           1
3    4             2           3


⸻

Program Output

Depending on the selected algorithm, the program typically prints:
	•	The schedule order / a textual Gantt chart
	•	Each process’s waiting time and turnaround time
	•	Average waiting time and average turnaround time


# Example:
❯ ./operation_system -a fcfs
Read 3 processes from processes.txt
PID	Arrival	Burst	Priority
1	0	5	2
2	2	3	1
3	4	2	3

----- FCFS -----
processing sequence:
P1 -> P2 -> P3
PID	Arr	Burst	WT	TAT	C
1	0	5	0	5	5
2	2	3	3	6	8
3	4	2	4	6	10
Avg WT=2.33  Avg TAT=5.67

Gantt chart:
| P1 | P2 | P3 | 
0   5   8   10



