/********************************************************************
* Maze Generation and Solving                                         
* 
* Author: Jordan VanEvery
* Version Date: 2017.4.11
*
* Maze Generate Inputs:
*   int width: number of maze columns
*   int height: number of maze rows
*   int wayPointX: x coordinate of position that is required to be 
*               passed through by maze
*   int wayPointY; same as wayPointX except for y coordinate
*   int wayPointAlleyLength:         NOT IN USE
*   double wayPointDirectionPercent: NOT IN USE
*   double straightProbability:      NOT IN USE
*   int printAlgorithmSteps:         NOT IN USE
*
* Functionality:
*   Generates a maze according to with characteristics determined by
*   input parameters.
*   and print the maze. The solution to the maze is colored in green.
* 
********************************************************************/

#include <stdio.h>
#include <ctype.h> 
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include "mazegen.h"
#include "bmpStruct.h"

#define PIXELS_ON_PIECE_SIDE 8
#define PIXELS_IN_BLOCK 64
#define PIXEL_OFFSET 54
#define BMP_BLOCK_SIZE 246 //in bytes  
#define COLOR_DEPTH_IN_BYTES 3

const char* errors[] ={ "ERROR: Invalid width argument",
                        "ERROR: Invalid height argument",
                        "ERROR: One or more waypoints out of bounds" };

const char generalError[] = {"An error has been detected. Program was\n" 
                              "terminated when error was detected. More\n"
                              "errors may in original input may remain\n"
                              "than the one first detected. \n"
                            };
                             

//Double pointer will represent maze
static unsigned char** maze;
static int rows, columns;
static int wayX, wayY;
unsigned int** pixelMap;
static int mazeMallocFlag = FALSE;

int carveMaze(int row, int col);
void makeExits(void);
void makeWall(int row, int mode);
void shuffle(int *array, size_t n); //Author of code cited in documentation
int searchPath(int row, int column);
void addMazeBlock(int currRow, int currCol);
void printPixelMap(int rows, int cols);

int mazeGenerate(int width, int height,
                 int wayPointX, int wayPointY,
                 int wayPointAlleyLength,
                 double wayPointDirectionPercent,
                 double straightProbability,
                 int printAlgorithmSteps)
{ 
  //Free your mallocs! (If we have a previous allocation floating)
  if(mazeMallocFlag)
  { mazeFree();
    mazeMallocFlag = FALSE;
  } 
  //Check for valid number of rows
  if(height<=2)
  { printf("%s\n", errors[0]);
    printf("%s\n", generalError);
    return TRUE;
  }
  //Check for valid number of columns
  if(width<=2)
  { printf("%s\n", errors[1]);
    printf("%s\n", generalError);
    return TRUE;
  }
  //Check for waypoint out of bounds
  if(wayPointX<=0 || wayPointY<=0 || wayPointX>width || wayPointY>height)
  { printf("%s\n", errors[2]);
    printf("%s\n", generalError);
    return TRUE;
  }
  
  /*Adding a row on top and bottom of maze anda column on each side
  * this will act as a buffer that paths can't cross when carving 
  * out the maze.*/
  rows = height + 2 ;
  columns = width + 2 ;

  //Allocate memory for maze
  maze = malloc((rows)*sizeof(unsigned char*));
  int i,j;
  for(i=0; i<rows; ++i){
    *(maze+i) = malloc((columns)*sizeof(unsigned char));
  }
  mazeMallocFlag = TRUE;

  //Initialize non-buffer entries to 0
  for(i=1; i<rows-1; ++i){
    for(j=1; j<columns-1; ++j){
      maze[i][j] = NO_DIRECTIONS; 
    }
  }

  //Initialize buffer chars to SPECIAL
  for(j=0; j<columns; ++j)
  { maze[0][j] = SPECIAL;
    maze[rows-1][j] = SPECIAL;
  }
  for(j=0; j<rows; ++j)
  { maze[j][0] = SPECIAL;
    maze[j][columns - 1] = SPECIAL;
  }
 
  /* If waypoint is above the middle row of the maze, temporarily 
  * block off cells above the row that the waypoint is in. If the
  * waypoint is below the middle row of the maze, temporarily
  * block off the cells below the row that waypoint is in.
  *
  * This is so that we can carve out the maze into two parts, 
  * with the waypoint being the only way inbetween the lower
  * and upper parts. This forces the solution to go through waypoint
  */
  makeWall( wayPointY, TRUE );
  if( wayPointY <= (rows-2)/2 )
  { 
    //Carve
    carveMaze(wayPointY+1, wayPointX);
    //Unblock section
    makeWall( wayPointY, FALSE );
    //Carve the rest
    carveMaze(wayPointY, wayPointX);
    //Connect cells
    maze[wayPointY+1][wayPointX] |= NORTH;
    maze[wayPointY][wayPointX] |= SOUTH;
  }
  else
  {  
    //Carve
    carveMaze(wayPointY-1, wayPointX);
    //Unblock section
    makeWall( wayPointY, FALSE );
    //Carve the rest
    carveMaze(wayPointY, wayPointX);
    //Connect cells
    maze[wayPointY-1][wayPointX] |= SOUTH;
    maze[wayPointY][wayPointX] |= NORTH;
  }

  //Clear out VISITED spots or any other anomalies
  for(i=1; i<rows-1; ++i)
  { for(j=1; j<columns-1; ++j)
    { maze[i][j] &= ALL_DIRECTIONS;
    }
  }
  //make exits
  makeExits();
  //make global waypoint accessible to print(sloppy fix)
  wayX = wayPointX;
  wayY = wayPointY;
  //In the case of proper evaluation
  return FALSE;
}

/********************************************************************
* void mazeFree() frees up previously allocated memory for the maze
*
* No parameters, no return
********************************************************************/
void mazeFree(void)
{ 
  int i; 
  if(mazeMallocFlag){
    for(i=0; i<rows; ++i)
    { free( *(maze+i) );
    }
    free( maze );
  }
  mazeMallocFlag = FALSE;
}

/********************************************************************
* mazePrint prints the maze using the extended ASCII pipe characters
* including the solution of the maze highlighted in green 
********************************************************************/
//printing with regular ascii right now
void mazePrint(void)
{ //old printing code

  
  //mark waypoint with SPECIAL, so we can color it
  /*
  maze[wayY][wayX] |= SPECIAL; 
  int i,j;
  for(i=1; i<rows-1; ++i)
  { for(j=1; j<columns-1; ++j) 
      { if( maze[i][j] & SPECIAL )
        { textcolor(TEXTCOLOR_RED);
        }
        else if( maze[i][j] & GOAL )
        { textcolor(TEXTCOLOR_GREEN);
        }
        else
        { textcolor(TEXTCOLOR_WHITE);
        }
        printf("%c", pipeList[maze[i][j]&ALL_DIRECTIONS]);
        //printf("%c", maze[i][j] + '0' );
      }
      printf("\n");
  }
  textcolor(TEXTCOLOR_WHITE);
  maze[wayY][wayX] &= (~SPECIAL);
  */
  
  //Allocate memory for buffer array
  //Has dimensions of (height*8)*(width*8)
  int i,j;
  int pixelMapRows = (rows-2)*PIXELS_ON_PIECE_SIDE;
  int pixelMapCols = (columns-2)*PIXELS_ON_PIECE_SIDE;
  pixelMap = malloc( pixelMapRows*sizeof(unsigned int*) );
  for(i=0; i<pixelMapRows; ++i)
  { *(pixelMap+i) = malloc(pixelMapCols*sizeof(unsigned int));
  }
 
  /*Begin walking through and building up bitmap piece by piece, and 
  * we can get a header on the map with our handy dandy header functions
  * that I made in bmpStruct.c */

  //Write pixelMap
  for(i=1; i<rows-1; ++i)
  {
    for(j=1; j<columns-1; ++j)
    { 
      addMazeBlock(i, j);
    }
  }

  //WRITE HEADER
  //24 bit color depth 
  struct headerBMP header;
  makeSimpleHeader(&header, pixelMapCols, pixelMapRows, 24);
  FILE* bmpPixelMap = fopen("maze.bmp", "wb");
  writeHeader(bmpPixelMap, &header);
  printf("WROTE HEADER\n");
  
  //WRITE FROM PIXELMAP TO IMAGE FILE
  //currently the data is stored in integers in pixelMap 
  unsigned char tempArr[4];
  for(i=pixelMapRows-1; i>=0; --i)
  { for(j=0; j<pixelMapCols; ++j)
    { intToCharArr(pixelMap[i][j], tempArr);
      fwrite(tempArr ,sizeof(unsigned char),COLOR_DEPTH_IN_BYTES,bmpPixelMap); 
    }
  }
  printf("WROTE DATA\n");
  
  //FREE PIXELMAP AND CLOSE FILE
  fclose(bmpPixelMap);

  for(i=0; i<pixelMapCols; ++i)
  { free( *(pixelMap+i) );
  }
  free( pixelMap );
 
} 
/********************************************************************
* addMazeBlock adds to the pixel map buffer the 8x8 bitmap associated
* with the piece currently being looped over in print maze. This will
* pull the pixel map data from the piece and add it to the larger 
* full maze pixel map.
*
* Params:
*   i,j: location of piece to be copied into bit map (row, column)
* No Returns
********************************************************************/
void addMazeBlock(int i, int j)
{ char temp = maze[i][j] & ALL_DIRECTIONS;
  char baseFileName[] = "mazeBitMap";
  char endFileName[] = ".bmp";
  char* fileNum;
  if(temp>9)
  { fileNum = malloc( sizeof(char)*3 );
    *fileNum = '1';
    *(fileNum+1) = temp - 10 + 48;
    *(fileNum+2) = '\0';
  }
  else
  { fileNum = malloc( sizeof(char)*2 );
    *fileNum = temp + 48;
    *(fileNum+1) = '\0';
  }
  char* fileName= strcat(strcat(baseFileName, fileNum),endFileName);
  FILE* mazeBlock = fopen(fileName, "rb");
  /* Verify proper file names test
  printf("%s\n", fileName); 
  */
  free(fileNum);
  
  //LOAD BUFFER WITH MAZE BLOCK FILE (.bmp file)
  int k,m,p;
  unsigned char mazeBlockBuffer[BMP_BLOCK_SIZE];
  fread(mazeBlockBuffer, sizeof(unsigned char), BMP_BLOCK_SIZE, mazeBlock);
  fclose(mazeBlock);
  
  /* Print Buffer Contents
  for(k=0; k<BMP_BLOCK_SIZE; ++k)
  { printf("%02x", mazeBlockBuffer[k]);
  }
  printf("\n");
  */
 
  //MOVE FROM BUFFER TO PIXELMAP 
  unsigned int rgbInfo = 0;
  unsigned char tempArr[4];
  unsigned char* offsetPtr = &mazeBlockBuffer[PIXEL_OFFSET];
  for(k=0; k<PIXELS_ON_PIECE_SIDE; ++k)
  { for(m=0; m<PIXELS_ON_PIECE_SIDE; ++m)
    { for(p=0; p<COLOR_DEPTH_IN_BYTES; ++p)
      { rgbInfo |= ( ( (unsigned int)*(offsetPtr+p) ) << (8*p) );
      }
      //printf("k=%d, m=%d\n",k,m);
      pixelMap[PIXELS_ON_PIECE_SIDE*i - k - 1][PIXELS_ON_PIECE_SIDE*(j-1) + m] = rgbInfo;
      rgbInfo = 0;
      offsetPtr += COLOR_DEPTH_IN_BYTES;
    }
  }

}

/********************************************************************
* carveMaze is a recursive function that will be used to generate
* the random maze. Current version will work in a deterministic
* fashion, will be updated to have some random element to it.
* 2017-4-4
*
* Parameters: 
*   int row, col : row and col constitute the indecies for the 
*   current location of the function call in the maze. Initial
*   row and col should correspond to waypoints x and y 
* Return:
*   TRUE: if it was possible to carve a path at the point specified
*   FALSE: if it was not possible to carve even a path of length 1
********************************************************************/
int carveMaze(int row, int col)
{ 
  if( maze[row][col] != NO_DIRECTIONS )
  { return FALSE;
  }
  //Scramble maze directions and store
  int order[] = {0, 1, 2, 3};
  shuffle(order, TOTAL_DIRECTIONS); 
  maze[row][col] = VISITED;
  int i;
  for(i=0; i<TOTAL_DIRECTIONS; ++i)
  { //Look each direction in order specified by 'order'
    if( carveMaze(row + DIRECTION_DY[order[i]], col + DIRECTION_DX[order[i]]) )
    {
      maze[row][col] |= DIRECTION_LIST[order[i]];
      if(DIRECTION_LIST[order[i]] >= SOUTH )
      {  maze[row + DIRECTION_DY[order[i]]][col + DIRECTION_DX[order[i]]] 
         |= (DIRECTION_LIST[order[i]]>>2);
      } 
      else
      {  maze[row + DIRECTION_DY[order[i]]][col + DIRECTION_DX[order[i]]] 
         |= (DIRECTION_LIST[order[i]]<<2);
      }
    }
  }
  //Deterministic testing code rotates clockwise from NORTH to WEST
  /*
  //Look NORTH
  if( carveMaze(row-1,col) )
  { maze[row][col] |= NORTH; 
    maze[row-1][col] |= SOUTH;
  }
  //Look EAST
  if( carveMaze(row,col+1) )
  { maze[row][col] |= EAST;
    maze[row][col+1] |= WEST;
  } 
  //Look SOUTH
  if( carveMaze(row+1,col) )
  { maze[row][col] |= SOUTH;
    maze[row+1][col] |=NORTH;
  }
  //Look WEST 
  if( carveMaze(row,col-1) )  
  { maze[row][col] |= WEST;
    maze[row][col-1] |= EAST;
  }
  */
  return TRUE;
}

/********************************************************************
* shuffle will mix up the elements of an array. 
*
* Code was copied from stack exchange:
*   USER: Asmodiel. 
*   Posted: May 25th 2011
*   Accessed: April 4th 2017
*
* Params: 
*   array: pointer to some array
*   n: number of elemets in array
********************************************************************/

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n)
{
    if (n > 1) 
    {
        size_t i;
        for (i = 0; i < n - 1; i++) 
        {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = array[j];
          array[j] = array[i];
          array[i] = t;
        }
    }
}

/********************************************************************
* makeWall splits the maze horizontally with a wall. This
* is used to block off a section of the maze for carving until later
* This is a means of satisfying the requirement that a solution to 
* the maze must pass through the waypoint. The 'wall' consists of the
* characters SPECIAL. If the function is passed TRUE it builds a wall.
* If it is passed FALSE it fills the row with NO_DIRECTIONS
*
* Params:
*   int row: row in which you want to fill or remove values.
*   int mode: !FALSE for making a wall, FALSE for removing a wall
********************************************************************/
void makeWall(int row, int mode)
{ int i; 
  if( mode == FALSE)
  {
    for(i=1; i<columns-1; ++i)
    { maze[row][i] = NO_DIRECTIONS;
    }
  }
  else
  { 
    for(i=1; i<columns-1; ++i)
    { maze[row][i] = SPECIAL;
    }
  }
}

/********************************************************************
* makeExits picks a random cell on the top and bottom of the maze to
* become an exit. The way the maze is constructed allows for any
* entrance or exit to be chosen on the top and bottom and still meet
* the condition that the solution passes thorugh the waypoint
*
* No params, no returns
********************************************************************/
void makeExits( void )
{ int temp = (int)(rand()%(columns-2) + 1);
  maze[1][temp] |= NORTH;
  temp = (int)(rand()%(columns-2) + 1);
  maze[rows-2][temp] |= (SOUTH|GOAL); 
}

/********************************************************************
* This function exists to meet project requirements of having
* a function called maze solve that is void-void. This function will
* just call searchPath, which is the actual solver. 
********************************************************************/
void mazeSolve ()
{ int i;
  for(i=1; i<columns-1; ++i)
  { if( (maze[1][i] & NORTH) ) break; 
  }
  //searchPath( row=1, col = entrance column)
  searchPath(1,i);
} 

/********************************************************************
* searchPath implements a direct solver for the maze. 
* Uses a recursive method very similar to the maze
* carving algorithm. Using a direct solver so that I could also solve
* unknown mazes. The solution will be stored in the maze model by 
* marking solution cells with the GOAL bit. 
*
* Params:
*   int row, col: current row and column
*
* Returns:
*   TRUE if returning from solution, returns FALSE otherwise
********************************************************************/
int searchPath( int row, int col ) {
  //Makes calls predetermined order, no need to be random here
  if( (maze[row][col] & SPECIAL)||(maze[row][col] & VISITED )) 
  { return FALSE;
  }
  maze[row][col] |= VISITED;
  if( maze[row][col] & GOAL )
  { return TRUE; 
  }
  //Look WEST, SOUTH, EAST, then NORTH
  int i;
  for(i=TOTAL_DIRECTIONS-1; i>=0; --i)
  {
    if( maze[row][col] & DIRECTION_LIST[i] )
    { if( searchPath(row + DIRECTION_DY[i], col + DIRECTION_DX[i] ) )
      { maze[row][col] |= GOAL;
        return TRUE;  
      }
    }
  }
  
  //Simpler to read 'copy paste' code
  /*
  if( (maze[row][col] & SOUTH) )
  { if( searchPath( row+1, col ) )
    { maze[row][col] |= GOAL; 
      return TRUE;
    }
  }
  //Look EAST if possible
  if( maze[row][col] & EAST )
  { if( searchPath( row, col+1 ) )
    { maze[row][col] |= GOAL;
      return TRUE;
    } 
  }
  //Look WEST if possible
  if( maze[row][col] & WEST )
  { if( searchPath( row, col-1 )  )
    { maze[row][col] |= GOAL;
      return TRUE;
    }
  }
  //Look NORTH if possible 
  if( maze[row][col] & NORTH )
  { if( searchPath( row-1, col ) )  
    { maze[row][col] |= GOAL;
      return TRUE;
    }
  }
  */
  return FALSE; 
}

