#include "solver.h"
#include "API.h"
#include <stdio.h>

#define UNREACHABLE 255

// Global variables for maze state
static int distance[MAZE_HEIGHT][MAZE_WIDTH];
static int wall_map[MAZE_HEIGHT][MAZE_WIDTH]; // Store wall information
static int mouse_x = 0;
static int mouse_y = 0;
static Heading mouse_direction = NORTH;

// Queue implementation
void initQueue(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

int isQueueEmpty(Queue* q) {
    return q->front == -1;
}

void enqueue(Queue* q, Cell item) {
    if (q->rear == MAZE_WIDTH * MAZE_HEIGHT - 1) return;
    if (q->front == -1) q->front = 0;
    q->rear++;
    q->items[q->rear] = item;
}

Cell dequeue(Queue* q) {
    Cell item = q->items[q->front];
    q->front++;
    if (q->front > q->rear) {
        q->front = q->rear = -1;
    }
    return item;
}

// --- Floodfill Algorithm ---

// Initialize the distance grid
void initialize_distances() {
    for (int i = 0; i < MAZE_HEIGHT; i++) {
        for (int j = 0; j < MAZE_WIDTH; j++) {
            distance[i][j] = UNREACHABLE;
        }
    }
}

// Check if a cell is valid and has no wall
int is_valid_and_no_wall(int x, int y, int current_x, int current_y, Heading direction) {
    if (x < 0 || x >= MAZE_WIDTH || y < 0 || y >= MAZE_HEIGHT) {
        return 0; // Out of bounds
    }

    // Check for walls based on direction
    switch (direction) {
        case NORTH:
            return !(wall_map[current_y][current_x] & 1);
        case EAST:
            return !(wall_map[current_y][current_x] & 2);
        case SOUTH:
            return !(wall_map[current_y][current_x] & 4);
        case WEST:
            return !(wall_map[current_y][current_x] & 8);
    }
    return 0;
}


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


// Update wall map based on sensor readings
void update_walls() {
    if (API_wallFront()) {
        switch (mouse_direction) {
            case NORTH:
                wall_map[mouse_y][mouse_x] |= 1;
                if (mouse_y < MAZE_HEIGHT - 1) wall_map[mouse_y + 1][mouse_x] |= 4;
                break;
            case EAST:
                wall_map[mouse_y][mouse_x] |= 2;
                if (mouse_x < MAZE_WIDTH - 1) wall_map[mouse_y][mouse_x + 1] |= 8;
                break;
            case SOUTH:
                wall_map[mouse_y][mouse_x] |= 4;
                if (mouse_y > 0) wall_map[mouse_y - 1][mouse_x] |= 1;
                break;
            case WEST:
                wall_map[mouse_y][mouse_x] |= 8;
                if (mouse_x > 0) wall_map[mouse_y][mouse_x - 1] |= 2;
                break;
        }
    }
    if (API_wallLeft()) {
         switch (mouse_direction) {
            case NORTH:
                wall_map[mouse_y][mouse_x] |= 8;
                if (mouse_x > 0) wall_map[mouse_y][mouse_x - 1] |= 2;
                break;
            case EAST:
                wall_map[mouse_y][mouse_x] |= 1;
                if (mouse_y < MAZE_HEIGHT - 1) wall_map[mouse_y + 1][mouse_x] |= 4;
                break;
            case SOUTH:
                wall_map[mouse_y][mouse_x] |= 2;
                if (mouse_x < MAZE_WIDTH - 1) wall_map[mouse_y][mouse_x + 1] |= 8;
                break;
            case WEST:
                wall_map[mouse_y][mouse_x] |= 4;
                if (mouse_y > 0) wall_map[mouse_y - 1][mouse_x] |= 1;
                break;
        }
    }
    if (API_wallRight()) {
        switch (mouse_direction) {
            case NORTH:
                wall_map[mouse_y][mouse_x] |= 2;
                if (mouse_x < MAZE_WIDTH - 1) wall_map[mouse_y][mouse_x + 1] |= 8;
                break;
            case EAST:
                wall_map[mouse_y][mouse_x] |= 4;
                if (mouse_y > 0) wall_map[mouse_y - 1][mouse_x] |= 1;
                break;
            case SOUTH:
                wall_map[mouse_y][mouse_x] |= 8;
                if (mouse_x > 0) wall_map[mouse_y][mouse_x - 1] |= 2;
                break;
            case WEST:
                wall_map[mouse_y][mouse_x] |= 1;
                if (mouse_y < MAZE_HEIGHT - 1) wall_map[mouse_y + 1][mouse_x] |= 4;
                break;
        }
    }
}

// Decide the next move based on the floodfill values
Action get_next_action() {
    int min_dist = UNREACHABLE;
    Heading best_direction = NORTH;

    // Check neighbors
    int dx[] = {0, 1, 0, -1}; // N, E, S, W
    int dy[] = {1, 0, -1, 0};
    Heading directions[] = {NORTH, EAST, SOUTH, WEST};

    for (int i = 0; i < 4; i++) {
        int next_x = mouse_x + dx[i];
        int next_y = mouse_y + dy[i];

        if (is_valid_and_no_wall(next_x, next_y, mouse_x, mouse_y, directions[i])) {
            if (distance[next_y][next_x] < min_dist) {
                min_dist = distance[next_y][next_x];
                best_direction = directions[i];
            }
        }
    }

    // Determine action to get to the best direction
    if (best_direction == mouse_direction) {
        mouse_x += (best_direction == EAST) - (best_direction == WEST);
        mouse_y += (best_direction == NORTH) - (best_direction == SOUTH);
        return FORWARD;
    } else if ((mouse_direction + 1) % 4 == best_direction) {
        mouse_direction = (mouse_direction + 1) % 4;
        return RIGHT;
    } else if ((mouse_direction - 1 + 4) % 4 == best_direction) {
        mouse_direction = (mouse_direction - 1 + 4) % 4;
        return LEFT;
    } else { // U-turn
        mouse_direction = (mouse_direction + 1) % 4; // Turn right
        return RIGHT;
    }
}


Action floodFill() {
    update_walls();
    run_floodfill();
    return get_next_action();
}

Action solver() {
    return floodFill();
}
