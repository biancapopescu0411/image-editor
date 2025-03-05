// Popescu Maria Bianca 325CA

#ifndef STRUCT_H
#define STRUCT_H

typedef struct {
	int width;
	int height;
	int maxval;
	int x1, y1, x2, y2;
	int **picture;
	bool already_loaded;
	int **red, **green, **blue;
	bool color;
} image;

#endif
