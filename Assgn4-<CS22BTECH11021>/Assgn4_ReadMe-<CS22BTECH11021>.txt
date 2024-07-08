                                                                            README
                                                                            ESLAVATH NAVEEN NAIK
                                                                            CS22BTECH11021
                                                                           
 --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                                                                     
 1st code (RW):
 How to Execute the Program
File Structure

Ensure that the following files are present in the directory where you intend to execute the program:

    rw-CS22BTECH11021.cpp - The C++ source code file provided.
    inp-params.txt - Input parameters file containing values for n_writers, n_readers, kw, kr, mu_CS, and mu_Rem.
    readme.txt - Instructions file for executing the program.

Compilation

To compile the program, use the following command in the terminal:

g++ rw-CS22BTECH11021.cpp 
This command assumes you have g++ installed and configured on your system.
Execution

After compilation, execute the program with the following command:
./a.out
Customizing Input Parameters

If you want to customize the input parameters:

    Open the inp-params.txt file.
    Modify the values of n_writers, n_readers, kw, kr, mu_CS, and mu_Rem as desired. Each parameter should be on a separate line.
    Save and close the file.

Output

Upon successful execution, the program generates the following output files:

    RW-log.txt - Log file containing the execution details.
    AverageTime.txt - File containing average times for writers and readers to gain entry to the critical section.

Note

    Ensure that you have necessary permissions to read from and write to files in the directory where the program is executed.
    The program uses random number generation, but the seed is based on the system time. If you need reproducible results for testing purposes, consider using a fixed seed.
    Make sure you have pthread library installed on your system to link during compilation (-lpthread).
    
    
EXAMPLES OF INPUT AND OUTPUT:

Example Input (inp-params.txt):
3 2 4 3 500 300


OUTPUT OF Rw-log.txt
1st CS request by Writer Thread 123456789 at 10:00:05.123456.
1st CS entry by Writer Thread 123456789 at 10:00:05.234567.
1st CS exit by Writer Thread 123456789 at 10:00:05.734567.
1st CS request by Reader Thread 987654321 at 10:00:06.345678.
1st CS entry by Reader Thread 987654321 at 10:00:06.456789.
1st CS exit by Reader Thread 987654321 at 10:00:06.756789.
2nd CS request by Writer Thread 123456789 at 10:00:07.123456.
2nd CS entry by Writer Thread 123456789 at 10:00:07.234567.
2nd CS exit by Writer Thread 123456789 at 10:00:07.734567.
...
AverageTime.txt

Average time for writers to gain entry to CS: 0.500 seconds.
Average time for readers to gain entry to CS: 0.300 seconds.
 -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 Readme for 2nd code (Fair):
                                                                           Title: Fair Readers-Writers Problem

Description:
This program simulates the Fair Readers-Writers Problem using a multithreaded approach. The Fair Readers-Writers Problem is a classical synchronization problem in computer science where multiple threads compete for access to a shared resource, with constraints ensuring fairness in granting access to readers and writers.

File Structure:

    frw-<CS22BTECH11021>.cpp: Contains the C++ code implementing the Fair Readers-Writers Problem solution.
    inp-params.txt: Input file containing parameters for the simulation (number of readers, writers, iterations, and delay times).
    Fair-log.txt: Output log file capturing the execution details of each thread.
    README.txt: Instructions on how to execute the program.
    
   


Instructions for Execution:

    Compile the Program:
        Open a terminal.
        Navigate to the directory containing the frw-<CS22BTECH11021>.cpp file.
       (i) g++ frw-<CS22BTECH11021>.cpp 
       (ii)  ./a.out

        Prepare Input Parameters:

    Ensure inp-params.txt is present and correctly formatted.
    Modify inp-params.txt if necessary, providing the following parameters in the order specified:
        Number of writers
        Number of readers
        Number of iterations for each writer (kw)
        Number of iterations for each reader (kr)
        Mean delay time for Critical Section (mu_CS)
        Mean delay time for Remainder Section (mu_Rem)
        
        
        Execute the Program:

    Run the compiled executable with ./fair_rw.
        
        
        ./fair_rw

View Output:

    Once the program execution completes, view the output in Fair-log.txt.
    Additionally, the program outputs the average entry time for writers and readers to the console.

Clean-Up:

    After execution, you may delete the log file (Fair-log.txt) if it's no longer needed.
    


Example Input (inp-params.txt):
3 2 4 3 500 300

Example Output (Fair-log.txt):
1st CS request by Writer Thread 1 at 10:00:05.123456
1st CS entry by Writer Thread 1 at 10:00:05.123457
1st CS exit by Writer Thread 1 at 10:00:05.623458
2nd CS request by Writer Thread 1 at 10:00:05.623459
2nd CS entry by Writer Thread 1 at 10:00:06.123460
1st CS request by Reader Thread 1 at 10:00:06.123461
2nd CS exit by Writer Thread 1 at 10:00:06.623462
1st CS entry by Reader Thread 1 at 10:00:06.623463
1st CS exit by Reader Thread 1 at 10:00:07.123464
2nd CS request by Writer Thread 1 at 10:00:07.123465
2nd CS entry by Writer Thread 1 at 10:00:07.623466
1st CS request by Writer Thread 2 at 10:00:07.623467
2nd CS exit by Writer Thread 1 at 10:00:08.123468
1st CS entry by Writer Thread 2 at 10:00:08.123469
...
Example of Average output:
Average entry time for writers: 500.345 milliseconds.
Average entry time for readers: 400.765 milliseconds.

    
    --------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
