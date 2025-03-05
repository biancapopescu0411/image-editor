// Popescu Maria Bianca 325CA

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "struct.h"
#include "helpers.h"
#include "alloc.h"
#include "commands.h"

void load(const char *filename, image *img)
{
	// if the image is already loaded, free the memory
	if (img->already_loaded) {
		if (img->color) {
			free_matrix(img->red, img->height);
			free_matrix(img->green, img->height);
			free_matrix(img->blue, img->height);
		} else {
			free_matrix(img->picture, img->height);
		}

		img->already_loaded = false;
	}

	// open the file
	FILE * file = fopen(filename, "rt");
	if (!file) {
		printf("Failed to load %s\n", filename);

		return;
	}

	// read the magic number
	char magic_number[3];
	fscanf(file, "%s", magic_number);

	// read the width and height
	int width, height, maxval;
	fscanf(file, "%d %d", &width, &height);

	// ignore comments
	ignore_comments(file);

	img->width = width;
	img->height = height;

	img->x1 = 0;
	img->x2 = width;
	img->y1 = 0;
	img->y2 = height;

	// verify if the image is black and white
	if (strcmp(magic_number, "P1") == 0 || strcmp(magic_number, "P4") == 0) {
		img->maxval = 1;
	} else {
		// else, the image is color so we need to read the maxval
		fscanf(file, "%d", &maxval);
		img->maxval = maxval;
	}

	// verify if the magic number is ascii or binary
	bool ascii = is_ascii(magic_number);

	if (ascii) {
		load_ascii(file, img, width, height, magic_number);
	} else {
		char buf[1000];
		fgets(buf, 1000, file);

		int pos = ftell(file);

		fclose(file);
		file = fopen(filename, "rb");

		fseek(file, pos, SEEK_SET);
		load_binary(file, img, width, height, magic_number);
	}

	fclose(file);

	printf("Loaded %s\n", filename);
}

void select(image *img, int x1, int y1, int x2, int y2)
{
	// verify if the image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// verify if the coordinates are valid
	if (x1 < 0 || x2 > img->width || y1 < 0 || y2 > img->height) {
		printf("Invalid set of coordinates\n");
		return;
	}

	if (x1 == x2 || y1 == y2) {
		printf("Invalid set of coordinates\n");
		return;
	}

	// swap the coordinates if needed
	if (x1 > x2)
		swap(&x1, &x2);

	if (y1 > y2)
		swap(&y1, &y2);

	// set the coordinates
	img->x1 = x1;
	img->x2 = x2;
	img->y1 = y1;
	img->y2 = y2;

	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
}

void select_all(image *img)
{
	// verify if the image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// select the entire image
	img->x1 = 0;
	img->x2 = img->width;
	img->y1 = 0;
	img->y2 = img->height;

	// success message
	printf("Selected ALL\n");
}

void exit_program(image *img)
{
	if (img->already_loaded) {
		if (img->color) {
			free_matrix(img->red, img->height);
			free_matrix(img->green, img->height);
			free_matrix(img->blue, img->height);
		} else {
			free_matrix(img->picture, img->height);
		}
	} else {
		printf("No image loaded\n");
	}

	free(img);
}

void crop(image *img)
{
	// verify if the image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// calculate the new width and height
	int width = img->x2 - img->x1;
	int height = img->y2 - img->y1;

	// if the image is rgb, we need to allocate new matrices for the colors
	if (img->color) {
		int **red = alloc_matrix(width, height);
		int **green = alloc_matrix(width, height);
		int **blue = alloc_matrix(width, height);

		// copy the pixels from the selected area to the new matrices
		for (int i = img->y1; i < img->y2; i++) {
			for (int j = img->x1; j < img->x2; j++) {
				red[i - img->y1][j - img->x1] = img->red[i][j];
				green[i - img->y1][j - img->x1] = img->green[i][j];
				blue[i - img->y1][j - img->x1] = img->blue[i][j];
			}
		}

		// free the old matrices
		free_matrix(img->red, img->height);
		free_matrix(img->green, img->height);
		free_matrix(img->blue, img->height);

		// update the image structure with the new cropped image
		img->red = red;
		img->green = green;
		img->blue = blue;
	} else {
		int **picture = alloc_matrix(width, height);

		for (int i = img->y1; i < img->y2; i++) {
			for (int j = img->x1; j < img->x2; j++)
				picture[i - img->y1][j - img->x1] = img->picture[i][j];
		}

		free_matrix(img->picture, img->height);

		img->picture = picture;
	}

	img->width = width;
	img->height = height;

	img->x1 = 0;
	img->x2 = width;
	img->y1 = 0;
	img->y2 = height;

	printf("Image cropped\n");
}

void save_ascii(const char *filename, image *img)
{
	// verify if an image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// open the file for writing in text mode
	FILE * file = fopen(filename, "wt");
	if (!file) {
		// print an error message if the file cannot be opened
		printf("Failed to save %s\n", filename);
		return;
	}

	// write the PBM or PPM header information to the file
	if (img->color) {
		// for color images (PPM format)
		fprintf(file, "P3\n");
		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		// write pixel values for each color channel to the file
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				fprintf(file, "%d ", img->red[i][j]);
				fprintf(file, "%d ", img->green[i][j]);
				fprintf(file, "%d ", img->blue[i][j]);
			}

			// start a new line for the next row of pixels
			fprintf(file, "\n");
		}
	} else {
		// for grayscale images (PBM format)
		fprintf(file, "P2\n");
		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		// rrite pixel values to the file
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++)
				fprintf(file, "%d ", img->picture[i][j]);

			// start a new line for the next row of pixels
			fprintf(file, "\n");
		}
	}

	// close the file
	fclose(file);

	// print a message indicating that the image has been saved
	printf("Saved %s\n", filename);
}

void save_binary(const char *filename, image *img)
{
	// verify if an image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// open the file for writing in text mode
	FILE * file = fopen(filename, "wt");
	if (!file) {
		// print an error message if the file cannot be opened
		printf("Failed to save %s\n", filename);
		return;
	}

	// write the PPM or PGM header information to the file
	if (img->color) {
		// for color images (PPM format)
		fprintf(file, "P6\n");
		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		// close the file and reopen it in binary append mode
		fclose(file);
		file = fopen(filename, "ab");
		if (!file) {
			// print an error message if reopening fails
			printf("Failed to save %s\n", filename);
			return;
		}

		// write pixel values for each color channel to the file
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				// write red, green, and blue channels for each pixel
				unsigned char pixel = img->red[i][j];
				fwrite(&pixel, sizeof(unsigned char), 1, file);

				pixel = img->green[i][j];
				fwrite(&pixel, sizeof(unsigned char), 1, file);

				pixel = img->blue[i][j];
				fwrite(&pixel, sizeof(unsigned char), 1, file);
			}
		}
	} else {
		// for grayscale images (PGM format)
		fprintf(file, "P5\n");
		fprintf(file, "%d %d\n", img->width, img->height);
		fprintf(file, "%d\n", img->maxval);

		// close the file and reopen it in binary append mode
		fclose(file);
		file = fopen(filename, "ab");
		if (!file) {
			// print an error message if reopening fails
			printf("Failed to save %s\n", filename);
			return;
		}

		// write pixel values to the file
		for (int i = 0; i < img->height; i++) {
			for (int j = 0; j < img->width; j++) {
				// write pixel values consecutively for each pixel
				unsigned char pixel = img->picture[i][j];
				fwrite(&pixel, sizeof(unsigned char), 1, file);
			}
		}
	}

	// close the file
	fclose(file);

	// print a message indicating that the image has been saved
	printf("Saved %s\n", filename);
}

void apply_filter(image *img, char *filter)
{
	// verify if an image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// check if a filter command is provided
	if (!filter) {
		printf("Invalid command\n");
		return;
	}

	// check if the image is a color image (RGB)
	if (!img->color) {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	// check and apply different filters based on the provided filter command
	if (strstr(filter, "SHARPEN")) {
		// apply the sharpen filter
		apply_sharpen(img);

		// print a message indicating that the sharpen filter has been applied
		printf("APPLY SHARPEN done\n");
	} else if (strstr(filter, "EDGE")) {
		// apply the edge detection filter
		apply_edge(img);

		printf("APPLY EDGE done\n");
	} else if (strstr(filter, "BLUR") && strstr(filter, "GAUSSIAN_BLUR") ==
			   NULL) {
		// apply the blur filter
		apply_blur(img);

		// print a message indicating that the blur filter has been applied
		printf("APPLY BLUR done\n");
	} else if (strstr(filter, "GAUSSIAN_BLUR")) {
		// apply the Gaussian blur filter
		apply_gaussian_blur(img);

		printf("APPLY GAUSSIAN_BLUR done\n");
	} else {
		// print a message if the provided filter command is invalid
		printf("APPLY parameter invalid\n");
	}
}

void equalize(image *img)
{
	// verify if an image is already loaded
	if (!img->already_loaded) {
		printf("No image loaded\n");
		return;
	}

	// check if the image is a color image (RGB)
	if (img->color) {
		printf("Black and white image needed\n");
		return;
	}

	// initialize an array to store the frequency of each intensity level
	int freq[256] = {0};

	// calculate the frequency of each intensity level in the image
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++)
			freq[img->picture[i][j]]++;
	}

	// perform histogram equalization on the image
	for (int i = 0; i < img->height; i++) {
		for (int j = 0; j < img->width; j++) {
			int sum = 0;

			// accumulate the frequencies up to the current intensity level
			for (int k = 0; k <= img->picture[i][j]; k++)
				sum += freq[k];

			// calculate the equalized value for the current pixel
			double value = 255.0 * (double)sum / (img->width * img->height);

			// ensure the equalized value is within the valid intensity range
			if (value > 255.0)
				value = 255.0;

			if (value < 0.0)
				value = 0.0;

			// update the pixel value with the equalized value
			img->picture[i][j] = round(value);
		}
	}

	// print a message indicating that the equalization process is done
	printf("Equalize done\n");
}
