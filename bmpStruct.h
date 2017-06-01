#include<stdio.h>
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
extern void makeSimpleHeader( struct headerBMP* header, unsigned int width, 
                       unsigned int height, unsigned int depth );
extern void writeHeader(FILE* filePointer, struct headerBMP* header);
extern void intToCharArr(unsigned int number, char* arr);
extern int byteArrayToInt(unsigned char a);
