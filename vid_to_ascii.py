import cv2
from ascii_magic import AsciiArt
from PIL import Image
import io
import sys
import re

def video_to_ascii(video_path, output_file_path, percentage):
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error: Could not open video.")
        return

    total_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
    end_frame = int(total_frames * (percentage / 100))

    if end_frame == 0:
        print("Error: Percentage too low, no frames to process.")
        return

    with open(output_file_path, 'w') as file:
        frame_count = 0
        while frame_count < end_frame:
            # Read frame from the video
            ret, frame = cap.read()
            if not ret:
                break

            # Convert the frame to RGB
            frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

            # Convert numpy array (frame) to PIL Image
            img = Image.fromarray(frame_rgb)

            # Convert PIL Image to ASCII
            with io.BytesIO() as output:
                img.save(output, format="JPEG")
                data = output.getvalue()
                ascii_art = AsciiArt.from_image(io.BytesIO(data))

            ascii_frame = ascii_art.to_ascii()
            ascii_frame = re.sub(' ', '', ascii_frame)

            file.write(ascii_frame + '\n---FRAME---\n')

            frame_count += 1
            sys.stdout.write(f"\rProcessed frame {frame_count}/{end_frame}")
            sys.stdout.flush()

        file.write('---END---\n')

        cap.release()
        print("\nDone processing video.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python vid_to_ascii.py <video_path> [percentage]")
        sys.exit(1)

    video_path = sys.argv[1]

    percentage = float(sys.argv[2]) if len(sys.argv) > 2 else 100
    if percentage <= 0:
        print("Error: Percentage must be greater than 0.")
        sys.exit(1)

    output_file_path = "ascii_video_frames.txt"
    video_to_ascii(video_path, output_file_path, percentage)