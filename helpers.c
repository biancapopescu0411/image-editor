// Popescu Maria Bianca 325CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include "struct.h"
#include "helpers.h"
#include "alloc.h"

void swap(int *a, int *b)
{
	int aux = *a;
	*a = *b;
	*b = aux;
}

void ignore_comments(FILE *file)
{
	// read the first character from the file
	char c = fgetc(file);

	// continue reading lines while '#' character is encountered (comments)
	while (c == '#') {
		// continue reading characters until the end of the line
		while (c != '\n')
			c = fgetc(file);

		// read the next character after the newline
		c = fgetc(file);
	}

	// move the file pointer one position back to the last non-comment character
	fseek(file, -1, SEEK_CUR);
}

bool is_ascii(char *magic_number)
{
	if (strcmp(magic_number, "P1") == 0 || strcmp(magic_number, "P2") == 0 ||
		strcmp(magic_number, "P3") == 0) {
		return true;
	}

	return false;
}

void load_ascii(FILE *file, image *img, int width, int height,
				char *magic_number)
{
	// set image properties based on the loaded image type and dimensions
	img->already_loaded = true;
	img->height = height;
	img->width = width;

	img->x1 = 0;
	img->y1 = 0;
	img->x2 = width;
	img->y2 = height;

	// check the magic number to determine the image type
	if (strcmp(magic_number, "P2") == 0) {
		// for PBM (grayscale) image
		img->color = false;

		// allocate memory for the grayscale image matrix
		img->picture = alloc_matrix(width, height);

		// read pixel values from the file into the grayscale matrix
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++)
				fscanf(file, "%d", &img->picture[i][j]);
		}
	} else if (strcmp(magic_number, "P3") == 0) {
		// for PPM (color) image
		img->color = true;

		// allocate memory for the color channels (red, green, blue) matrices
		img->red = alloc_matrix(width, height);
		img->green = alloc_matrix(width, height);
		img->blue = alloc_matrix(width, height);

		// read color values from the file into the respective color channels
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				fscanf(file, "%d", &img->red[i][j]);
				fscanf(file, "%d", &img->green[i][j]);
				fscanf(file, "%d", &img->blue[i][j]);
			}
		}
	}
}

void load_binary(FILE *file, image *img, int width, int height,
				 char *magic_number)
{
	// set image properties based on the loaded image type and dimensions
	img->already_loaded = true;
	img->height = height;
	img->width = width;

	img->x1 = 0;
	img->y1 = 0;
	img->x2 = width;
	img->y2 = height;

	// check the magic number to determine the image type
	if (strcmp(magic_number, "P5") == 0) {
		// for PGM (grayscale) image
		img->color = false;

		// allocate memory for the grayscale image matrix
		img->picture = alloc_matrix(width, height);

		// read pixel values from the file into the grayscale matrix
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				unsigned char pixel;
				fread(&pixel, sizeof(unsigned char), 1, file);

				img->picture[i][j] = pixel;
			}
		}
	} else if (strcmp(magic_number, "P6") == 0) {
		// for PPM (color) image
		img->color = true;

		// allocate memory for the color channels (red, green, blue) matrices
		img->red = alloc_matrix(width, height);
		img->green = alloc_matrix(width, height);
		img->blue = alloc_matrix(width, height);

		// read color values from the file into the respective color channels
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				unsigned char pixel;

				// read and store red channel value
				fread(&pixel, sizeof(unsigned char), 1, file);
				img->red[i][j] = pixel;

				// read and store green channel value
				fread(&pixel, sizeof(unsigned char), 1, file);
				img->green[i][j] = pixel;

				// read and store blue channel value
				fread(&pixel, sizeof(unsigned char), 1, file);
				img->blue[i][j] = pixel;
			}
		}
	}
}

void apply_sharpen(image *img)
{
	int **copy_red = alloc_matrix(img->width, img->height);
	int **copy_green = alloc_matrix(img->width, img->height);
	int **copy_blue = alloc_matrix(img->width, img->height);

	// copy red, green, blue
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			copy_red[i][j] = img->red[i][j];
			copy_green[i][j] = img->green[i][j];
			copy_blue[i][j] = img->blue[i][j];
		}
	}

	int kernel[3][3] = {
		{0, -1, 0},
		{-1, 5, -1},
		{0, -1, 0}
	};

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (i == 0 || i == img->height - 1 || j == 0 ||
				j == img->width - 1) {
				continue;
			}

			int sum_red = 0;
			int sum_green = 0;
			int sum_blue = 0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					int x = j + l - 1;
					int y = i + k - 1;

					if (x < 0 || x >= img->width || y < 0 || y >= img->height)
						continue;

					sum_red += img->red[y][x] * kernel[k][l];
					sum_green += img->green[y][x] * kernel[k][l];
					sum_blue += img->blue[y][x] * kernel[k][l];
				}
			}

			if (sum_red > 255)
				sum_red = 255;

			if (sum_green > 255)
				sum_green = 255;

			if (sum_blue > 255)
				sum_blue = 255;

			if (sum_red < 0)
				sum_red = 0;

			if (sum_green < 0)
				sum_green = 0;

			if (sum_blue < 0)
				sum_blue = 0;

			copy_red[i][j] = sum_red;
			copy_green[i][j] = sum_green;
			copy_blue[i][j] = sum_blue;
		}
	}

	free_matrix(img->red, img->height);
	free_matrix(img->green, img->height);
	free_matrix(img->blue, img->height);

	img->red = copy_red;
	img->green = copy_green;
	img->blue = copy_blue;
}

void apply_edge(image *img)
{
	int **copy_red = alloc_matrix(img->width, img->height);
	int **copy_green = alloc_matrix(img->width, img->height);
	int **copy_blue = alloc_matrix(img->width, img->height);

	// copy red, green, blue
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			copy_red[i][j] = img->red[i][j];
			copy_green[i][j] = img->green[i][j];
			copy_blue[i][j] = img->blue[i][j];
		}
	}

	int kernel[3][3] = {
		{-1, -1, -1},
		{-1, 8, -1},
		{-1, -1, -1}
	};

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (i == 0 || i == img->height - 1 || j == 0 ||
				j == img->width - 1) {
				continue;
			}

			int sum_red = 0;
			int sum_green = 0;
			int sum_blue = 0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					int x = j + l - 1;
					int y = i + k - 1;

					if (x < 0 || x >= img->width || y < 0 ||
						y >= img->height)
						continue;

					sum_red += img->red[y][x] * kernel[k][l];
					sum_green += img->green[y][x] * kernel[k][l];
					sum_blue += img->blue[y][x] * kernel[k][l];
				}
			}

			if (sum_red > 255)
				sum_red = 255;

			if (sum_green > 255)
				sum_green = 255;

			if (sum_blue > 255)
				sum_blue = 255;

			if (sum_red < 0)
				sum_red = 0;

			if (sum_green < 0)
				sum_green = 0;

			if (sum_blue < 0)
				sum_blue = 0;

			copy_red[i][j] = sum_red;
			copy_green[i][j] = sum_green;
			copy_blue[i][j] = sum_blue;
		}
	}

	free_matrix(img->red, img->height);
	free_matrix(img->green, img->height);
	free_matrix(img->blue, img->height);

	img->red = copy_red;
	img->green = copy_green;
	img->blue = copy_blue;
}

void apply_blur(image *img)
{
	int **copy_red = alloc_matrix(img->width, img->height);
	int **copy_green = alloc_matrix(img->width, img->height);
	int **copy_blue = alloc_matrix(img->width, img->height);

	// copy red, green, blue
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			copy_red[i][j] = img->red[i][j];
			copy_green[i][j] = img->green[i][j];
			copy_blue[i][j] = img->blue[i][j];
		}
	}

	int kernel[3][3] = {
		{1, 1, 1},
		{1, 1, 1},
		{1, 1, 1}
	};

	double multiplier = 1.0 / 9.0;

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (i == 0 || i == img->height - 1 || j == 0 ||
				j == img->width - 1) {
				continue;
			}

			double sum_red = 0.0;
			double sum_green = 0.0;
			double sum_blue = 0.0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					int x = j + l - 1;
					int y = i + k - 1;

					if (x < 0 || x >= img->width || y < 0 ||
						y >= img->height) {
						continue;
					}

					sum_red += img->red[y][x] * kernel[k][l] * multiplier;
					sum_green += img->green[y][x] * kernel[k][l] * multiplier;
					sum_blue += img->blue[y][x] * kernel[k][l] * multiplier;
				}
			}

			if (sum_red > 255.0)
				sum_red = 255.0;

			if (sum_green > 255.0)
				sum_green = 255.0;

			if (sum_blue > 255.0)
				sum_blue = 255.0;

			if (sum_red < 0.0)
				sum_red = 0.0;

			if (sum_green < 0.0)
				sum_green = 0.0;

			if (sum_blue < 0.0)
				sum_blue = 0.0;

			copy_red[i][j] = round(sum_red);
			copy_green[i][j] = round(sum_green);
			copy_blue[i][j] = round(sum_blue);
		}
	}

	free_matrix(img->red, img->height);
	free_matrix(img->green, img->height);
	free_matrix(img->blue, img->height);

	img->red = copy_red;
	img->green = copy_green;
	img->blue = copy_blue;
}

void apply_gaussian_blur(image *img)
{
	int **copy_red = alloc_matrix(img->width, img->height);
	int **copy_green = alloc_matrix(img->width, img->height);
	int **copy_blue = alloc_matrix(img->width, img->height);

	// copy red, green, blue
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			copy_red[i][j] = img->red[i][j];
			copy_green[i][j] = img->green[i][j];
			copy_blue[i][j] = img->blue[i][j];
		}
	}

	int kernel[3][3] = {
		{1, 2, 1},
		{2, 4, 2},
		{1, 2, 1}
	};

	double multiplier = 1.0 / 16.0;

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			if (i == 0 || i == img->height - 1 || j == 0 ||
				j == img->width - 1)
				continue;

			double sum_red = 0.0;
			double sum_green = 0.0;
			double sum_blue = 0.0;

			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					int x = j + l - 1;
					int y = i + k - 1;

					if (x < 0 || x >= img->width || y < 0 ||
						y >= img->height)
						continue;

					sum_red += img->red[y][x] * kernel[k][l] * multiplier;
					sum_green += img->green[y][x] * kernel[k][l] * multiplier;
					sum_blue += img->blue[y][x] * kernel[k][l] * multiplier;
				}
			}

			if (sum_red > 255.0)
				sum_red = 255.0;

			if (sum_green > 255.0)
				sum_green = 255.0;

			if (sum_blue > 255.0)
				sum_blue = 255.0;

			if (sum_red < 0.0)
				sum_red = 0.0;

			if (sum_green < 0.0)
				sum_green = 0.0;

			if (sum_blue < 0.0)
				sum_blue = 0.0;

			copy_red[i][j] = round(sum_red);
			copy_green[i][j] = round(sum_green);
			copy_blue[i][j] = round(sum_blue);
		}
	}

	free_matrix(img->red, img->height);
	free_matrix(img->green, img->height);
	free_matrix(img->blue, img->height);

	img->red = copy_red;
	img->green = copy_green;
	img->blue = copy_blue;
}
