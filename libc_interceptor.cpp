#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <chrono>
#include <atomic>
#include <stdarg.h>
#include <limits.h>

// Constants for shared memory and semaphores
const int SHARED_MEMORY_SIZE = 4096;
const char* SHARED_MEMORY_FILEIO_NAME = "/shm_fileio";
const char* SHARED_MEMORY_MEMMGMT_NAME = "/shm_memmgmt";

struct SharedMemoryData {
    std::atomic<int> offset;
    char buffer[SHARED_MEMORY_SIZE - sizeof(std::atomic<int>)];
};

// Shared resources
SharedMemoryData* shared_memory_data_fileio = nullptr;
SharedMemoryData* shared_memory_data_memmgmt = nullptr;
std::string process_name = "";

void handleError(bool condition, const char* error_message) {
    if (condition) {
        perror(error_message);
        exit(EXIT_FAILURE);
    }
}

void getCurrentProcessName() {
    if (process_name.empty()) {
        pid_t pid = getpid();
        std::string path = "/proc/" + std::to_string(pid) + "/exe";
        char exe_path[PATH_MAX];
        ssize_t len = readlink(path.c_str(), exe_path, sizeof(exe_path) - 1);
        handleError(len == -1, "Failed to read exe link");
        exe_path[len] = '\0';
        process_name = std::string(exe_path);
        handleError(process_name.empty(), "Failed to retrieve process name");
    }
}

std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    char timestamp[128];
    std::tm* time_info = std::localtime(&now_time_t);
    snprintf(timestamp, sizeof(timestamp), "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
                time_info->tm_year + 1900, time_info->tm_mon + 1, time_info->tm_mday,
                time_info->tm_hour, time_info->tm_min, time_info->tm_sec, milliseconds.count());
    return std::string(timestamp);
}

void initializeSharedMemory(const char* shm_name, SharedMemoryData*& shared_memory_buffer) {
    int shared_memory_fd = shm_open(shm_name, O_RDWR, 0666);
    handleError(shared_memory_fd == -1, "Failed to open shared memory");
    handleError(ftruncate(shared_memory_fd, sizeof(SharedMemoryData)) == -1, "Failed to set shared memory size");
    shared_memory_buffer = static_cast<SharedMemoryData*>(mmap(nullptr, sizeof(SharedMemoryData), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0));
    handleError(shared_memory_buffer == MAP_FAILED, "Failed to map shared memory");
}

void writeToSharedMemory(SharedMemoryData* shared_memory_data, const char* message) {
    int message_length = std::min(static_cast<int>(strlen(message)), 256);
    int current_offset = shared_memory_data->offset.load(std::memory_order_relaxed);
    while (true) {
        int new_offset = current_offset + message_length;
        if (new_offset >= SHARED_MEMORY_SIZE - sizeof(std::atomic<int>)) {
            new_offset = 0; 
        }
        if (shared_memory_data->offset.compare_exchange_weak(current_offset, new_offset, std::memory_order_acquire, std::memory_order_relaxed)) {
            memcpy(shared_memory_data->buffer + current_offset, message, message_length);
            break;
        } else {
            current_offset = shared_memory_data->offset.load(std::memory_order_relaxed);
        }
    }
}

// Function pointers for libc functions
int (*libc_open)(const char*, int, ...) = nullptr;
int (*libc_close)(int) = nullptr;
off_t (*libc_lseek)(int, off_t, int) = nullptr;
ssize_t (*libc_read)(int, void*, size_t) = nullptr;
ssize_t (*libc_write)(int, const void*, size_t) = nullptr;
void* (*libc_malloc)(size_t) = nullptr;
void* (*libc_realloc)(void*, size_t) = nullptr;
void (*libc_free)(void*) = nullptr;

__attribute__((constructor))
void initializeLibrary() {
    initializeSharedMemory(SHARED_MEMORY_FILEIO_NAME, shared_memory_data_fileio);
    initializeSharedMemory(SHARED_MEMORY_MEMMGMT_NAME, shared_memory_data_memmgmt);

    // Load libc function pointers (dlopen + Lazy = crash)
    libc_open = (int (*)(const char*, int, ...))dlsym(RTLD_NEXT, "open");
    libc_close = (int (*)(int))dlsym(RTLD_NEXT, "close");
    libc_lseek = (off_t (*)(int, off_t, int))dlsym(RTLD_NEXT, "lseek");
    libc_read = (ssize_t (*)(int, void*, size_t))dlsym(RTLD_NEXT, "read");
    libc_write = (ssize_t (*)(int, const void*, size_t))dlsym(RTLD_NEXT, "write");
    libc_malloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
    libc_realloc = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc");
    libc_free = (void (*)(void*))dlsym(RTLD_NEXT, "free");
    handleError(!libc_open || !libc_close || !libc_lseek || !libc_read || !libc_write ||
                !libc_malloc || !libc_realloc || !libc_free,
                "Failed to load libc functions");
    getCurrentProcessName();
}

// Cleanup the library
__attribute__((destructor))
void finalizeLibrary() {
    munmap(shared_memory_data_fileio, SHARED_MEMORY_SIZE);
    munmap(shared_memory_data_memmgmt, SHARED_MEMORY_SIZE);
}

// Intercepted libc functions
extern "C" {
    int open(const char* filename, int flags, ...) {
        handleError(!libc_open, "libc_open is null");
        va_list args;
        va_start(args, flags);
        int file_descriptor = libc_open(filename, flags, args);
        va_end(args);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();
        snprintf(log_message, sizeof(log_message), 
                    "[%s] PID=%d, process=%s, open: filename=%s, flags=%d, file_descriptor=%d\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), filename, flags, file_descriptor);
        writeToSharedMemory(shared_memory_data_fileio, log_message);
        return file_descriptor;
    }

    int close(int fd) {
        handleError(!libc_close, "libc_close is null");
        int return_code = libc_close(fd);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();    
        snprintf(log_message, sizeof(log_message), 
                    "[%s] PID=%d, process=%s, close: file_descriptor=%d, return_code=%d\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), fd, return_code);
        writeToSharedMemory(shared_memory_data_fileio, log_message);
        return return_code;
    }

    off_t lseek(int fd, off_t offset, int whence) {
        handleError(!libc_lseek, "libc_lseek is null");
        off_t result = libc_lseek(fd, offset, whence);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();;
        snprintf(log_message, sizeof(log_message), 
                    "[%s] PID=%d, process=%s, lseek: file_descriptor=%d, requested_offset=%ld, whence=%d, resulted_offset=%ld\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), fd, offset, whence, result);
        writeToSharedMemory(shared_memory_data_fileio, log_message);
        return result;
    }

    ssize_t read(int fd, void* buffer, size_t count) {
        handleError(!libc_read, "libc_read is null");
        ssize_t bytes_read = libc_read(fd, buffer, count);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();
        snprintf(log_message, sizeof(log_message), 
                    "[%s] PID=%d, process=%s, read: file_descriptor=%d, buffer_pointer=%p, count=%zu, bytes_read=%zd\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), fd, buffer, count, bytes_read);
        writeToSharedMemory(shared_memory_data_fileio, log_message);
        return bytes_read;
    }

    ssize_t write(int fd, const void* buffer, size_t count) {
        handleError(!libc_write, "libc_write is null");
        ssize_t bytes_written = libc_write(fd, buffer, count);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();
        snprintf(log_message, sizeof(log_message), 
                    "[%s] PID=%d, process=%s, write: file_descriptor=%d, buffer_pointer=%p, count=%zu, bytes_written=%zd\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), fd, buffer, count, bytes_written);
        writeToSharedMemory(shared_memory_data_fileio, log_message);
        return bytes_written;
    }

    void* realloc(void* ptr, size_t size) {
        handleError(!libc_realloc, "libc_realloc is null");
        auto new_ptr = libc_realloc(ptr, size);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();
        snprintf(log_message, sizeof(log_message),
                    "[%s] PID=%d, process=%s, realloc: bytes_requested=%zu, current_mem_pointer=%p, new_mem_pointer=%p\n",
                    timestamp.c_str(), getpid(), process_name.c_str(), size, ptr, new_ptr);
        writeToSharedMemory(shared_memory_data_memmgmt, log_message);
        return new_ptr;
    }

    void free(void* ptr) {
        handleError(!libc_free, "libc_free is null");
        libc_free(ptr);
        char log_message[256];
        snprintf(log_message, sizeof(log_message),
                    "PID=%d, process=%s, free: mem_pointer=%p\n",
                    getpid(), process_name.c_str(), ptr);
        writeToSharedMemory(shared_memory_data_memmgmt, log_message);
    }

    /*
    void* malloc(size_t size) {
        handleError(!libc_malloc, "libc_malloc is null");           
        void* pointer = libc_malloc(size);
        char log_message[256];
        std::string timestamp = getCurrentTimestamp();
        
        snprintf(log_message, sizeof(log_message), 
                    " PID=%d, process=%s, malloc: bytes_requested=%zu, new_mem_pointer=%p\n",
                    getpid(), process_name.c_str(), size, pointer);
        writeToSharedMemory(shared_memory_data_memmgmt, log_message);
        return pointer;
    }
    */
}