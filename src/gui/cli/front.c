#include "../../brick_game/tetris/try.h"

void drawBoard(GameInfo_t *gameInfo);
void userInput(UserAction_t *action, bool hold);
void gameOverScreen();

int main() {
  srand(time(NULL));

  GameInfo_t *gameInfo = initGameInfo();
  initscr();
  noecho();
  curs_set(FALSE);
  keypad(stdscr, TRUE);
  nodelay(stdscr, TRUE);
  bool running = true;

  UserAction_t action = Up;

  while (running) {
    if (!gameInfo->pause) {
      gameInfo = updateCurrentState();

      sigact(action);

      drawBoard(gameInfo);

    } else {
      mvprintw(5, BOARD_WIDTH * 2 + 2, "Paused");
      refresh();
    }

    userInput(&action, false);
    if (gameInfo->currentState == GAMEOVER) {
      gameOverScreen(&action, false);
      nodelay(stdscr, FALSE);
      while (1) {
        if (getch() == 'q') {
          gameInfo->currentState = EXIT_STATE;
          break;
        } else {
          srand(time(NULL));
          break;
        }
      }
      nodelay(stdscr, TRUE);
    }

    if (gameInfo->currentState == EXIT_STATE || action == Terminate) {
      running = false;
    }

    timeout(10);
  }

  freeGameInfo(gameInfo);
  endwin();

  return 0;
}

void drawBoard(GameInfo_t *gameInfo) {
  clear();

  for (int y = 0; y < BOARD_HEIGHT; ++y) {
    for (int x = 0; x < BOARD_WIDTH; ++x) {
      if (gameInfo->field[y][x] != 0) {
        mvprintw(y, x * 2, "[]");
      }
    }
  }

  if (gameInfo->currentState != START) {
    for (int y = 0; y < PIECE_SIZE; ++y) {
      for (int x = 0; x < PIECE_SIZE; ++x) {
        if (gameInfo->current[y][x] != 0) {
          int boardX = gameInfo->currentX + x;
          int boardY = gameInfo->currentY + y;
          if (boardX >= 0 && boardX < BOARD_WIDTH && boardY >= 0 &&
              boardY < BOARD_HEIGHT) {
            mvprintw(boardY, boardX * 2, "[]");
          }
        }
      }
    }
  }

  mvprintw(0, BOARD_WIDTH * 2 + 2, "Next:");
  for (int y = 0; y < PIECE_SIZE; ++y) {
    for (int x = 0; x < PIECE_SIZE; ++x) {
      if (gameInfo->next[y][x] != 0) {
        mvprintw(y + 1, (BOARD_WIDTH + 2 + x) * 2, "[]");
      }
    }
  }

  mvprintw(6, BOARD_WIDTH * 2 + 2, "Score: %d", gameInfo->score);
  mvprintw(7, BOARD_WIDTH * 2 + 2, "High Score: %d", gameInfo->high_score);
  mvprintw(8, BOARD_WIDTH * 2 + 2, "Level: %d", gameInfo->level);

  if (gameInfo->pause) {
    mvprintw(10, BOARD_WIDTH * 2 + 2, "Game Paused");
  }

  refresh();
}

void userInput(UserAction_t *action, bool hold) {
  (void)hold;
  GameInfo_t *gameInfo = updateCurrentState();
  int ch = getch();
  switch (ch) {
    case KEY_LEFT:
      *action = Left;
      break;
    case KEY_RIGHT:
      *action = Right;
      break;
    case KEY_DOWN:
      *action = Down;
      break;
    case ' ':
      *action = Action;
      break;
    case 10:
      *action = Start;
      break;
    case 'p':
      *action = Pause;
      gameInfo->pause = !gameInfo->pause;
      break;
    case 'q':
      *action = Terminate;
      break;
    default:
      *action = Up;
      break;
  }
}

void gameOverScreen() {
  clear();
  mvprintw(BOARD_HEIGHT / 2 - 1, (BOARD_WIDTH * 2) / 2 - 6, "Game Over!");
  refresh();
}