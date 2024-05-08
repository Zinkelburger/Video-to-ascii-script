#!/bin/bash

# Define the path to the ASCII art file
ASCII_FILE="ascii_video_frames.txt"

# Check if the ASCII file exists
if [ ! -f "$ASCII_FILE" ]; then
    echo "Error: ASCII art file not found."
    exit 1
fi

# Read and display the file content frame by frame
while IFS= read -r line
do
    # Check for the end delimiter
    if [[ "$line" == "---END---" ]]; then
        break # Exit the loop if end delimiter is found
    elif [[ "$line" == "---FRAME---" ]]; then
        sleep 0.016 # Wait for a short period to simulate the time between frames
        clear       # Clear the screen for the next frame
    else
        echo "$line" # Print the line of ASCII art
    fi
done < "$ASCII_FILE"

echo "Playback finished."
