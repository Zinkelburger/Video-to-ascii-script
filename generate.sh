#!/bin/bash

if [ "$#" -lt 1 ] || [ "$#" -gt 3 ]; then
    echo "Usage: $0 <video_file_path> [percentage] [frame_rate]"
    exit 1
fi

VIDEO_PATH="$1"
PERCENTAGE="${2:-100}"
FRAME_RATE="${3:-30}"

DELAY=$((1000000 / FRAME_RATE))

python vid_to_ascii.py "$VIDEO_PATH" "$PERCENTAGE" || exit 1

echo "Running gzip"
gzip -9 -c -k ascii_video_frames.txt > ascii_video_frames.gz

echo "Running objcopy"
objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 ascii_video_frames.gz ascii_video_frames.o

echo "Running gcc"
file_size=$(stat -c %s ascii_video_frames.txt)
gcc -DUNCOMPRESSED_SIZE="$file_size" -DDELAY="$DELAY" -o ascii_video play_obj.c ascii_video_frames.o -Os -lz
