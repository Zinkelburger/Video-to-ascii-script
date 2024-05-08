#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>  // Include for system function
#include "ascii_frames.h"

int main() {
    while (1) {  // Infinite loop to replay the video
        char *frame = (char *)ascii_video_frames_txt;  // Pointer to the array
        char *next_frame = NULL;
        const char *delimiter = "\n---FRAME---\n";
        
        while ((next_frame = strstr(frame, delimiter)) != NULL) {
            *next_frame = '\0';  // Terminate the current frame string at the delimiter
            printf("%s", frame); // Print the current frame
            usleep(33000);       // Delay for approximately 33 milliseconds
            system("clear");     // Clear the screen for the next frame
            
            frame = next_frame + strlen(delimiter); // Move the pointer to the start of the next frame
        }
        
        printf("%s", frame); // Print the last frame (after the last delimiter)
    }
    return 0;
}
