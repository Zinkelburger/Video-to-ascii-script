#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "ascii_frames.h"
#include <zlib.h>

#define UNCOMPRESSED_SIZE 293821984  // Adjust based on the expected size of uncompressed data

int decompress_data(unsigned char **uncompressed_data) {
    *uncompressed_data = malloc(UNCOMPRESSED_SIZE);
    if (*uncompressed_data == NULL) {
        fprintf(stderr, "Failed to allocate memory for uncompressed data.\n");
        return -1;
    }

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = ascii_video_frames_gz_len;
    strm.next_in = ascii_video_frames_gz;
    strm.avail_out = UNCOMPRESSED_SIZE;
    strm.next_out = *uncompressed_data;

    // Initialize the zlib decompression for gzip
    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        free(*uncompressed_data);
        fprintf(stderr, "Failed to initialize zlib.\n");
        return -1;
    }

    int ret = inflate(&strm, Z_FINISH);
    if (ret != Z_STREAM_END) {
        fprintf(stderr, "Failed to decompress data: %s\n", strm.msg);
        inflateEnd(&strm);
        free(*uncompressed_data);
        return -1;
    }

    inflateEnd(&strm);
    return 0;
}

int main() {
    unsigned char *uncompressed_data = NULL;
    if (decompress_data(&uncompressed_data) != 0) {
        return 1;  // Exit if decompression fails
    }

    // Process the decompressed data
    char *frame = (char *)uncompressed_data;
    char *next_frame = NULL;
    const char *delimiter = "\n---FRAME---\n";
    char *end_marker = strstr(frame, "---END---\n"); // Find the end marker only once

    while (1) {  // Infinite loop to replay the video
        while ((next_frame = strstr(frame, delimiter)) != NULL && frame < end_marker) {
            *next_frame = '\0';  // Terminate the current frame string at the delimiter
            printf("%s", frame); // Print the current frame
            usleep(16000);       // Delay for approximately 16 milliseconds
            printf("\033[H\033[J");    // Clear the screen for the next frame
            
            frame = next_frame + strlen(delimiter); // Move the pointer to the start of the next frame
        }

        // Print the last frame if it's before the end marker
        if (frame < end_marker) {
            printf("%s", frame);
        }
        
        // Reset to the start of the frames for looping
        frame = (char *)uncompressed_data;  // Reset pointer to the beginning
    }

    free(uncompressed_data);
    return 0;
}
