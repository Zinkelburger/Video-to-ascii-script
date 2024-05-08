 #! /bin/bash
objcopy --input binary --output elf64-x86-64 --binary-architecture i386:x86-64 ascii_video_frames.gz ascii_video_frames.o

gcc -o ascii_obj play_obj.c ascii_video_frames.o -Oz -Os -lz
