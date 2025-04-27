from PIL import Image, ImageDraw
import math # Import math for rounding
import argparse # Import argparse for command-line arguments

# --- Setup Command-Line Argument Parsing ---
parser = argparse.ArgumentParser(description='Generate a grid image with specified line thickness.')
parser.add_argument(
    '-lw', '--line-width',
    type=int,
    default=1, # Default line width if not specified
    help='The target thickness of the grid lines in pixels (default: 1)'
)
# Add arguments for other parameters if desired (e.g., image size, num lines)
# parser.add_argument('-s', '--size', type=int, default=1024, help='Image size in pixels')
# parser.add_argument('-n', '--num-lines', type=int, default=15, help='Number of horizontal/vertical lines')

args = parser.parse_args()

# --- Configuration (using parsed arguments and constants) ---
image_size = 1024 # Could also be made a command-line argument
num_lines = 15    # Could also be made a command-line argument
target_line_width = args.line_width # Use the value from command line
background_color = (255, 255, 255) # White
line_color = (0, 0, 0)             # Black
# Update output filename based on the actual line width used
output_filename = f"grid_{target_line_width}px_lines_approx_even.png"

# --- Calculation for Even Spacing (Center-to-Center) ---
# Calculate the ideal spacing between the *centers* of the lines.
# The image is divided into (num_lines + 1) sections by the lines.
center_spacing = image_size / (num_lines + 1)
print(f"Image Size: {image_size}x{image_size}")
print(f"Number of Lines: {num_lines}")
print(f"Target line width: {target_line_width}px")
print(f"Ideal center-to-center spacing: {center_spacing:.2f}px")
print(f"Output filename: {output_filename}")

# --- Image Creation ---
# Create a new blank image with the specified background color
img = Image.new('RGB', (image_size, image_size), background_color)

# Get a drawing context
draw = ImageDraw.Draw(img)

# --- Draw Vertical Lines ---
print("Drawing vertical lines...")
for i in range(num_lines):
    # Calculate the ideal center position of the line
    center_x = (i + 1) * center_spacing

    # Calculate the start (left edge) and end (right edge) pixel coordinates
    start_x = round(center_x - target_line_width / 2)
    start_x = max(0, start_x) # Ensure start_x is not negative

    end_x = start_x + target_line_width - 1
    end_x = min(image_size - 1, end_x) # Ensure end_x does not exceed image bounds

    # Adjust start_x again if end_x was capped
    actual_width = end_x - start_x + 1
    if actual_width < target_line_width and start_x > 0:
        start_x = max(0, end_x - target_line_width + 1)

    # Define the rectangle for the vertical line
    if end_x >= start_x:
        line_rect = [start_x, 0, end_x, image_size - 1]
        draw.rectangle(line_rect, fill=line_color)
        # print(f"  V-Line {i+1}: Center={center_x:.2f}, Start={start_x}, End={end_x}") # Uncomment for debug
    else:
        print(f"  Skipping V-Line {i+1} near edge (calculated width < 1)")

# --- Draw Horizontal Lines ---
print("Drawing horizontal lines...")
for i in range(num_lines):
    # Calculate the ideal center position of the line
    center_y = (i + 1) * center_spacing

    # Calculate the start (top edge) and end (bottom edge) pixel coordinates
    start_y = round(center_y - target_line_width / 2)
    start_y = max(0, start_y) # Ensure start_y is not negative

    end_y = start_y + target_line_width - 1
    end_y = min(image_size - 1, end_y) # Ensure end_y does not exceed image bounds

    # Adjust start_y again if end_y was capped
    actual_width = end_y - start_y + 1
    if actual_width < target_line_width and start_y > 0:
        start_y = max(0, end_y - target_line_width + 1)

    # Define the rectangle for the horizontal line
    if end_y >= start_y:
        line_rect = [0, start_y, image_size - 1, end_y]
        draw.rectangle(line_rect, fill=line_color)
        # print(f"  H-Line {i+1}: Center={center_y:.2f}, Start={start_y}, End={end_y}") # Uncomment for debug
    else:
        print(f"  Skipping H-Line {i+1} near edge (calculated width < 1)")

# --- Save the Image ---
try:
    img.save(output_filename)
    print(f"Image saved successfully as '{output_filename}'")
except Exception as e:
    print(f"Error saving image: {e}")

# --- (Optional) Display the image ---
# Uncomment the following lines if you want to display the image directly
# try:
#     img.show()
# except Exception as e:
#     print(f"Could not display image automatically: {e}")

