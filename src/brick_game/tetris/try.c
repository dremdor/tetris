#include "try.h"

int **create(int rows, int cols) {
  int **ptr = (int **)calloc(rows, sizeof(int *));
  for (int i = 0; i < rows; ++i) {
    ptr[i] = (int *)calloc(cols, sizeof(int));
  }
  return ptr;
}

void destroy(int **ptr, int rows) {
  for (int i = 0; i < rows; ++i) {
    free(ptr[i]);
  }
  free(ptr);
}

GameInfo_t *initGameInfo() {
  static GameInfo_t gameInfo;

  if (gameInfo.field == NULL && gameInfo.next == NULL) {
    gameInfo.field = create(BOARD_HEIGHT, BOARD_WIDTH);
    gameInfo.current = create(PIECE_SIZE, PIECE_SIZE);
    gameInfo.next = create(PIECE_SIZE, PIECE_SIZE);

    gameInfo.currentX = (BOARD_WIDTH - PIECE_SIZE) / 2;
    gameInfo.currentY = 0;
    gameInfo.score = 0;
    initScore(&gameInfo);
    gameInfo.level = 1;
    gameInfo.speed = 500;
    gameInfo.pause = 0;
    gameInfo.last_time = 0;
    gameInfo.currentState = START;

    nextPiece(&gameInfo);
    for (int y = 0; y < PIECE_SIZE; ++y)
      for (int x = 0; x < PIECE_SIZE; ++x)
        gameInfo.current[y][x] = gameInfo.next[y][x];
  }

  return &gameInfo;
}

void initScore() {
  GameInfo_t *gameInfo = updateCurrentState();
  FILE *fp = fopen("score.txt", "r");
  if (fp) {
    int score = 0;
    if (fscanf(fp, "%d", &score) == 1 && score > 0) {
      gameInfo->high_score = score;
    } else {
      gameInfo->high_score = 0;
    }
  } else {
    gameInfo->high_score = 0;
  }
  fclose(fp);
}

GameInfo_t *updateCurrentState() { return initGameInfo(); }

void freeGameInfo() {
  GameInfo_t *gameInfo = updateCurrentState();
  if (gameInfo->field) destroy(gameInfo->field, BOARD_HEIGHT);
  if (gameInfo->current) destroy(gameInfo->current, PIECE_SIZE);
  if (gameInfo->next) destroy(gameInfo->next, PIECE_SIZE);
}

void startState(UserAction_t action) {
  GameInfo_t *gameInfo = updateCurrentState();
  switch (action) {
    case Start:
      gameInfo->currentState = SPAWN;
      break;
    case Terminate:
      gameInfo->currentState = EXIT_STATE;
      break;
    default:
      break;
  }
}

void spawnState() {
  GameInfo_t *gameInfo = updateCurrentState();

  for (int y = 0; y < PIECE_SIZE; ++y)
    for (int x = 0; x < PIECE_SIZE; ++x)
      gameInfo->current[y][x] = gameInfo->next[y][x];

  gameInfo->currentX = (BOARD_WIDTH - PIECE_SIZE) / 2;
  gameInfo->currentY = 0;

  nextPiece();

  if (collision())
    gameInfo->currentState = GAMEOVER;
  else
    gameInfo->currentState = MOVING;
}

void movingState(UserAction_t action) {
  GameInfo_t *gameInfo = updateCurrentState();

  switch (action) {
    case Down:
      moveDown();
      break;
    case Left:
      moveLeft();
      break;
    case Right:
      moveRight();
      break;
    case Action:
      rotate();
      break;
    case Terminate:
      gameInfo->currentState = EXIT_STATE;
      break;
    default:
      break;
  }

  if (timer() && gameInfo->currentState != EXIT_STATE) {
    gameInfo->currentState = SHIFTING;
  }
}

void shiftingState() {
  GameInfo_t *gameInfo = updateCurrentState();
  gameInfo->currentY += 1;

  if (!collision()) {
    gameInfo->currentState = MOVING;
  } else {
    gameInfo->currentY -= 1;
    gameInfo->currentState = ATTACHING;
  }
}

void attachingState() {
  GameInfo_t *gameInfo = updateCurrentState();
  placePiece();
  int lines = clearLines(gameInfo);
  countScore(lines);

  gameInfo->currentState = SPAWN;
}

void gameoverState() {
  GameInfo_t *gameInfo = updateCurrentState();
  resetGame(gameInfo);
  gameInfo->currentState = START;
}

void resetGame() {
  GameInfo_t *gameInfo = updateCurrentState();

  gameInfo->currentX = (BOARD_WIDTH - PIECE_SIZE) / 2;
  gameInfo->currentY = 0;
  gameInfo->score = 0;
  initScore(gameInfo);
  gameInfo->level = 1;
  gameInfo->speed = 500;
  gameInfo->pause = 0;
  gameInfo->last_time = 0;

  nextPiece(&gameInfo);
  for (int y = 0; y < PIECE_SIZE; ++y)
    for (int x = 0; x < PIECE_SIZE; ++x)
      gameInfo->current[y][x] = gameInfo->next[y][x];

  for (int y = 0; y < BOARD_HEIGHT; ++y)
    for (int x = 0; x < BOARD_WIDTH; ++x) gameInfo->field[y][x] = 0;
}

void sigact(UserAction_t action) {
  GameInfo_t *gameInfo = updateCurrentState();
  switch (gameInfo->currentState) {
    case START:
      startState(action);
      break;
    case SPAWN:
      spawnState();
      break;
    case MOVING:
      movingState(action);
      break;
    case SHIFTING:
      shiftingState();
      break;
    case ATTACHING:
      attachingState();
      break;
    case GAMEOVER:
      gameoverState();
      break;
    default:
      break;
  }

  if (action == Terminate) {
    gameInfo->currentState = EXIT_STATE;
  }
}

bool collision() {
  GameInfo_t *gameInfo = updateCurrentState();
  bool collision = false;

  for (int y = 0; y < PIECE_SIZE; ++y) {
    for (int x = 0; x < PIECE_SIZE; ++x) {
      if (gameInfo->current[y][x] != 0) {
        int boardX = gameInfo->currentX + x;
        int boardY = gameInfo->currentY + y;

        if (boardX < 0 || boardX >= BOARD_WIDTH || boardY >= BOARD_HEIGHT ||
            (boardY >= 0 && gameInfo->field[boardY][boardX] != 0)) {
          collision = true;
        }
      }
    }
  }

  return collision;
}

void moveRight() {
  GameInfo_t *gameInfo = updateCurrentState();
  gameInfo->currentX += 1;
  if (collision(gameInfo)) {
    gameInfo->currentX -= 1;
  }
}

void moveLeft() {
  GameInfo_t *gameInfo = updateCurrentState();
  gameInfo->currentX -= 1;
  if (collision(gameInfo)) {
    gameInfo->currentX += 1;
  }
}

void moveDown() {
  GameInfo_t *gameInfo = updateCurrentState();
  gameInfo->currentY += 1;
  if (collision(gameInfo)) {
    gameInfo->currentY -= 1;
  }
}

void rotate() {
  GameInfo_t *gameInfo = updateCurrentState();
  int temp[PIECE_SIZE][PIECE_SIZE] = {0};

  for (int i = 0; i < PIECE_SIZE; ++i) {
    for (int j = 0; j < PIECE_SIZE; ++j) {
      temp[j][PIECE_SIZE - 1 - i] = gameInfo->current[i][j];
    }
  }

  int original[PIECE_SIZE][PIECE_SIZE];
  for (int i = 0; i < PIECE_SIZE; ++i) {
    for (int j = 0; j < PIECE_SIZE; ++j) {
      original[i][j] = gameInfo->current[i][j];
      gameInfo->current[i][j] = temp[i][j];
    }
  }

  if (collision()) {
    for (int i = 0; i < PIECE_SIZE; ++i) {
      for (int j = 0; j < PIECE_SIZE; ++j) {
        gameInfo->current[i][j] = original[i][j];
      }
    }
  }
}

void nextPiece() {
  GameInfo_t *gameInfo = updateCurrentState();
  int pieces[NUM_PIECES][PIECE_SIZE][PIECE_SIZE] = {
      {
          {0, 0, 0, 0},
          {1, 1, 1, 1},
          {0, 0, 0, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {1, 0, 0, 0},
          {1, 1, 1, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {0, 0, 1, 0},
          {1, 1, 1, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {0, 1, 1, 0},
          {0, 1, 1, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {0, 1, 1, 0},
          {1, 1, 0, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {0, 1, 0, 0},
          {1, 1, 1, 0},
          {0, 0, 0, 0},
      },
      {
          {0, 0, 0, 0},
          {1, 1, 0, 0},
          {0, 1, 1, 0},
          {0, 0, 0, 0},
      },
  };

  int next = rand() % NUM_PIECES;

  for (int y = 0; y < PIECE_SIZE; ++y) {
    for (int x = 0; x < PIECE_SIZE; ++x) {
      gameInfo->next[y][x] = pieces[next][y][x];
    }
  }
}

void placePiece() {
  GameInfo_t *gameInfo = updateCurrentState();
  for (int y = 0; y < PIECE_SIZE; ++y) {
    for (int x = 0; x < PIECE_SIZE; ++x) {
      if (gameInfo->current[y][x] != 0) {
        gameInfo->field[gameInfo->currentY + y][gameInfo->currentX + x] =
            gameInfo->current[y][x];
      }
    }
  }
}

int clearLines() {
  GameInfo_t *gameInfo = updateCurrentState();
  int cleared = 0;
  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    if (fullLine(y)) {
      removeLine(y);
      cleared += 1;
    }
  }
  return cleared;
}

bool fullLine(int y) {
  GameInfo_t *gameInfo = updateCurrentState();
  for (int x = 0; x < BOARD_WIDTH; ++x) {
    if (gameInfo->field[y][x] == 0) {
      return false;
    }
  }
  return true;
}

void removeLine(int y) {
  GameInfo_t *gameInfo = updateCurrentState();
  for (int i = y; i > 0; --i) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      gameInfo->field[i][x] = gameInfo->field[i - 1][x];
    }
  }
  for (int x = 0; x < BOARD_WIDTH; ++x) {
    gameInfo->field[0][x] = 0;
  }
}

void countScore(int lines) {
  GameInfo_t *gameInfo = updateCurrentState();
  int const scores[] = {0, 100, 300, 700, 1500};

  gameInfo->score += scores[lines];
  gameInfo->level = 1 + (gameInfo->score / 600);

  countSpeed();

  if (gameInfo->score >= gameInfo->high_score) {
    gameInfo->high_score = gameInfo->score;
    updateScore();
  }
}

void updateScore() {
  GameInfo_t *gameInfo = updateCurrentState();
  FILE *fp = fopen("score.txt", "w");
  if (fp) {
    fprintf(fp, "%d", gameInfo->score);
    fclose(fp);
  }
}

void countSpeed() {
  GameInfo_t *gameInfo = updateCurrentState();
  int delay = 500;
  int min_delay = 50;
  int max_lvl = 10;

  if (gameInfo->level <= max_lvl)
    gameInfo->speed =
        delay - (gameInfo->level - 1) * ((delay - min_delay) / (max_lvl - 1));
  else
    gameInfo->speed = min_delay;
}

long long getCurrentTime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

bool timer() {
  GameInfo_t *gameInfo = updateCurrentState();
  bool result = false;
  long long current_time = getCurrentTime();

  if (current_time - gameInfo->last_time >= gameInfo->speed) {
    gameInfo->last_time = current_time;
    result = true;
  }

  return result;
}