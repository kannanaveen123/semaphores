#include <iostream>
#include <fstream>
#include <thread>
#include <semaphore.h>
#include <vector>
#include <iomanip>
#include <random>
#include <chrono>
#include <ctime>
#include <sstream>
#include <cmath>

using namespace std;

int n_readers;
int n_writers;
int write_counter;
int read_counter;
int kw, kr;
double mu_CS, mu_Rem;
sem_t log_file_lock, write_mutex, read_mutex, resource, read_try;
ofstream log_file;

string getTimeFormatted()
{
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::microseconds>(now);
    auto value = now_ms.time_since_epoch();
    int microseconds = static_cast<int>(chrono::duration_cast<chrono::microseconds>(value).count() % 1000000);

    auto now_c = chrono::system_clock::to_time_t(now);
    struct tm *timeinfo = localtime(&now_c);
    stringstream ss;
    ss << setw(2) << setfill('0') << timeinfo->tm_hour << ":" << setw(2) << setfill('0') << timeinfo->tm_min << ":" << setw(2) << setfill('0') << timeinfo->tm_sec << "." << setw(6) << setfill('0') << microseconds;
    return ss.str();
}

double getSystemTime()
{
    auto now = chrono::system_clock::now();
    auto now_c = chrono::system_clock::to_time_t(now);
    return now_c;
}

double exponentialDelay(double mu)
{
    double u;
    do
    {
        u = ((double)rand() / RAND_MAX);
    } while (u == 0);
    return -log(u) * mu;
}

void initializeSemaphores()
{
    sem_init(&log_file_lock, 0, 1);
    sem_init(&write_mutex, 0, 1);
    sem_init(&read_mutex, 0, 1);
    sem_init(&resource, 0, 1);
    sem_init(&read_try, 0, 1);
}

void writerFunc(int id, vector<double> &times, double &totalTime)
{
    id = hash<thread::id>{}(this_thread::get_id());
    for (int i = 0; i < kw; i++)
    {
        double reqTime = getSystemTime();

        sem_wait(&log_file_lock);
        auto reqTimestamp = getTimeFormatted();
        auto reqTimeMicro = chrono::system_clock::now();
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS request by Writer Thread " << id << " at " << reqTimestamp << "." << setw(6) << setfill('0') << chrono::duration_cast<chrono::microseconds>(reqTimeMicro.time_since_epoch()).count() % 1000000 << endl;

        log_file << flush;
        sem_post(&log_file_lock);

        sem_wait(&write_mutex);
        write_counter++;
        if (write_counter == 1)
            sem_wait(&read_try);
        sem_post(&write_mutex);
        sem_wait(&resource);

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS entry by Writer Thread " << id << " at " << getTimeFormatted() << endl;
        log_file << flush;
        sem_post(&log_file_lock);

        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_CS))); // Simulate a thread writing in CS

        // Exit section
        sem_post(&resource); // Release resource
        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS exit by Writer Thread " << id << " at " << getTimeFormatted() << endl;
        log_file << flush;
        sem_post(&log_file_lock);

        sem_wait(&write_mutex);  // Reserve exit section
        write_counter--;         // Indicate you're leaving
        if (write_counter == 0)  // Check if you're the last writer
            sem_post(&read_try); // If you're the last, unlock the readers
        sem_post(&write_mutex);  // Release exit section

        double exitTime = getSystemTime();
        double timeInCS = exitTime - reqTime;
        times.push_back(timeInCS);
        totalTime += timeInCS;

        // Remainder section
        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_Rem))); // Simulate a thread executing in Remainder Section
    }
}

void readerFunc(int id, vector<double> &times, double &totalTime)
{
    id = hash<thread::id>{}(this_thread::get_id());
    for (int i = 0; i < kr; i++)
    {
        double reqTime = getSystemTime();

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS request by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file << flush;
        sem_post(&log_file_lock);

        sem_wait(&read_try);     // Indicate a reader is trying to enter
        sem_wait(&read_mutex);   // Lock entry section to avoid race condition with other readers
        read_counter++;          // Report yourself as a reader
        if (read_counter == 1)   // Check if you're the first reader
            sem_wait(&resource); // If you're the first reader, lock the resource
        sem_post(&read_mutex);   // Release entry section for other readers
        sem_post(&read_try);     // Indicate you're done trying to access the resource

        sem_wait(&log_file_lock);

        double entryTime = getSystemTime();
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS entry by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file << flush;
        sem_post(&log_file_lock);

        // Reading operation performed
        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_CS))); // Simulate a thread reading from CS

        // Exit section
        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd"
                                            : (i == 2)   ? "rd"
                                                         : "th")
                 << " CS exit by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file << flush;
        sem_post(&log_file_lock);

        sem_wait(&read_mutex);   // Reserve exit section - avoids race condition with readers
        read_counter--;          // Indicate you're leaving
        if (read_counter == 0)   // Check if you're the last reader leaving
            sem_post(&resource); // If you're the last, release the locked resource
        sem_post(&read_mutex);   // Release exit section for other readers

        double exitTime = getSystemTime();
        double timeInCS = exitTime - reqTime;
        times.push_back(timeInCS);
        totalTime += timeInCS;

        // Remainder section
        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_Rem))); // Simulate a thread executing in Remainder Section
    }
}

int main()
{
    srand(time(NULL));

    ifstream input("inp-params.txt");
    input >> n_writers >> n_readers >> kw >> kr >> mu_CS >> mu_Rem;
    input.close();

    log_file.open("RW-log.txt");

    initializeSemaphores();

    vector<double> rw_times;
    vector<thread> rw_threads;
    double totalWriterTime = 0.0, totalReaderTime = 0.0;

    for (int i = 1; i <= n_writers; ++i)
    {
        rw_threads.emplace_back(writerFunc, i, ref(rw_times), ref(totalWriterTime));
    }

    for (int i = 1; i <= n_readers; ++i)
    {
        rw_threads.emplace_back(readerFunc, i, ref(rw_times), ref(totalReaderTime));
    }

    for (auto &t : rw_threads)
    {
        t.join();
    }

    log_file.flush();
    log_file.close();

    // Calculate average time for both readers and writers
    double avgWriterTime = totalWriterTime / (n_writers * kw);
    double avgReaderTime = totalReaderTime / (n_readers * kr);

    ofstream avg_file("AverageTime.txt");
    avg_file << "Average time for writers to gain entry to CS: " << avgWriterTime << " seconds." << endl;
    avg_file << "Average time for readers to gain entry to CS: " << avgReaderTime << " seconds." << endl;
    avg_file.close();

    return 0;
}
