#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

// Function to check for errors and handle them appropriately
void handleError(bool condition, const char* error_message) {
    if (condition) {
        perror(error_message);
        exit(EXIT_FAILURE);
    }
}

// Function to demonstrate file I/O operations
void testFileIO() {
    int file_descriptor = open("test_file.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    handleError(file_descriptor == -1, "Failed to open file");

    const char* write_buffer = "sabd";
    ssize_t bytes_written = write(file_descriptor, write_buffer, strlen(write_buffer));
    handleError(bytes_written == -1, "Failed to write to file");

    char read_buffer[10];
    ssize_t bytes_read = read(file_descriptor, read_buffer, sizeof(read_buffer) - 1);
    handleError(bytes_read == -1, "Failed to read from file");
    read_buffer[bytes_read] = '\0'; // Null-terminate the string

    off_t seek_result = lseek(file_descriptor, 0, SEEK_CUR);
    handleError(seek_result == -1, "Failed to seek in file");

    int close_result = close(file_descriptor);
    handleError(close_result == -1, "Failed to close file");
}

// Function to test dynamic memory allocation
void testMemoryAllocation() {
    void* memory_ptr = malloc(10);
    handleError(memory_ptr == NULL, "Failed to allocate memory");

    free(memory_ptr);
}

// Function to test memory reallocation
void testMemoryReallocation() {
    void* memory_ptr = malloc(10);
    handleError(memory_ptr == NULL, "Failed to allocate memory");

    memory_ptr = realloc(memory_ptr, 20);
    handleError(memory_ptr == NULL, "Failed to reallocate memory");

    free(memory_ptr);
}

int main() {
    // Test file I/O
    testFileIO();

    // Test dynamic memory allocation
    //testMemoryAllocation();

    // Test memory reallocation
    testMemoryReallocation();

    return 0;
}