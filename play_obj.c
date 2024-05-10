#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include <signal.h>

// Declarations to access the embedded data
extern unsigned char _binary_ascii_video_frames_gz_start;
extern unsigned char _binary_ascii_video_frames_gz_end;

void handle_signal(int sig) {
        // put the cursor back
        printf("\033[?25h\n");
        fflush(stdout);
        exit(0);
}

int decompress_data(unsigned char **uncompressed_data) {
        unsigned char *compressed_data = &_binary_ascii_video_frames_gz_start;
        size_t compressed_size = &_binary_ascii_video_frames_gz_end - &_binary_ascii_video_frames_gz_start;

        *uncompressed_data = malloc(UNCOMPRESSED_SIZE);
        if (*uncompressed_data == NULL) {
                fprintf(stderr, "Failed to allocate memory for uncompressed data.\n");
                return -1;
        }

        z_stream strm = {
                .zalloc = Z_NULL,
                .zfree = Z_NULL,
                .opaque = Z_NULL,
                .avail_in = compressed_size,
                .next_in = compressed_data,
                .avail_out = UNCOMPRESSED_SIZE,
                .next_out = *uncompressed_data
        };

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
        signal(SIGINT, handle_signal);

        unsigned char *uncompressed_data = NULL;
        if (decompress_data(&uncompressed_data) != 0) {
                return 1;
        }

        // Process the decompressed data
        char *frame = (char *)uncompressed_data;
        char *next_frame = NULL;
        const char *delimiter = "\n---FRAME---\n";
        char *end_marker = strstr(frame, "---END---\n");

        printf("\033[?25l");
        while (1) {
                while ((next_frame = strstr(frame, delimiter)) != NULL && frame < end_marker) {
                        char temp = *next_frame;
                        *next_frame = '\0';
                        printf("\033[H\033[J%s", frame);
                        fflush(stdout);
                        usleep(40000);
                        *next_frame = temp;
                        frame = next_frame + strlen(delimiter);
                }

                if (frame < end_marker) {
                        printf("%s", frame);
                }
                
                frame = (char *)uncompressed_data;
        }

        free(uncompressed_data);
        return 0;
}
