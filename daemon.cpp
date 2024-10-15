#include <iostream>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <csignal>
#include <chrono>

// Constants
const int SHARED_MEMORY_SIZE = 4096;
char* SHARED_MEMORY_NAME = "/shm_"; 

struct SharedMemoryData {
    int offset;
    char buffer[SHARED_MEMORY_SIZE - sizeof(int)];
};

// Shared resources
SharedMemoryData* shared_memory_data = nullptr;
int shared_memory_fd = -1;
std::ofstream log_file;

// Error checking utility
void handleError(bool condition, const char* error_message) {
    if (condition) {
        perror(error_message);
        exit(EXIT_FAILURE);
    }
}

// Signal handler for cleanup on SIGINT
void signalHandler(int signal) {
    if (signal == SIGINT) {
        munmap(shared_memory_data, SHARED_MEMORY_SIZE);
        close(shared_memory_fd);
        shm_unlink(SHARED_MEMORY_NAME);
        log_file.close();
        exit(EXIT_SUCCESS);
    }
}

// Daemon function to initialize shared memory and log file
void daemonize(const char* daemon_mode, const char* log_file_name, int poll_interval) {
    if (strcmp(daemon_mode, "fileio") == 0) {
        SHARED_MEMORY_NAME = "/shm_fileio";
    } else if (strcmp(daemon_mode, "memmgmt") == 0){
        SHARED_MEMORY_NAME = "/shm_memmgmt";
    } else {
        perror("daemon mod");
        exit(EXIT_FAILURE);
    }

    shared_memory_fd = shm_open(SHARED_MEMORY_NAME, O_RDWR | O_CREAT, 0666);
    handleError(shared_memory_fd == -1, "Failed to open shared memory");
    bool is_newly_created = (errno == ENOENT);
    handleError(ftruncate(shared_memory_fd, sizeof(SharedMemoryData)) == -1, "Failed to set size of shared memory");
    shared_memory_data = static_cast<SharedMemoryData*>(mmap(nullptr, sizeof(SharedMemoryData), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0));
    handleError(shared_memory_data == MAP_FAILED, "Failed to map shared memory");
    log_file.open(log_file_name, std::ios::app);
    handleError(!log_file.is_open(), "Failed to open log file");

    if (is_newly_created) {
        memset(shared_memory_data, 0, sizeof(SharedMemoryData));
        shared_memory_data->offset = 0;
    }

    while (true) {
        std::string data(shared_memory_data->buffer, strnlen(shared_memory_data->buffer, SHARED_MEMORY_SIZE - sizeof(int)));
        if (!data.empty()) {
            log_file << data;
            log_file << "\n";
            log_file.flush();
            memset(shared_memory_data->buffer, 0, SHARED_MEMORY_SIZE - sizeof(int));
            shared_memory_data->offset = 0;
        }
        sleep(poll_interval);
    }
}

int main(int argc, char* argv[]) {
    signal(SIGINT, signalHandler);
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <daemon_mode (fileio / memmgmt)> <log_file_name> <poll_interval>" << std::endl;
        return EXIT_FAILURE;
    }
    daemonize(argv[1], argv[2], std::atoi(argv[3]));
    return EXIT_SUCCESS;
}