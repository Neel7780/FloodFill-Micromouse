#ifndef SOLVER_H
#define SOLVER_H

#include "API.h"

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 16

typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;

// A struct to represent a cell in the maze
typedef struct {
    int x;
    int y;
} Cell;

// A struct for the queue used in floodfill
typedef struct {
    Cell items[MAZE_WIDTH * MAZE_HEIGHT];
    int front;
    int rear;
} Queue;

// Function declarations for the solver
Action solver();
Action floodFill();
Action getNextMove(int x, int y, Heading direction);

// Helper functions for the queue
void initQueue(Queue* q);
int isQueueEmpty(Queue* q);
void enqueue(Queue* q, Cell item);
Cell dequeue(Queue* q);

#endif // SOLVER_H