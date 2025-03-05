// Popescu Maria Bianca 325CA

#ifndef COMMANDS_H
#define COMMANDS_H

void load(const char *filename, image *img);
void select(image *img, int x1, int y1, int x2, int y2);
void select_all(image *img);
void exit_program(image *img);
void crop(image *img);
void save_ascii(const char *filename, image *img);
void save_binary(const char *filename, image *img);
void apply_filter(image *img, char *filter);
void equalize(image *img);

#endif
