#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define PIECE_SIZE 4
#define NUM_PIECES 7

typedef enum {
  START,
  SPAWN,
  MOVING,
  SHIFTING,
  ATTACHING,
  GAMEOVER,
  EXIT_STATE,
} GameState_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **current;
  int currentX;
  int currentY;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  long long last_time;
  GameState_t currentState;
} GameInfo_t;

void startState(UserAction_t action);
void spawnState();
void movingState(UserAction_t action);
void shiftingState();
void attachingState();
void gameoverState();

void sigact(UserAction_t action);

GameInfo_t *initGameInfo();
void initScore();
void updateScore();
GameInfo_t *updateCurrentState();
void freeGameInfo();
void resetGame();

int **create(int rows, int cols);
void destroy(int **ptr, int rows);

bool collision();
void moveRight();
void moveLeft();
void moveDown();
void rotate();

void nextPiece();

void placePiece();
int clearLines();
bool fullLine(int y);
void removeLine(int y);
void countScore(int lines);
void countSpeed();

long long getCurrentTime();
bool timer();