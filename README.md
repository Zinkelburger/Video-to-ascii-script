# Video to ascii
`./generate.sh <.mp4 file> <optional percentage>`
This will generate `ascii_video`, which will play the .mp4 file in ascii.

## Steps
`vid_to_ascii.py` uses OpenCV and ascii_magic, which you can install from requirements.txt.
It generates `ascii_video_frames.txt` which is a very large file with all of the frames in ascii.

Because it's a huge file, it is compressed with `gzip`. Its then turned into a binary object `ascii_video_frames.o`, so it can be compiled with C.

We `sed` the uncompressed file size into C, and then compile it.

Now you should have a wonderful `ascii_video` executable to send to your friends (and enemies). The default one in this repo is a rickroll.