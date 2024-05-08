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
    if [[ "$line" == "---FRAME---" ]]; then
        sleep 0.033
        clear
    else
        echo "$line"
    fi
done < "$ASCII_FILE"

echo "Playback finished."
