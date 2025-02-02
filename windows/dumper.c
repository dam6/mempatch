#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <io.h>
#include <stdint.h>

#define CHUNK_SIZE 4096  // Define chunk size as a macro

void read_and_print_memory(DWORD pid, uintptr_t start_addr, size_t mem_size) {

    // Open a handle on the process by Windows API
    HANDLE handleProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (handleProcess == NULL) {
        fprintf(stderr, "Error opening process: %lu\n", GetLastError());
        return;
    }

    // Read memory in chunks
    char buffer[CHUNK_SIZE];
    size_t bytes_remaining = mem_size;
    size_t bytes_to_read;
    SIZE_T bytes_read;
    uintptr_t current_addr = start_addr;

    // Memory metadata struct
    MEMORY_BASIC_INFORMATION memoryInfo;

    // Iterate until the end of the process memory
    while (bytes_remaining > 0) {     
        
        if (bytes_remaining < CHUNK_SIZE) {
            bytes_to_read = bytes_remaining;
        } else {
            bytes_to_read = CHUNK_SIZE;
        }

        // Query memory attributes
        if (VirtualQueryEx(handleProcess, (LPCVOID)current_addr, &memoryInfo, sizeof(memoryInfo)) == 0) {
            break;
        }

        // Skip unreadable or protected memory pages
        if (memoryInfo.State != MEM_COMMIT || memoryInfo.Protect == PAGE_NOACCESS || memoryInfo.Protect == PAGE_EXECUTE) {
            current_addr += memoryInfo.RegionSize;
            continue;
        }

        // Read memory
        if (ReadProcessMemory(handleProcess, (LPCVOID)current_addr, buffer, bytes_to_read, &bytes_read) == 0) {
            if (GetLastError() != ERROR_PARTIAL_COPY) {
                break;
            }
        }

        // If no bytes were read, move to the next region
        if (bytes_read == 0) {
            current_addr += memoryInfo.RegionSize;
            continue;    
        }


        // Write to stdout
        if (_write(_fileno(stdout), buffer, bytes_read) == -1) {
            fprintf(stderr, "Error writing to stdout: %lu\n", GetLastError());
            break;
        }

        bytes_remaining -= bytes_read;
        current_addr += bytes_read;
    }

    CloseHandle(handleProcess);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <pid> <start_address> <end_address>\n", argv[0]);
        return 1;
    }

    // Get PID from arguments
    DWORD pid = (DWORD)atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID.\n");
        return 1;
    }

    // Parse start and end addresses
    char *endptr;
    uintptr_t start_addr = strtoull(argv[2], &endptr, 16);
        if (*endptr != '\0') {
        fprintf(stderr, "Invalid start address.\n");
        return 1;
    }

    uintptr_t end_addr = strtoull(argv[3], &endptr, 16);
        if (*endptr != '\0') {
        fprintf(stderr, "Invalid end address.\n");
        return 1;
    }

    // Validate address range
    if (end_addr <= start_addr) {
        fprintf(stderr, "The end address must be greater than the start address.\n");
        return 1;
    }

    // Calculate memory size
    size_t mem_size = (size_t)(end_addr - start_addr);

    // Read and print memory
    read_and_print_memory(pid, start_addr, mem_size);
    
    return 0;
}