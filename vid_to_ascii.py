import cv2
from ascii_magic import AsciiArt
from PIL import Image
import io
import sys

def video_to_ascii(video_path, output_file_path):
    # Open the video file
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error: Could not open video.")
        return

    # Open the output file
    with open(output_file_path, 'w') as file:
        frame_count = 0
        while True:
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

            # Get ASCII art string
            ascii_frame = ascii_art.to_ascii()

            # Write the ASCII art to file
            file.write(ascii_frame + '\n---FRAME---\n')

            # Optional: Print progress
            frame_count += 1
            print(f"Processed frame {frame_count}")

        file.write('---END---\n')

        # Release video capture
        cap.release()
        print("Done processing video.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python vid_to_ascii.py <video_path>")
        sys.exit(1)
    video_path = sys.argv[1]
    output_file_path = "ascii_video_frames.txt"
    video_to_ascii(video_path, output_file_path)
