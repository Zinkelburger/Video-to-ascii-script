#!/bin/bash

ASCII_FILE="ascii_video_frames.txt"

if [ ! -f "$ASCII_FILE" ]; then
    echo "Error: ASCII art file not found."
    exit 1
fi

show_cursor() {
    echo -ne "\033[?25h"
    exit 1
}
trap show_cursor SIGINT

echo -ne "\033[?25l"

while IFS= read -r line
do
    if [[ "$line" == "---END---" ]]; then
        break
    elif [[ "$line" == "---FRAME---" ]]; then
        sleep 0.04
        clear
    else
        echo "$line"
    fi
done < "$ASCII_FILE"

echo -ne "\033[?25h"

echo "Playback finished."
