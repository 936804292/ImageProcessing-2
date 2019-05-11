# 103_ImageProcessing2
Image processing for image detection using connected components, BFS, and a queue.

Command line arguments:
The function takes a pointer to 3D array, which contains the image. A pointer to a 2D array to which
the gray-scale version of the image will be placed. Also, it take the height and the width of the array.

./label gray coins.bmp coins_gray.bmp //creates a gray-scale image


./label binary coins.bmp coins_binary.bmp //creates a binary image


./label segment coins.bmp output.bmp //creates a labeled image with the colors
