// Popescu Maria Bianca 325CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "struct.h"
#include "helpers.h"
#include "commands.h"
#include "alloc.h"

int main(void)
{
	// allocate memory for the image
	image *img = malloc(sizeof(image));
	// check if memory allocation was successful
	if (!img) {
		printf("Failed to allocate memory for image\n");
		return 0;
	}

	img->already_loaded = false;

	// read commands from stdin
	char line[1000];
	while (fgets(line, 1000, stdin)) {
		// copy the line because strtok modifies the string
		char line_copy[1000];

		strcpy(line_copy, line);
		// remove the newline character
		line_copy[strlen(line_copy) - 1] = '\0';

		// get the command
		char *command = strtok(line, "\n ");

		// check which command was given and call the corresponding function
		if (strcmp(command, "LOAD") == 0) {
			char *filename = strtok(NULL, "\n ");

			load(filename, img);
		} else if (strcmp(line_copy, "SELECT ALL") == 0) {
			select_all(img);
		} else if (strcmp(command, "SELECT") == 0) {
			char *x1 = strtok(NULL, "\n ");
			char *y1 = strtok(NULL, "\n ");
			char *x2 = strtok(NULL, "\n ");
			char *y2 = strtok(NULL, "\n ");

			select(img, atoi(x1), atoi(y1), atoi(x2), atoi(y2));
		} else if (strcmp(command, "CROP") == 0) {
			crop(img);
		} else if (strcmp(command, "SAVE") == 0) {
			if (strstr(line_copy, "ascii")) {
				char *filename = strtok(NULL, "\n ");

				save_ascii(filename, img);
			} else {
				char *filename = strtok(NULL, "\n ");

				save_binary(filename, img);
			}
		} else if (strcmp(command, "APPLY") == 0) {
			char *filter = strtok(NULL, "\n ");

			apply_filter(img, filter);
		} else if (strcmp(command, "EXIT") == 0) {
			exit_program(img);

			break;
		} else if (strcmp(command, "EQUALIZE") == 0) {
			equalize(img);
		} else {
			// if the command is invalid, print an error message
			printf("Invalid command\n");
		}
	}

	return 0;
}

