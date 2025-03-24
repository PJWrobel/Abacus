#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "abautils.h"

#define MAX_LINE_LENGTH 1024
#define WARNING_LINE_LENGTH 80
#define ERROR_BUFFER_SIZE 256

const char* keywords[] = {"null","fn","struct","enum","u8","u16","u32","u64","i8","i16","i32","i64","f32","f64","Big","bool","char","String","str","if","while","do","for","in","where"};

// Custom error handling structure
typedef struct {
    int error_code;
    char error_message[ERROR_BUFFER_SIZE];
} FileParseError;

// Function to read and process file with comprehensive error handling
int read_file(const char* filename, FileParseError* error) {
    // Clear any previous error
    if (error) {
        error->error_code = 0;
        memset(error->error_message, 0, ERROR_BUFFER_SIZE);
    }

    // Validate input
    if (!filename) {
        if (error) {
            error->error_code = -1;
            snprintf(error->error_message, ERROR_BUFFER_SIZE, 
                     "Invalid filename: NULL pointer provided to filename");
        }
        return -1;
    }

    FILE* fp = fopen(filename, "r");
    if (!fp) {
        if (error) {
            error->error_code = errno;
            snprintf(error->error_message, ERROR_BUFFER_SIZE, 
                     "Failed to open file '%s': %s", 
                     filename, strerror(errno));
        }
        return -1;
    }

    char* line_buffer = malloc(MAX_LINE_LENGTH * sizeof(char));
    if (!line_buffer) {
        if (error) {
            error->error_code = errno;
            snprintf(error->error_message, ERROR_BUFFER_SIZE, 
                     "Memory allocation failed: %s", strerror(errno));
        }
        fclose(fp);
        return -1;
    }

    // Line processing
    int line_number = 0;
    int processed_lines = 0;

    while (fgets(line_buffer, MAX_LINE_LENGTH, fp) != NULL) {
        line_number++;

        // Remove trailing newline if present
        size_t len = strlen(line_buffer);
        if (len > 0 && line_buffer[len-1] == '\n') {
            line_buffer[len-1] = '\0';
        }

        // Check for line length truncation
        if (len == MAX_LINE_LENGTH - 1 && line_buffer[len-1] != '\n') {
            if (error) {
                error->error_code = -2;
                snprintf(error->error_message,
                         ERROR_BUFFER_SIZE, 
                         "FILE %s: Line %d exceeds maximum length",
                         filename,
                         line_number);
            }
            // Optional: Handle long lines (skip or process partially)
        }

        // Process line (replace with your actual processing logic)
        printf("Processing line %d: %s\n", line_number, line_buffer);

        processed_lines++;
    }

    // Check for file reading errors
    if (ferror(fp)) {
        if (error) {
            error->error_code = errno;
            snprintf(error->error_message, ERROR_BUFFER_SIZE, 
                     "Error reading file: %s", strerror(errno));
        }
        free(line_buffer);
        fclose(fp);
        return -1;
    }

    // Cleanup
    free(line_buffer);
    fclose(fp);

    return processed_lines;
}

// Example usage function
void example_usage() {
    FileParseError error;
    
    int result = read_file("pstr.c", &error);
    if (result < 0) {
        fprintf(stderr, "File parsing error: %s\n", 
                error.error_message ? error.error_message : "Unknown error");
        // Handle error appropriately
    } else {
        printf("Successfully processed %d lines\n", result);
    }
}

int main() {
    example_usage();
    return 0;
}
