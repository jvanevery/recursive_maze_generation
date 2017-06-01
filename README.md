# recursive_maze_generation
Uses a recursive algorithm to generate a model of a 2D maze, and from the model writes a pixel map that is a visual representation of the maze.


===============================================
'INTERFACE' FUNCTIONS TO BE USED BY CALLER
===============================================

I)
int mazeGenerate(int width, int height,   // [3, 1000],  [3, 1000]
    int wayPointX, int wayPointY,         // [1, width],   [1, height]
    int wayPointAlleyLength,              // [0,  min(width, height)/2 ]
    double wayPointDirectionPercent,      // [0.0,  1.0]
    double straightProbability,           // [0.0,  1.0]
    int printAlgorithmSteps);             // [TRUE | FALSE]
    
    width, height : dimensions of maze
    wayPointX, wayPointY : point through which solution must pass
    
    OTHER ARGUMENTS ARE CURRENTLY NOT IN USE AND SHOULD BE HANDED FILLER
    
II)
void mazePrint(void)

Writes a bmp format image file that is a visualization of the current maze held in memory from the call to mazeGenerate(...)

III)
void mazeFree(void)

Frees the memory that the last generated maze from mazeGenerate() is occupying.

IV)
void mazeSolve(void)

Finds a solution to the maze and adds it to the maze model in memory. Currently mazePrint() has not been modified to include the visualization of this part of the maze, and ignores the extra info.

An example test file called mazetest.c demonstrates how calls to the functions are made. An example of what is generated is shown in test.bmp
