#! /bin/bash

if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
    echo "Usage: $0 <video_file_path> [percentage]"
    exit 1
fi

VIDEO_PATH="$1"
PERCENTAGE="${2:-100}"

python vid_to_ascii.py "$VIDEO_PATH" "$PERCENTAGE" || exit 1

echo "Running gzip"
gzip -9 -c -k ascii_video_frames.txt > ascii_video_frames.gz

echo "Running objcopy"
objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 ascii_video_frames.gz ascii_video_frames.o

file_size=$(stat -c %s ascii_video_frames.txt)

sed -i "s/#define UNCOMPRESSED_SIZE [0-9]\+/#define UNCOMPRESSED_SIZE $file_size/" play_obj.c

echo "Running gcc"
gcc -o ascii_video play_obj.c ascii_video_frames.o -Os -lz
