#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define CHUNK_SIZE 4096  // Define chunk size as a macro


// Function to read and directly print memory from a process
void read_and_print_memory(pid_t pid, unsigned long start_addr, size_t mem_size) {
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

    // Read and print memory in chunks
    char buffer[CHUNK_SIZE];
    size_t bytes_remaining = mem_size;
    size_t bytes_to_read;

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

        if (write(STDOUT_FILENO, buffer, bytes_to_read) == -1) {
            perror("Error writing to stdout");
            break;
        }

        bytes_remaining -= bytes_to_read;
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <pid> <start_address> <end_address>\n", argv[0]);
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

    // Calculate the mem_size of the memory range
    size_t mem_size = (size_t)(end_addr - start_addr);

    // Read and print memory
    read_and_print_memory(pid, start_addr, mem_size);

    return 0;
}

