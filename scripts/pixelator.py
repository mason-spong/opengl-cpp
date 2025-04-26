import numpy as np
from PIL import Image
import os
import argparse # Import argparse for command-line arguments

# Define target dimensions and sampling kernel size
target_size = 16
kernel_size = 5

# --- Setup Argument Parser ---
parser = argparse.ArgumentParser(
    description=f"Resize an image (expected 1024x1024) to {target_size}x{target_size} "
                f"by averaging a central {kernel_size}x{kernel_size} area of each "
                f"corresponding block. Output is saved in the script's directory."
)
parser.add_argument(
    "input_path", # Define a required positional argument
    help="Path to the input image file (e.g., /path/to/your_image.jpg)"
)

# Parse the command-line arguments
args = parser.parse_args()
input_filename = args.input_path # Get the input path from arguments

# --- Check if the input file exists ---
if not os.path.exists(input_filename):
    print(f"Error: Input file '{input_filename}' not found.")
else:
    try:
        # --- Determine Output Filename ---
        base_name = os.path.basename(input_filename) # Get filename e.g., "dirt.jpg"
        name_part, ext_part = os.path.splitext(base_name) # Split into "dirt" and ".jpg"
        # Construct output name in the current directory
        output_filename = f"{name_part}_16x16.png"

        # --- Image Processing Logic (mostly unchanged) ---

        # Load the image
        img = Image.open(input_filename)

        # Ensure image is in RGB format
        img = img.convert("RGB")

        # Get image dimensions
        width, height = img.size

        # Validate dimensions (optional but recommended)
        if width != 1024 or height != 1024:
            print(f"Warning: Input image is {width}x{height}, expected 1024x1024. "
                  f"Proceeding, but block calculations assume 1024x1024 input for {target_size}x{target_size} output.")
            # Adjust block size calculation if you want to handle other sizes
            # For now, we'll keep the assumption for the 16x16 target
            if width % target_size != 0 or height % target_size != 0:
                 print(f"Error: Image dimensions ({width}x{height}) are not cleanly divisible by target size ({target_size}). Cannot proceed.")
                 exit() # Exit if dimensions are not divisible
            block_width = width // target_size
            block_height = height // target_size
        else:
             block_width = width // target_size # Should be 64
             block_height = height // target_size # Should be 64


        # Calculate the radius of the kernel
        if kernel_size % 2 == 0:
            print("Warning: Kernel size should be odd for a distinct center. Using as is.")
            # Adjust radius calculation slightly for even kernels if needed
            kernel_radius_x_low = kernel_size // 2 - 1
            kernel_radius_x_high = kernel_size // 2
            kernel_radius_y_low = kernel_size // 2 - 1
            kernel_radius_y_high = kernel_size // 2
        else:
            kernel_radius_x_low = kernel_size // 2
            kernel_radius_x_high = kernel_size // 2
            kernel_radius_y_low = kernel_size // 2
            kernel_radius_y_high = kernel_size // 2


        # Create a new blank image for the output
        output_img = Image.new("RGB", (target_size, target_size))

        # Convert the input image to a NumPy array
        img_array = np.array(img)

        # Iterate through each pixel block of the target image
        for y_target in range(target_size):
            for x_target in range(target_size):
                # Calculate the center coordinates of the corresponding block
                center_x = x_target * block_width + block_width // 2
                center_y = y_target * block_height + block_height // 2

                # Calculate the bounds of the sampling area
                start_x = max(0, center_x - kernel_radius_x_low)
                end_x = min(width, center_x + kernel_radius_x_high + 1) # Numpy slice excludes end
                start_y = max(0, center_y - kernel_radius_y_low)
                end_y = min(height, center_y + kernel_radius_y_high + 1) # Numpy slice excludes end

                # Extract the region
                region = img_array[start_y:end_y, start_x:end_x]

                # Calculate the average color
                if region.size > 0:
                   avg_color = np.mean(region, axis=(0, 1))
                else:
                   avg_color = np.array([0,0,0]) # Fallback

                # Convert average color components to integers
                avg_color_int = tuple(avg_color.astype(np.uint8))

                # Set the pixel in the output image
                output_img.putpixel((x_target, y_target), avg_color_int)

        # Save the resulting image IN THE CURRENT DIRECTORY
        output_img.save(output_filename)
        print(f"Successfully created '{output_filename}' in the script directory.")

    except Exception as e:
        print(f"An error occurred during image processing: {e}")