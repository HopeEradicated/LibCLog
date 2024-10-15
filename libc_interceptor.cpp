#include <cstdio>
#include <cstring>
#include <sys/mman.h>
#include <sys/time.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <algorithm>
#include <string>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <semaphore.h>

namespace libcwrapper 
{
    // Constants for shared memory and semaphores
    int SHARED_MEMORY_SIZE = 1024;
    const char* SHARED_MEMORY_FILEIO_NAME = "/shm_fileio";
    const char* SHARED_MEMORY_MEMMGMT_NAME = "/shm_memmgmt";
    const char* SEMAPHORE_NAME_FILEIO = "/sem_fileio";
    const char* SEMAPHORE_NAME_MEMMGMT = "/sem_memmgmt";

    // Shared resources
    char* shared_memory_buffer_fileio = nullptr;
    char* shared_memory_buffer_memmgmt = nullptr;
    sem_t* shared_memory_semaphore = nullptr;
    int offset_fileio = 0;
    int offset_memmgmt = 0;

    void handleError(bool condition, const char* error_message) {
        if (condition) {
            perror(error_message);
            exit(EXIT_FAILURE);
        }
    }
    // Function to retrieve the current process name
    std::string getCurrentProcessName() {
        /*
        pid_t pid = getpid();
        std::string path = "/proc/" + std::to_string(pid) + "/cmdline";
        std::ifstream cmdline_file(path, std::ios::in | std::ios::binary);
        handleError(!cmdline_file.is_open(),"Failed to open cmd file");
        std::string process_name;
        std::getline(cmdline_file, process_name, '\0');
        handleError(process_name.empty(),"Failed to read process name");
        */
        return "process_name"; // Placeholder return
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

    // Initialize shared memory buffer
    void initializeSharedMemory(const char* shm_name, char*& shared_memory_buffer) {
        int shared_memory_fd = shm_open(shm_name, O_RDWR, 0666);
        handleError(shared_memory_fd == -1, "Failed to open shared memory");
        handleError(ftruncate(shared_memory_fd, SHARED_MEMORY_SIZE) == -1, "Failed to set shared memory size");
        shared_memory_buffer = static_cast<char*>(mmap(nullptr, SHARED_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0));
        handleError(shared_memory_buffer == MAP_FAILED, "Failed to map shared memory");
        memset(shared_memory_buffer, 0, SHARED_MEMORY_SIZE);
    }

    void writeToSharedMemory(char* shared_memory_buffer, int& shared_memory_offset, const char* message) {
        int message_length = strlen(message);
        if (shared_memory_offset + message_length >= SHARED_MEMORY_SIZE) {
            shared_memory_offset = 0;
        }
        message_length = std::min(message_length, static_cast<int>(SHARED_MEMORY_SIZE - shared_memory_offset));
        memcpy(shared_memory_buffer + shared_memory_offset, message, message_length);
        shared_memory_offset += message_length;
        memset(shared_memory_buffer + shared_memory_offset, 0, SHARED_MEMORY_SIZE - shared_memory_offset);
    }

    // Function pointers for libc functions
    int (*libc_open)(const char*, int);
    int (*libc_close)(int);
    off_t (*libc_lseek)(int, off_t, int);
    ssize_t (*libc_read)(int, void*, size_t);
    ssize_t (*libc_write)(int, const void*, size_t);
    void* (*libc_malloc)(size_t);
    void* (*libc_realloc)(void*, size_t);
    void (*libc_free)(void*);

    __attribute__((constructor))
    void initializeLibrary() {
        initializeSharedMemory(SHARED_MEMORY_FILEIO_NAME, shared_memory_buffer_fileio);
        initializeSharedMemory(SHARED_MEMORY_MEMMGMT_NAME, shared_memory_buffer_memmgmt);

        // Load libc function pointers
        libc_open = (int (*)(const char*, int))dlsym(RTLD_NEXT, "open");
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
    }

    // Cleanup the library
    __attribute__((destructor))
    void finalizeLibrary() {
        munmap(shared_memory_buffer_fileio, SHARED_MEMORY_SIZE);
        munmap(shared_memory_buffer_memmgmt, SHARED_MEMORY_SIZE);
    }

    // Intercepted libc functions
    extern "C" {
        int open(const char* filename, int flags) {
            handleError(!libc_open, "libc_open is null");
            int file_descriptor = libc_open(filename, flags);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, open: filename=%s, flags=%d, fd=%d\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), filename, flags, file_descriptor);
            writeToSharedMemory(shared_memory_buffer_fileio, offset_fileio, log_message);
            return file_descriptor;
        }

        int close(int fd) {
            handleError(!libc_close, "libc_close is null");
            int return_code = libc_close(fd);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, close: fd=%d, rc=%d\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), fd, return_code);
            writeToSharedMemory(shared_memory_buffer_fileio, offset_fileio, log_message);
            return return_code;
        }

        off_t lseek(int fd, off_t offset, int whence) {
            handleError(!libc_lseek, "libc_lseek is null");
            off_t result = libc_lseek(fd, offset, whence);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, lseek: fd=%d, offset=%ld, whence=%d, result=%ld\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), fd, offset, whence, result);
            writeToSharedMemory(shared_memory_buffer_fileio, offset_fileio, log_message);
            return result;
        }

        ssize_t read(int fd, void* buffer, size_t count) {
            handleError(!libc_read, "libc_read is null");
            ssize_t bytes_read = libc_read(fd, buffer, count);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, read: fd=%d, buf=%p, count=%zu, bytes_read=%zd\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), fd, buffer, count, bytes_read);
            writeToSharedMemory(shared_memory_buffer_fileio, offset_fileio, log_message);
            return bytes_read;
        }

        ssize_t write(int fd, const void* buffer, size_t count) {
            handleError(!libc_write, "libc_write is null");
            ssize_t bytes_written = libc_write(fd, buffer, count);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, write: fd=%d, buf=%p, count=%zu, bytes_written=%zd\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), fd, buffer, count, bytes_written);
            writeToSharedMemory(shared_memory_buffer_fileio, offset_fileio, log_message);
            return bytes_written;
        }
        /*
        void* malloc(size_t size) {
            if (!libc_malloc) {
                libc_malloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
                handleError(!libc_malloc, "libc_malloc is null");
            }
            
            void* pointer = libc_malloc(size);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();
            
            snprintf(log_message, sizeof(log_message), 
                     "[%s] PID=%d, process=%s, malloc: size=%zu, ptr=%p\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), size, pointer);
            writeToSharedMemory(shared_memory_buffer_memmgmt, offset_memmgmt, log_message);
            return pointer;
        }*/

        /*
        void* realloc(void* ptr, size_t size) {
            if (!libc_realloc) {
                libc_realloc = (void* (*)(void*, size_t))dlsym(RTLD_NEXT, "realloc");
                handleError(!libc_realloc, "libc_realloc is null");
            }
            void* new_ptr = libc_realloc(ptr, size);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), "[%s] PID=%d, process=%s, realloc: old_ptr=%p, size=%zu, new_ptr=%p\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), ptr, size, new_ptr);
            writeToSharedMemory(log_message);
            return new_ptr;
        }

        void free(void* ptr) {
            if (!libc_free) {
                libc_free = (void (*)(void*))dlsym(RTLD_NEXT, "free");
                handleError(!libc_free, "libc_free is null");
            }
            libc_free(ptr);
            char log_message[256];
            std::string timestamp = getCurrentTimestamp();
            std::string process_name = getCurrentProcessName();

            snprintf(log_message, sizeof(log_message), "[%s] PID=%d, process=%s, free: ptr=%p\n",
                     timestamp.c_str(), getpid(), process_name.c_str(), ptr);
            writeToSharedMemory(log_message);
        } 
        */
    }
}