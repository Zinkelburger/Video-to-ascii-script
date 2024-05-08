#!/bin/bash

# Check if an argument was provided
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <video_file_path>"
    exit 1
fi

VIDEO_PATH="$1"

# Step 1: Run the Python script to generate the ASCII video frames
python vid_to_ascii.py "$VIDEO_PATH"

# Check if the Python script was successful
if [ ! -f "ascii_video_frames.txt" ]; then
    echo "Failed to generate ASCII video frames."
    exit 1
fi

# Compress the ASCII frames
gzip -c ascii_video_frames.txt > ascii_video_frames.gz

# Step 2: Convert the compressed ASCII frames to a C header file
xxd -i ascii_video_frames.gz > ascii_frames.h

file_size=$(stat -c %s ascii_frames.h)

sed -i "s/#define UNCOMPRESSED_SIZE [0-9]\+/#define UNCOMPRESSED_SIZE $file_size/" play.c

# Step 3: Compile the C program that uses the header
gcc -o ascii_player play.c -lz

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful, running the program..."
    ./ascii_player
else
    echo "Compilation failed."
    exit 2
fi
