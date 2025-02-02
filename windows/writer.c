#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

#define CHUNK_SIZE 4096  // Define chunk size as a macro

void overwrite_memory(DWORD pid, uintptr_t start_addr,  const char* data) {

    // Open a handle on the process by windows api
    HANDLE handleProcess = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION , FALSE, pid);
    if (handleProcess == NULL) {
        fprintf(stderr, "Error to handle process: %lu\n", GetLastError());
        return;
    }

    // Overwrite memory
    if (WriteProcessMemory(handleProcess, (LPCVOID)start_addr, (LPCVOID)data, strlen(data), NULL) == 0) {
        fprintf(stderr, "Error writing to the process memory: %lu\n", GetLastError());
    } else {
        printf("Memory successfully modified in process %d, address 0x%llx.\n", pid, (unsigned long long)start_addr);
    }

    CloseHandle(handleProcess);

}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <pid> <memory_address> <data>\n", argv[0]);
        return 1;
    }

    // PID from arg
    DWORD pid = (DWORD)atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "Invalid PID.\n");
        return 1;
    }

    // Parse memory address
    char *endptr;
    uintptr_t start_addr = strtoull(argv[2], &endptr, 16);
        if (*endptr != '\0') {
        fprintf(stderr, "Invalid start address.\n");
        return 1;
    }

   // Data to overwirte
    const char *data = argv[3];

    // Read and print memory
    overwrite_memory(pid, start_addr, data);
    
    return 0;
}
