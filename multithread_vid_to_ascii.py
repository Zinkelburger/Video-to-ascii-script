import cv2
from ascii_magic import AsciiArt
from PIL import Image
import io
import sys
from concurrent.futures import ThreadPoolExecutor, as_completed

def process_frame(frame):
    """Converts a single frame to ASCII."""
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
    return ascii_art.to_ascii()

def video_to_ascii(video_path, output_file_path):
    # Open the video file
    cap = cv2.VideoCapture(video_path)
    if not cap.isOpened():
        print("Error: Could not open video.")
        return

    frames = []
    while True:
        ret, frame = cap.read()
        if not ret:
            break
        frames.append(frame)

    # Process frames in parallel
    with ThreadPoolExecutor() as executor:
        future_to_frame = {executor.submit(process_frame, frame): idx for idx, frame in enumerate(frames)}
        print('Did the thing for all the frames')
        results = {}
        for future in as_completed(future_to_frame):
            frame_idx = future_to_frame[future]
            try:
                results[frame_idx] = future.result()
                print(f"Completed frame {frame_idx}")
            except Exception as exc:
                print(f"Frame {frame_idx} generated an exception: {exc}")

    # Release video capture
    cap.release()

    # Write results to file in order
    with open(output_file_path, 'w') as file:
        for idx in sorted(results):
            file.write(results[idx] + '\n---FRAME---\n')
        file.write('---END---\n')

    print("Done processing video.")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python vid_to_ascii.py <video_path>")
        sys.exit(1)
    video_path = sys.argv[1]
    output_file_path = "ascii_video_frames.txt"
    video_to_ascii(video_path, output_file_path)
