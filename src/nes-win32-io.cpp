#include <Windows.h>
#include "nes-types.h"

global u64 bytes_read = 0;

void CALLBACK
nes_win32_io_completion_routine(u64 error_code,
                                 u64 bytes_transferred,
                                 LPOVERLAPPED lpOverlapped) {
        
    bytes_read = bytes_transferred;
}

internal void
nes_win32_io_open_and_write_file(char* file_name, char* write_str, u32 write_str_size) {

    //create the file handle used for reading the file
    HANDLE file_handle = CreateFileA(file_name,
                                    GENERIC_WRITE,
                                    FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                    NULL);

    ASSERT(file_handle != INVALID_HANDLE_VALUE);

    OVERLAPPED overlapped = {0};
    b32 write_result = WriteFileEx(file_handle,
                                   write_str,
                                   strlen(write_str),
                                   &overlapped,
                                   nes_win32_io_completion_routine);

    ASSERT(write_result);

    //close the file
    CloseHandle(file_handle);
}

internal Buffer 
nes_win32_io_open_and_read_file(char *file_name) {

    Buffer file_buffer = {0};

    //create the file handle used for reading the file
    HANDLE file_handle = CreateFileA(file_name,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                                    NULL);
    
    ASSERT(file_handle != INVALID_HANDLE_VALUE);

    //get the file size and create a buffer
    file_buffer.buffer_size = GetFileSize(file_handle, NULL) + 1;
    file_buffer.buffer_contents = (char*)malloc(sizeof(char) * file_buffer.buffer_size);

    //read the file into a null terminated buffer
    OVERLAPPED overlapped = {0};
    bool read_result = ReadFileEx(file_handle,
                                  file_buffer.buffer_contents,
                                  file_buffer.buffer_size,
                                  &overlapped,
                                  nes_win32_io_completion_routine);

    file_buffer.buffer_contents[file_buffer.buffer_size - 1] = '\0';

    ASSERT(read_result);

    //close the file
    CloseHandle(file_handle);

    return (file_buffer);
}


internal void
nes_win32_io_destroy_file_info(Buffer file_buffer) {
    if (file_buffer.buffer_contents != NULL) {
        free(file_buffer.buffer_contents);
    }
}