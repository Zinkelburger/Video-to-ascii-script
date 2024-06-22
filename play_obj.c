#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>
#include <signal.h>
#include <sys/ioctl.h>

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

// Function to get the terminal size
void get_terminal_size(int *rows, int *cols) {
        struct winsize ws;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
                perror("ioctl");
                exit(EXIT_FAILURE);
        }
        *rows = ws.ws_row;
        *cols = ws.ws_col;
}

// Function to print a string up to a certain number of visible characters
void print_visible(const char *str, int cols) {
        int num_printed = 0;
        int in_escape = 0;

        while (*str && num_printed < cols) {
                if (*str == '\033') {
                        in_escape = 1;
                        putchar(*str);
                } else if (in_escape && *str == 'm') {
                        in_escape = 0;
                        putchar(*str);
                } else if (!in_escape) {
                        if (*str != ' ') {
                                if (num_printed < cols) {
                                        putchar(*str);
                                        num_printed++;
                                }
                        }
                } else {
                        putchar(*str);
                }
                str++;
        }
        putchar('\n');
}

// Function to print a frame within the terminal dimensions
void print_frame_within_terminal(char *frame, int rows, int cols) {
        int line_count = 0;
        char *line = strtok(frame, "\n");

        while (line != NULL && line_count < rows) {
                print_visible(line, cols);
                line_count++;
                line = strtok(NULL, "\n");
        }
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

        // hides the cursor
        printf("\033[?25l");
        while (1) {
                int rows, cols;
                while ((next_frame = strstr(frame, delimiter)) != NULL && frame < end_marker) {
                        get_terminal_size(&rows, &cols);
                        char temp = *next_frame;
                        *next_frame = '\0';
                        // move cursor to top left, clear screen
                        printf("\033[H\033[J");
                        print_frame_within_terminal(frame, rows, cols);
                        fflush(stdout);
                        usleep(DELAY);
                        *next_frame = temp;
                        frame = next_frame + strlen(delimiter);
                }

                if (frame < end_marker) {
                        printf("\033[H\033[J");
                        print_frame_within_terminal(frame, rows, cols);
                }

                frame = (char *)uncompressed_data;
        }

        free(uncompressed_data);
        return 0;
}
