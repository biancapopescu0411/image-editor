// Popescu Maria Bianca 325CA

#include <stdio.h>
#include <stdlib.h>
#include "alloc.h"

int **alloc_matrix(int width, int height)
{
	int **matrix = malloc(sizeof(int *) * height);
	if (!matrix) {
		printf("Failed to allocate memory for image\n");
		return NULL;
	}

	for (int i = 0; i < height; i++) {
		matrix[i] = malloc(sizeof(int) * width);
		if (!matrix[i]) {
			printf("Failed to allocate memory for image\n");
			return NULL;
		}
	}

	return matrix;
}

void free_matrix(int **matrix, int rows)
{
	for (int i = 0; i < rows; i++)
		free(matrix[i]);

	free(matrix);
}
