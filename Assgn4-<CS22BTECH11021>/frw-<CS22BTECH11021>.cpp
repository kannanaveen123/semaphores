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
int kw, kr;
double mu_CS, mu_Rem;
int read_count = 0;
sem_t log_file_lock, write_mutex, read_mutex, resource, read_try;
ofstream log_file;

string getTimeFormatted() {
    auto now = chrono::system_clock::now();
    auto now_ms = chrono::time_point_cast<chrono::microseconds>(now);
    auto value = now_ms.time_since_epoch();
    int microseconds = static_cast<int>(chrono::duration_cast<chrono::microseconds>(value).count() % 1000000);

    auto now_c = chrono::system_clock::to_time_t(now);
    struct tm* timeinfo = localtime(&now_c);
    stringstream ss;
    ss << setw(2) << setfill('0') << timeinfo->tm_hour << ":" << setw(2) << setfill('0') << timeinfo->tm_min << ":" << setw(2) << setfill('0') << timeinfo->tm_sec << "." << setw(6) << setfill('0') << microseconds;
    return ss.str();
}

double getSystemTime() {
    auto now = chrono::system_clock::now();
    auto now_c = chrono::system_clock::to_time_t(now);
    return now_c;
}

double exponentialDelay(double mu) {
    double u;
    do {
        u = ((double)rand() / RAND_MAX);
    } while (u == 0);
    return -log(u) * mu;
}

void initializeSemaphores() {
    sem_init(&log_file_lock, 0, 1);
    sem_init(&write_mutex, 0, 1);
    sem_init(&read_mutex, 0, 1);
    sem_init(&resource, 0, 1);
    sem_init(&read_try, 0, 1);
}

void writerFunc(int id, vector<double>& writer_times, double& total_writer_time) {
    for (int i = 0; i < kw; i++) {
        double reqTime = getSystemTime();

        sem_wait(&log_file_lock);
        auto reqTimestamp = getTimeFormatted();
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS request by Writer Thread " << id << " at " << reqTimestamp << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        sem_wait(&write_mutex);
        sem_wait(&resource); // Acquire resource semaphore directly
        sem_post(&write_mutex);

        double entryTime = getSystemTime();

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS entry by Writer Thread " << id << " at " << getTimeFormatted() << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_CS)));

        sem_post(&resource); // Release resource semaphore

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS exit by Writer Thread " << id << " at " << getTimeFormatted() << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        double exitTime = getSystemTime();
        writer_times.push_back(exitTime - reqTime);
        total_writer_time += exitTime - reqTime;

        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_Rem)));
    }
}

void readerFunc(int id, vector<double>& reader_times, double& total_reader_time) {
    for (int i = 0; i < kr; i++) {
        double reqTime = getSystemTime();

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS request by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        sem_wait(&read_mutex); // Acquire read mutex to update read_count
        read_count++;
        if (read_count == 1) {
            sem_wait(&resource); // If first reader, acquire resource
        }
        sem_post(&read_mutex);

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS entry by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        // Reading operation performed
        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_CS))); // Simulate a thread reading from CS

        sem_wait(&read_mutex); // Acquire read mutex to update read_count
        read_count--;
        if (read_count == 0) {
            sem_post(&resource); // If last reader, release resource
        }
        sem_post(&read_mutex);

        double exitTime = getSystemTime();

        sem_wait(&log_file_lock);
        log_file << i + 1 << ((i == 0) ? "st" : (i == 1) ? "nd" : (i == 2) ? "rd" : "th")
                 << " CS exit by Reader Thread " << id << " at " << getTimeFormatted() << endl;
        log_file.flush();
        sem_post(&log_file_lock);

        reader_times.push_back(exitTime - reqTime);
        total_reader_time += exitTime - reqTime;

        // Remainder section
        this_thread::sleep_for(chrono::milliseconds((int)exponentialDelay(mu_Rem))); // Simulate a thread executing in Remainder Section
    }
}

int main() {
    srand(time(NULL));

    ifstream input("inp-params.txt");
    if (!input.is_open()) {
        cerr << "Error: Unable to open input file." << endl;
        return 1;
    }
    input >> n_writers >> n_readers >> kw >> kr >> mu_CS >> mu_Rem;
    input.close();

    log_file.open("Fair-log.txt");
    if (!log_file.is_open()) {
        cerr << "Error: Unable to open output file." << endl;
        return 1;
    }

    initializeSemaphores();

    vector<double> writer_times;
    vector<double> reader_times;
    double total_writer_time = 0.0;
    double total_reader_time = 0.0;

    vector<thread> threads;

    for (int i = 1; i <= n_writers; ++i) {
        threads.emplace_back(writerFunc, i, ref(writer_times), ref(total_writer_time));
    }

    for (int i = 1; i <= n_readers; ++i) {
        threads.emplace_back(readerFunc, i, ref(reader_times), ref(total_reader_time));
    }

    for (auto& t : threads) {
        t.join();
    }

    log_file.flush();
    log_file.close();

    // Calculate the average entry time for writers
    double avg_writer_time = total_writer_time / (n_writers * kw);
    cout << "Average entry time for writers: " << avg_writer_time * 1000 << " milliseconds." << endl;

    // Calculate the average entry time for readers
    double avg_reader_time = total_reader_time / (n_readers * kr);
    cout << "Average entry time for readers: " << avg_reader_time * 1000 << " milliseconds." << endl;

    return 0;
}
