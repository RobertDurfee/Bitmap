# Bitmap
C++ class for bitmap file creating.

### Disclaimer
This is not production-worthy code! View this simply as a proof-of-concept.

### Initialization
```
Bitmap(BitmapFileHeader fileHeader, BitmapImageHeader imageHeader, unsigned char * pixels);
Bitmap(BitmapFileHeader fileHeader, BitmapImageHeader imageHeader);
Bitmap(int width, int height, unsigned char * pixels);
Bitmap(int width, int height);
Bitmap(char * filename);
```
The `Bitmap` class can be initialized through the five different constructors:

1. Providing `Bitmap` file header structures with the supplied pixels' RGB values.
2. Providing `Bitmap` file header structures.
3. Specifying `width` and `height` of the supplied pixels' RGB values.
4. Specifying `width` and `height`.
5. Specifying the `Bitmap` file location.

Additional required file information, such as pixel allocation and `Bitmap` file header fields, will be generated by the constructors upon execution.

### Opening
```
void Open(char * filename);
```
If the user chooses to pick a new `Bitmap` file to load, the `Open` method can be called to load a new `Bitmap` file. All the neccesary information about the `Bitmap` file will be generated upon execution.

### Saving
```
void Save(char * filename);
```
The user can call the `Save` method to create a viewable `Bitmap` file. This file can be viewed by any major picture software.

### Setters/Getters
```
unsigned char * GetPixels(int * width, int * height);
void SetPixels(int width, int height, unsigned char * pixels);
void SetPixels(unsigned char * pixels);
```
These are pretty self-explanatory methods that allow the user to set and get important parts of the `Bitmap` file. The pixels should be specified as stringed RGB values each from `0-255`.

### Deinitialization
```
~Bitmap();
```
The allocated pixels will be deallocated by the destructor.
