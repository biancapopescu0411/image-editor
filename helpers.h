// Popescu Maria Bianca 325CA

#ifndef MATRIX_H
#define MATRIX_H

int **alloc_matrix(int width, int height);
void free_matrix(int **matrix, int rows);
void swap(int *a, int *b);
void ignore_comments(FILE *file);
bool is_ascii(char *magic_number);
void load_ascii(FILE *file, image *img, int width, int height,
				char *magic_number);
void load_binary(FILE *file, image *img, int width, int height,
				 char *magic_number);
void apply_sharpen(image *img);
void apply_edge(image *img);
void apply_blur(image *img);
void apply_gaussian_blur(image *img);

#endif
