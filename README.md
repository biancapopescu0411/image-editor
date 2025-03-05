# Image Editor

## Overview
This project is a simple command-line image editor that supports loading, selecting, cropping, applying filters, and saving images in ASCII or binary formats. The editor works with PGM and PPM image formats and provides various image processing functionalities.

## Features
- **Load** images in PGM (grayscale) and PPM (color) formats.
- **Select** specific regions of the image or the entire image.
- **Crop** the image based on the selected region.
- **Apply various filters** including:
  - Sharpen
  - Edge detection
  - Blur
  - Gaussian blur
- **Equalize** grayscale images.
- **Save** images in ASCII (`P2`, `P3`) or binary (`P5`, `P6`) formats.
- **Memory management** for efficient image processing.

## File Structure
```
├── alloc.c
├── alloc.h
├── commands.c
├── commands.h
├── helpers.c
├── helpers.h
├── image_editor.c
├── struct.h
├── Makefile
```

## Example Commands
```
LOAD image.pgm
SELECT 10 10 100 100
APPLY SHARPEN
CROP
SAVE output.pgm ascii
EXIT
```
## Commands

| Command                  | Description                                        |
|--------------------------|----------------------------------------------------|
| `LOAD <file>`            | Load an image from the specified file.            |
| `SELECT x1 y1 x2 y2`     | Select a rectangular region of the image.         |
| `SELECT ALL`             | Select the entire image.                          |
| `CROP`                   | Crop the image to the selected area.              |
| `APPLY <filter>`         | Apply a filter (`SHARPEN`, `EDGE`, `BLUR`, `GAUSSIAN_BLUR`). |
| `EQUALIZE`               | Apply histogram equalization to a grayscale image. |
| `SAVE <file> ascii/binary` | Save the image in ASCII or binary format.       |
| `EXIT`                   | Exit the program.                                 |
