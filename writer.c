#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// Function to read and directly print memory from a process
void overwrite_memory(pid_t pid, unsigned long start_addr, const char* data) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid); // Build the memory file path

    // Open the memory file of the process
    int fd = open(path, O_RDWR);
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
    
    // Write data on process memory
    if (write(fd, data, strlen(data)) == -1) {
        perror("Error writing to the process memory");
    } else {
        printf("Memory successfully modified in process %d, address 0x%lx.\n", pid, start_addr);
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

    // Parse start address
    char *endptr;
    unsigned long start_addr = strtoul(argv[2], &endptr, 16);
    if (*endptr != '\0') {
        fprintf(stderr, "Invalid start address.\n");
        return 1;
    }

    // Data to replace
    const char *data = argv[3];


    // Read and print memory
    overwrite_memory(pid, start_addr, data);

    return 0;
}
