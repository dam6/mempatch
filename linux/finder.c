#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define CHUNK_SIZE 4096  // Define chunk size as a macro

// Function to find data in a memory process
void search_string_in_memory(pid_t pid, unsigned long start_addr, size_t mem_size, const char *search_str) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid); // Build the memory file path

    // Open the memory file of the process
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening memory file");
        return;
    }

    // Seek to the start address
    if (lseek(fd, start_addr, SEEK_SET) == -1) {
        perror("Error seeking to start address");
        close(fd);
        return;
    }

    // Read memory in chunks
    char buffer[CHUNK_SIZE];
    size_t bytes_remaining = mem_size;
    size_t bytes_to_read;
    unsigned long current_addr = start_addr;

    while (bytes_remaining > 0) {     
        
        if (bytes_remaining < CHUNK_SIZE) {
            bytes_to_read = bytes_remaining;
        } else {
            bytes_to_read = CHUNK_SIZE;
        }

        if (read(fd, buffer, bytes_to_read) == -1) {
            perror("Error reading memory");
            break;
        }

        // Find string on memory chunk
        char *found = memmem(buffer, bytes_to_read, search_str, strlen(search_str));
        if (found) {
            unsigned long found_addr = current_addr + (found - buffer);
            printf("0x%lx\n", found_addr);
        }

        bytes_remaining -= bytes_to_read;
        current_addr += bytes_to_read;
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <pid> <start_address> <end_address> <data>\n", argv[0]);
        return 1;
    }

    // Parse the PID
    pid_t pid = (pid_t)atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID.\n");
        return 1;
    }

    // Parse start and end addresses
    char *endptr;
    unsigned long start_addr = strtoul(argv[2], &endptr, 16);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid start address.\n");
        return 1;
    }

    unsigned long end_addr = strtoul(argv[3], &endptr, 16);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid end address.\n");
        return 1;
    }

    // Validate the address range
    if (end_addr <= start_addr) {
        fprintf(stderr, "The end address must be greater than the start address.\n");
        return 1;
    }

    // Parse data to be searched
    const char *search_str = argv[4];

    // Calculate the mem_size of the memory range
    size_t mem_size = (size_t)(end_addr - start_addr);

    // Read and print memory
    search_string_in_memory(pid, start_addr, mem_size, search_str);

    return 0;
}
