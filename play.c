#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "ascii_frames.h"

int main() {
    while (1) {  // Infinite loop to replay the video
        char *frame = (char *)ascii_video_frames_gz;  // Pointer to the array
        char *next_frame = NULL;
        const char *delimiter = "\n---FRAME---\n";
        char *end_marker = strstr(frame, "---END---\n"); // Find the end marker only once

        while ((next_frame = strstr(frame, delimiter)) != NULL && frame < end_marker) {
            *next_frame = '\0';  // Terminate the current frame string at the delimiter
            printf("%s", frame); // Print the current frame
            usleep(33000);       // Delay for approximately 33 milliseconds
            printf("\033[H\033[J");    // Clear the screen for the next frame
            
            frame = next_frame + strlen(delimiter); // Move the pointer to the start of the next frame
        }

        // Print the last frame if it's before the end marker
        if (frame < end_marker) {
            printf("%s", frame);
        }
        
        // Reset to the start of the frames for looping
        frame = (char *)ascii_video_frames_txt;  // Reset pointer to the beginning
    }
    return 0;
}
