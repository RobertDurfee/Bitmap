#ifndef BITMAP_HEADER
#define BITMAP_HEADER

#include <string>
#include <fstream>

using namespace std;

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

#define SIZE_OF_HEADERS 0x36

class Bitmap
{
public:
	Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader, unsigned char * pixels);
	Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader);
	Bitmap(int width, int height, unsigned char * pixels);
	Bitmap(int width, int height);
	Bitmap(string filename);

	void Open(string filename);
	void Save(string filename);
	void Save();

	unsigned char * GetPixels(int * width, int * height);
	void SetPixels(int width, int height, unsigned char * pixels);
	void SetPixels(unsigned char * pixels);

private:
	string Filename;
	unsigned char * PixelData;
	int Padding;
	BITMAP_FILE_HEADER FileHeader;
	BITMAP_IMAGE_HEADER ImageHeader;

	void SetHeaders(int width, int height);
	void SetHeaders(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader);
	void SetHeaders(char * headers);
	char * GetHeaders();
};
Bitmap::Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader)
{
	this->SetHeaders(fileHeader, imageHeader);
	
	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);
}
Bitmap::Bitmap(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader, unsigned char * pixels)
{
	this->SetHeaders(fileHeader, imageHeader);
	
	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);

	this->SetPixels(pixels);
}
Bitmap::Bitmap(int width, int height, unsigned char * pixels)
{
	this->SetHeaders(width, height);

	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);

	this->SetPixels(pixels);
}
Bitmap::Bitmap(int width, int height)
{
	this->SetHeaders(width, height);

	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);
}
Bitmap::Bitmap(string filename)
{
	this->Open(filename);
}

void Bitmap::Open(string filename)
{
	if (this->PixelData)
		free(this->PixelData);

	this->Filename = filename;

	ifstream file(this->Filename, ios::binary);

	char headers[SIZE_OF_HEADERS];
	file.read(headers, SIZE_OF_HEADERS);
	this->SetHeaders(headers);

	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);
	file.read((char *)this->PixelData, this->ImageHeader.ImageSize);

	file.close();
}
void Bitmap::Save(string filename)
{
	this->Filename = filename;

	ofstream file(this->Filename, ios::binary);

	char * headers = this->GetHeaders();
	file.write(headers, SIZE_OF_HEADERS);
	free(headers);

	file.write((char *)this->PixelData, this->ImageHeader.ImageSize);

	file.close();
}
void Bitmap::Save()
{
	this->Save(this->Filename);
}

unsigned char * Bitmap::GetPixels(int * width, int * height)
{
	*width = this->ImageHeader.Width;
	*height = this->ImageHeader.Height;

	unsigned char * pixels = (unsigned char *)malloc(this->ImageHeader.Width * this->ImageHeader.Height * 3);

	for (int y = 0; y < *height; y++)
		for (int x = 0; x < *width; x++)
			for (int i = 0; i < 3; i++)
				pixels[y * (*width * 3) + (x * 3) + i] = this->PixelData[y * (*width * 3 + this->Padding) + (x * 3) + i];

	return pixels;
}
void Bitmap::SetPixels(int width, int height, unsigned char * pixels)
{
	this->ImageHeader.Width = width;
	this->ImageHeader.Height = height;

	this->Padding = 4 - ((this->ImageHeader.Width * 3) % 4);
	if (this->Padding == 4)
		this->Padding = 0;

	this->ImageHeader.ImageSize = (width * height * 3) + this->Padding * height;

	this->FileHeader.Size = this->ImageHeader.ImageSize + 54;

	if (this->PixelData)
		free(this->PixelData);

	this->PixelData = (unsigned char *)malloc(this->ImageHeader.ImageSize);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
			for (int i = 0; i < 3; i++)
				this->PixelData[y * (width * 3 + this->Padding) + (x * 3) + i] = pixels[y * (width * 3) + (x * 3) + i];

		for (int i = 0; i < this->Padding; i++)
			this->PixelData[y * (width * 3 + this->Padding) + (width * 3) + i] = 0xCC;
	}
}
void Bitmap::SetPixels(unsigned char * pixels)
{
	this->SetPixels(this->ImageHeader.Width, this->ImageHeader.Height, pixels);
}

void Bitmap::SetHeaders(int width, int height)
{
	this->Padding = (4 - ((width * 3) % 4));
	if (this->Padding == 4)
		this->Padding = 0;

	this->FileHeader.Type = 0x4D42;
	this->FileHeader.Reserved = 0x00000000;
	this->FileHeader.Offset = 54;

	this->ImageHeader.Size = 40;
	this->ImageHeader.Width = width;
	this->ImageHeader.Height = height;
	this->ImageHeader.Planes = 1;
	this->ImageHeader.BitCount = 24;
	this->ImageHeader.Compression = 0;
	this->ImageHeader.ImageSize = (width * height * 3) + this->Padding * height;
	this->ImageHeader.WidthPixelsPerMeter = 0;
	this->ImageHeader.HeightPixelsPerMeter = 0;
	this->ImageHeader.pixelsUsed = 0;
	this->ImageHeader.Importantpixels = 0;

	this->FileHeader.Size = this->ImageHeader.ImageSize + 54;
}
void Bitmap::SetHeaders(BITMAP_FILE_HEADER fileHeader, BITMAP_IMAGE_HEADER imageHeader)
{
	this->FileHeader = fileHeader;
	this->ImageHeader = imageHeader;

	this->Padding = (4 - ((this->ImageHeader.Width * 3) % 4));
	if (this->Padding == 4)
		this->Padding = 0;
}
void Bitmap::SetHeaders(char * headers)
{
	int index = 0;

	this->FileHeader.Type = *(unsigned short*)&headers[index]; index += sizeof(unsigned short);
	this->FileHeader.Size = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->FileHeader.Reserved = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->FileHeader.Offset = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);

	this->ImageHeader.Size = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.Width = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.Height = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.Planes = *(unsigned short*)&headers[index]; index += sizeof(unsigned short);
	this->ImageHeader.BitCount = *(unsigned short*)&headers[index]; index += sizeof(unsigned short);
	this->ImageHeader.Compression = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.ImageSize = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.WidthPixelsPerMeter = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.HeightPixelsPerMeter = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.pixelsUsed = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);
	this->ImageHeader.Importantpixels = *(unsigned long*)&headers[index]; index += sizeof(unsigned long);

	this->Padding = (4 - ((this->ImageHeader.Width * 3) % 4));
	if (this->Padding == 4)
		this->Padding = 0;
}
char * Bitmap::GetHeaders()
{
	char * headers = (char *)malloc(SIZE_OF_HEADERS);

	int index = 0;

	*(unsigned short*)&headers[index] = this->FileHeader.Type; index += sizeof(unsigned short);
	*(unsigned long*)&headers[index] = this->FileHeader.Size; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->FileHeader.Reserved; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->FileHeader.Offset; index += sizeof(unsigned long);

	*(unsigned long*)&headers[index] = this->ImageHeader.Size; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.Width; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.Height; index += sizeof(unsigned long);
	*(unsigned short*)&headers[index] = this->ImageHeader.Planes; index += sizeof(unsigned short);
	*(unsigned short*)&headers[index] = this->ImageHeader.BitCount; index += sizeof(unsigned short);
	*(unsigned long*)&headers[index] = this->ImageHeader.Compression; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.ImageSize; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.WidthPixelsPerMeter; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.HeightPixelsPerMeter; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.pixelsUsed; index += sizeof(unsigned long);
	*(unsigned long*)&headers[index] = this->ImageHeader.Importantpixels; index += sizeof(unsigned long);

	return headers;
}

#endif
