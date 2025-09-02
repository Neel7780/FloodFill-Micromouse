# Floodfill Micromouse

This project implements the Floodfill algorithm for a micromouse maze-solving robot. The algorithm is designed to find the shortest path from the starting corner to the center of the maze.

## Core Logic

The heart of the Floodfill algorithm is to calculate the "distance" of each cell from the target (the center of the maze). The mouse then always moves to an adjacent cell with a lower distance value. If the mouse encounters a wall, it updates its internal map of the maze and recalculates the distances. This process is repeated until the mouse reaches the center.

The core of this logic can be seen in the `run_floodfill` function:

```c
// The main floodfill calculation
void run_floodfill() {
    Queue q;
    initQueue(&q);
    initialize_distances();

    // Target cells (center of the maze)
    distance[MAZE_HEIGHT / 2 - 1][MAZE_WIDTH / 2 - 1] = 0;
    distance[MAZE_HEIGHT / 2 - 1][MAZE_WIDTH / 2] = 0;
    distance[MAZE_HEIGHT / 2][MAZE_WIDTH / 2 - 1] = 0;
    distance[MAZE_HEIGHT / 2][MAZE_WIDTH / 2] = 0;

    enqueue(&q, (Cell){MAZE_WIDTH / 2 - 1, MAZE_HEIGHT / 2 - 1});
    enqueue(&q, (Cell){MAZE_WIDTH / 2, MAZE_HEIGHT / 2 - 1});
    enqueue(&q, (Cell){MAZE_WIDTH / 2 - 1, MAZE_HEIGHT / 2});
    enqueue(&q, (Cell){MAZE_WIDTH / 2, MAZE_HEIGHT / 2});


    while (!isQueueEmpty(&q)) {
        Cell current = dequeue(&q);

        // Neighbors
        int dx[] = {0, 1, 0, -1}; // N, E, S, W
        int dy[] = {1, 0, -1, 0};
        Heading directions[] = {NORTH, EAST, SOUTH, WEST};

        for (int i = 0; i < 4; i++) {
            int next_x = current.x + dx[i];
            int next_y = current.y + dy[i];

            if (is_valid_and_no_wall(next_x, next_y, current.x, current.y, directions[i])) {
                if (distance[next_y][next_x] > distance[current.y][current.x] + 1) {
                    distance[next_y][next_x] = distance[current.y][current.x] + 1;
                    enqueue(&q, (Cell){next_x, next_y});
                }
            }
        }
    }
}
```

## Testing

This algorithm was tested using the open-source micromouse simulator **mms**.

## References

The `mms` simulator can be found on GitHub: [https://github.com/micromouseonline/mms](https://github.com/micromouseonline/mms)
