#!/bin/bash

# Compress the ASCII frames
gzip -c ascii_video_frames.txt > ascii_video_frames.gz

# Step 2: Convert the compressed ASCII frames to a C header file
xxd -i ascii_video_frames.gz > ascii_frames.h

file_size=$(stat -c %s ascii_video_frames.txt)

sed -i "s/#define UNCOMPRESSED_SIZE [0-9]\+/#define UNCOMPRESSED_SIZE $file_size/" play_compressed.c

# Step 3: Compile the C program that uses the header
gcc -o ascii_player play_compressed.c -Oz -lz

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful, running the program..."
    ./ascii_player
else
    echo "Compilation failed."
    exit 2
fi
