#include <stdio.h> 

struct headerBMP{
  // BITMAP FILE HEADER //
  // 'B' and 'M' will be added to file upon construction  2 bytes
  unsigned int bmpSize;      //4 bytes
  unsigned int reserved;     //4 bytes
  unsigned int pixelOffset;  //4 bytes
  // DIB HEADER //
  unsigned int dibSize;         //4 bytes
  unsigned int imageWidth;      //4 bytes
  unsigned int imageHeight;     //4 bytes
  unsigned int numColorPlanes;  //2 bytes
  unsigned int colorDepth;      //2 bytes
  unsigned int compression;     //4 bytes
  unsigned int imageByteSize;   //4 bytes
  unsigned int horizontalRes;   //4 bytes
  unsigned int verticalRes;     //4 bytes
  unsigned int colorPalette;    //4 bytes
  unsigned int importantColors; //4 bytes
};

void makeSimpleHeader( struct headerBMP* header, unsigned int width, 
                       unsigned int height, unsigned int depth );
void intToCharArr(unsigned int number, char* arr);
void writeHeader(FILE* filePointer, struct headerBMP* header);
int btyeArrayToInt(unsigned char* a);

/*
int main(void)
{ printf("size of int: %lu  size of char: %lu\n", sizeof(unsigned int), sizeof(char));
  struct headerBMP testHeader;
  makeSimpleHeader(&testHeader, 8, 8, 24);
  printf("Made it past making header\n");
  printf("bmp size: %d, image height: %d, image width: %d, color depth: %d\n", 
         testHeader.bmpSize, testHeader.imageHeight, testHeader.imageWidth, 
         testHeader.colorDepth);
  FILE* testFile = fopen("testFile.bmp", "w");
  writeHeader(testFile, &testHeader);
  printf("Made it past writing header\n");
  fclose(testFile);
}
*/


/******************************************************************************
 * makeSimple header takes image width and height and pixel depth and pointer 
 * to a struct and creates a bmp header with default values for othe fields 
 * like compression and resolution
 * ***************************************************************************/

void makeSimpleHeader( struct headerBMP* headerPtr, unsigned int width, 
                       unsigned int height, unsigned int depth )
{ headerPtr->imageWidth = width;   //in pixels
  headerPtr->imageHeight = height; //in pixels
  headerPtr->numColorPlanes = 1;   
  headerPtr->colorDepth = depth;   //in bits
  headerPtr->dibSize = 40;     //in bytes
  headerPtr->bmpSize = (((headerPtr->imageWidth)*(headerPtr->imageHeight)*
                       (headerPtr->colorDepth)/8) + 54); //in bytes
  headerPtr->reserved = 0;
  headerPtr->pixelOffset = 54; //in bytes
  headerPtr->compression = 0; 
  headerPtr->imageByteSize = width*height*depth/8;
  headerPtr->horizontalRes = 0x13 | (0x0B << 8);
  headerPtr->verticalRes = 0x13 | (0x0B << 8);
  headerPtr->colorPalette = 0;
  headerPtr->importantColors = 0;
}
//Header size is in bytes
void writeHeader(FILE* filePointer, struct headerBMP* header)
{ //File must already be open for writing!!
  char tempArray[4];
  char bufferArray[header->pixelOffset];
  int nextOpen = 0;

  //Magic values
  bufferArray[0]='B';
  bufferArray[1]='M';
  nextOpen+=2;

  //size of file
  intToCharArr(header->bmpSize, tempArray);
  int i;
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i]; 
  }
  nextOpen+=4;

  //reserved
  intToCharArr(header->reserved, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //Pixel offset
  intToCharArr(header->pixelOffset, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //DIB size
  intToCharArr(header->dibSize, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //Image Width and height
  intToCharArr(header->imageWidth, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  intToCharArr(header->imageHeight, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //Number of Color Planes
  intToCharArr(header->numColorPlanes, tempArray);
  for(i=0; i<2; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=2;

  //Color Depth (bits in a pixel)
  intToCharArr(header->colorDepth, tempArray);
  for(i=0; i<2; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=2;

  //Compression
  intToCharArr(header->compression, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //image byte size
  intToCharArr(header->imageByteSize, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //horizontal and vertical resolution
  intToCharArr(header->horizontalRes, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  intToCharArr(header->verticalRes, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  //colors in palette and important colors
  intToCharArr(header->colorPalette, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  nextOpen+=4;

  intToCharArr(header->importantColors, tempArray);
  for(i=0; i<4; ++i)
  { bufferArray[nextOpen+i] = tempArray[i];
  }
  
  //Write in the buffer array to the file
  fwrite(bufferArray, sizeof(char), header->pixelOffset, filePointer);
}

//integers to char arrays in little endian
void intToCharArr(unsigned int number, char* arr){
  unsigned int n = 0;
  for(n=0; n<4; ++n)
  { //logical shift right
    *(arr+n) = (unsigned char)(number >> (8*n));
    //printf( "%02x ", *(arr+n) );
  }
  //printf("\n");
}

//Joel's function from website
int byteArrayToInt(unsigned char* a)
{
  return a[0] | (a[1]<<8) | (a[2]<<16) | a[3]<<24;
}
