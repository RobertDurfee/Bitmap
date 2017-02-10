#ifndef BITMAP_HEADER
#define BITMAP_HEADER

#include <fstream>		//ifstream, ofstream

struct BITMAP_FILE_HEADER
{
	unsigned short Type;
	unsigned long Size;
	unsigned long Reserved;
	unsigned long Offset;
};

struct BITMAP_IMAGE_HEADER
{
	unsigned long Size;
	unsigned long Width;
	unsigned long Height;
	unsigned short Planes;
	unsigned short BitCount;
	unsigned long Compression;
	unsigned long ImageSize;
	unsigned long WidthPixelsPerMeter;
	unsigned long HeightPixelsPerMeter;
	unsigned long pixelsUsed;
	unsigned long Importantpixels;
};

#define SIZE_OF_HEADERS		0x36
#define PADDING_CONSTANT	0xCC

class Bitmap
{
public:
	Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader, unsigned char * pixels);
	Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader);
	Bitmap(int width, int height, unsigned char * pixels);
	Bitmap(int width, int height);
	Bitmap(char * filename);

	~Bitmap();

	void Open(char * filename);
	void Save(char * filename);

	unsigned char * GetPixels(int * width, int * height);
	void SetPixels(int width, int height, unsigned char * pixels);
	void SetPixels(unsigned char * pixels);

private:
	unsigned char * PixelData;
	int Padding;
	BITMAP_FILE_HEADER FileHeader;
	BITMAP_IMAGE_HEADER ImageHeader;

	void SetHeaders(int width, int height);
	void SetHeaders(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader);
	void SetHeaders(char * headers);
	char * GetHeaders();

	void SetPadding();
};
Bitmap::Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader)
{
	SetHeaders(fileHeader, imageHeader);
	
	PixelData = (unsigned char *)malloc(ImageHeader.ImageSize);
}
Bitmap::Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader, unsigned char * pixels)
{
	SetHeaders(fileHeader, imageHeader);
	
	PixelData = (unsigned char *)malloc(ImageHeader.ImageSize);

	SetPixels(pixels);
}
Bitmap::Bitmap(int width, int height, unsigned char * pixels)
{
	SetHeaders(width, height);

	PixelData = (unsigned char *)malloc(ImageHeader.ImageSize);

	SetPixels(pixels);
}
Bitmap::Bitmap(int width, int height)
{
	SetHeaders(width, height);

	PixelData = (unsigned char *)malloc(ImageHeader.ImageSize);
}
Bitmap::Bitmap(char * filename)
{
	Open(filename);
}

Bitmap::~Bitmap()
{
	free(PixelData);
}

void Bitmap::Open(char * filename)
{	
	std::ifstream file(filename, std::ios::binary);

	char * headers = (char *)malloc(SIZE_OF_HEADERS);
	file.read(headers, SIZE_OF_HEADERS);
	SetHeaders(headers);
	free(headers);

	if (PixelData)
		realloc(PixelData, ImageHeader.ImageSize);
	else
		PixelData = (char *)malloc(ImageHeader.ImageSize);

	file.read((char *)PixelData, ImageHeader.ImageSize);

	file.close();
}
void Bitmap::Save(char * filename)
{
	std::ofstream file(filename, std::ios::binary);

	char * headers = GetHeaders();
	file.write(headers, SIZE_OF_HEADERS);
	free(headers);

	file.write((char *)PixelData, ImageHeader.ImageSize);

	file.close();
}

unsigned char * Bitmap::GetPixels(int * width, int * height)
{
	*width = ImageHeader.Width;
	*height = ImageHeader.Height;

	unsigned char * pixels = (unsigned char *)malloc(ImageHeader.Width * ImageHeader.Height * 3);

	for (int y = 0; y < *height; y++) //Each Row
		for (int x = 0; x < *width; x++) //Each Column
			for (int i = 0; i < 3; i++) //Each RGB
				pixels[y * (*width * 3) + (x * 3) + i] /*Doesn't have padding*/ = PixelData[y * (*width * 3 + Padding) + (x * 3) + i] /*Has padding*/;

	return pixels;
}
void Bitmap::SetPixels(int width, int height, unsigned char * pixels)
{
	//Update headers if dimensions changed
	if (width != ImageHeader.Width || height != ImageHeader.Height)
	{
		SetHeaders(width, height);
		realloc(PixelData, ImageHeader.ImageSize);
	}

	for (int y = 0; y < height; y++) //Each Row
	{
		for (int x = 0; x < width; x++) //Each Column
			for (int i = 0; i < 3; i++) //Each RGB
				PixelData[y * (width * 3 + Padding) + (x * 3) + i] /*Has padding*/ = pixels[y * (width * 3) + (x * 3) + i] /*Doesn't have padding*/;

		for (int i = 0; i < Padding; i++) //Set Padding
			PixelData[y * (width * 3 + Padding) + (width * 3) + i] = PADDING_CONSTANT;
	}
}
void Bitmap::SetPixels(unsigned char * pixels)
{
	SetPixels(ImageHeader.Width, ImageHeader.Height, pixels);
}

void Bitmap::SetHeaders(int width, int height)
{
	SetPadding(width);

	FileHeader.Type = 0x4D42; //"BM"
	FileHeader.Reserved = 0x00000000;
	FileHeader.Offset = 54;

	ImageHeader.Size = 40;
	ImageHeader.Width = width;
	ImageHeader.Height = height;
	ImageHeader.Planes = 1;
	ImageHeader.BitCount = 24;
	ImageHeader.Compression = 0;
	ImageHeader.ImageSize = (width * height * 3) + Padding * height;
	ImageHeader.WidthPixelsPerMeter = 0;
	ImageHeader.HeightPixelsPerMeter = 0;
	ImageHeader.pixelsUsed = 0;
	ImageHeader.Importantpixels = 0;

	FileHeader.Size = ImageHeader.ImageSize + 54;
}
void Bitmap::SetHeaders(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader)
{
	FileHeader = fileHeader;
	ImageHeader = imageHeader;

	SetPadding(ImageHeader.Width);
}
void Bitmap::SetHeaders(char * headers)
{
	int index = 0;

	//memcpy() is not used due to compiler-specfic structure padding
	FileHeader.Type = *(unsigned short*)&headers[index];					index += sizeof(unsigned short);
	FileHeader.Size = *(unsigned long*)&headers[index];						index += sizeof(unsigned long);
	FileHeader.Reserved = *(unsigned long*)&headers[index];					index += sizeof(unsigned long);
	FileHeader.Offset = *(unsigned long*)&headers[index];					index += sizeof(unsigned long);

	ImageHeader.Size = *(unsigned long*)&headers[index];					index += sizeof(unsigned long);
	ImageHeader.Width = *(unsigned long*)&headers[index];					index += sizeof(unsigned long);
	ImageHeader.Height = *(unsigned long*)&headers[index];					index += sizeof(unsigned long);
	ImageHeader.Planes = *(unsigned short*)&headers[index];					index += sizeof(unsigned short);
	ImageHeader.BitCount = *(unsigned short*)&headers[index];				index += sizeof(unsigned short);
	ImageHeader.Compression = *(unsigned long*)&headers[index];				index += sizeof(unsigned long);
	ImageHeader.ImageSize = *(unsigned long*)&headers[index];				index += sizeof(unsigned long);
	ImageHeader.WidthPixelsPerMeter = *(unsigned long*)&headers[index];		index += sizeof(unsigned long);
	ImageHeader.HeightPixelsPerMeter = *(unsigned long*)&headers[index];	index += sizeof(unsigned long);
	ImageHeader.pixelsUsed = *(unsigned long*)&headers[index];				index += sizeof(unsigned long);
	ImageHeader.Importantpixels = *(unsigned long*)&headers[index];

	SetPadding(ImageHeader.Width);
}
char * Bitmap::GetHeaders()
{
	char * headers = (char *)malloc(SIZE_OF_HEADERS);

	int index = 0;

	//memcpy() is not used due to compiler-specfic structure padding
	*(unsigned short*)&headers[index] = FileHeader.Type;					index += sizeof(unsigned short);
	*(unsigned long*)&headers[index] = FileHeader.Size;						index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = FileHeader.Reserved;					index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = FileHeader.Offset;					index += sizeof(unsigned long);

	*(unsigned long*)&headers[index] = ImageHeader.Size;					index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.Width;					index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.Height;					index += sizeof(unsigned long);
	*(unsigned short*)&headers[index] = ImageHeader.Planes;					index += sizeof(unsigned short);
	*(unsigned short*)&headers[index] = ImageHeader.BitCount;				index += sizeof(unsigned short);
	*(unsigned long*)&headers[index] = ImageHeader.Compression;				index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.ImageSize;				index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.WidthPixelsPerMeter;		index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.HeightPixelsPerMeter;	index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.pixelsUsed;				index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = ImageHeader.Importantpixels;

	return headers;
}

void Bitmap::SetPadding(int width)
{
	if ((Padding = (4 - ((width * 3) % 4))) == 4)
		Padding = 0; //Padding of 4 is illegal
}

#endif
