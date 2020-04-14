# Page_Replacement_Simulation  
A simulation of FIFO, Optimum and, Aging page replacement algorithms in C.  

# Overview:  
The three algorithms in this project are:  
Opt – Simulates what the optimal page replacement algorithm would choose if it had perfect
knowledge.    
FIFO – Simulates first-in, first-out.    
Aging – Simulates the aging algorithm that approximates LRU with an 8-bit counter. Does a
refresh of the R bits every P CPU cycles.  

This program implements a page table for a 32-bit address space with pages 4KB in size. The number of frames will be a parameter to the execution of the program.  

# How to run:  
Compile vmsim.c and enter the following command line:  
./vmsim –n <numframes> -a <opt|fifo|aging> [-r <refresh>] <tracefile>  
The program will then run through the memory references of the file and decide the action taken for
each address (hit, page fault – no eviction, page fault – evict clean, page fault – evict dirty).
When the trace is over, it prints out summary statistics in the following format:  
Algorithm: FIFO  
Number of frames: 8  
Total memory accesses: %d  
Total page faults: %d  
Total writes to disk: %d  

# Trace files:  
The simulator takes a command-line argument that specifies the trace file that will be used to compute
the output statistics. The trace file will specify all the data memory accesses that occur in the sample program. Each line in the trace file will specify a new memory reference. Each line in the trace will therefore have the following three fields:  
Access Type: A single character indicating whether the access is a load ('l') or a store ('s').  
Address: A 32-bit integer (in unsigned hexadecimal format) specifying the memory address that is being accessed. For example, "0xff32e100" specifies that memory address 4281524480 is accessed.   
CPU cycles since last memory access: Indicates the number of CPU cycles that elapsed since the
last memory access (i.e., the one on the previous line in the trace). For example, if the 5th and
10th cycles in the program's execution are loads, and there are no memory operations between
them, then the trace line for the second load has "4" for this field. For the first line in the trace
file, the CPU cycles number indicates the cycle number at which the access occurs.  
Fields on the same line are separated by a single space. Example trace lines look like:  
l 0x300088a0 6
s 0x1ffffd00 0  

swim.trace is provided.
