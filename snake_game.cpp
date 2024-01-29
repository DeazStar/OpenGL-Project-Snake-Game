#include <GL/glut.h>
#include <iostream>
#include <sstream>
#include <vector>

int gridWidth = 30;
int gridHeight = 20;
float squareLeft;
float squareRight;
float squareTop;
float squareBottom;
int direction = 1;
int previousDirectionState;
int score = 0;
int highestScore = 0;

void generateFood();

enum State { START_SCREEN, IN_GAME, GAME_OVER };

enum Level { EASY, MEDIUM };

State gameState = START_SCREEN;
Level currentLevel = EASY;

struct SnakeSegment {
  int x, y;
};

std::vector<SnakeSegment> snake;
int foodX, foodY;

void drawSquare(int x, int y) {
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex2i(x, y);
  glVertex2i(x + 1, y);
  glVertex2i(x + 1, y + 1);
  glVertex2i(x, y + 1);
  glEnd();

  // draw a stroke
  glColor3f(0.0, 0.5, 0.0); // dark green
  glLineWidth(2.0);
  glBegin(GL_LINE_LOOP);
  glVertex2f(x, y);
  glVertex2f(x + 1, y);
  glVertex2f(x + 1, y + 1);
  glVertex2f(x, y + 1);
  glEnd();
}

void drawSnake() {
  for (auto segment : snake) {
    drawSquare(segment.x, segment.y);
  }
}

void drawFood(int foodX, int foodY) {
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_POLYGON);
  glVertex2i(foodX, foodY);
  glVertex2i(foodX + 1, foodY);
  glVertex2i(foodX + 1, foodY + 1);
  glVertex2i(foodX, foodY + 1);
  glEnd();
}

void drawMediumLevelObstacle() {
  // Calculate center coordinates of the screen
  float centerX = gridWidth / 2.0f;
  float centerY = gridHeight / 2.0f;

  float squareSize = std::max(gridWidth, gridHeight) / 4.0f;

  // Calculate the coordinates of the square corners
  squareLeft = centerX - squareSize / 2.0f;
  squareRight = centerX + squareSize / 2.0f;
  squareTop = centerY + squareSize / 2.0f;
  squareBottom = centerY - squareSize / 2.0f;

  // Draw the square
  glColor3f(0.0f, 0.0f, 1.0f); // Blue color
  glBegin(GL_POLYGON);
  glVertex2f(squareLeft, squareBottom);
  glVertex2f(squareRight, squareBottom);
  glVertex2f(squareRight, squareTop);
  glVertex2f(squareLeft, squareTop);
  glEnd();
}

void drawStartScreen() {
  std::string startMessage = "Press Insert to Start";

  glRasterPos2f(1, gridHeight - 1); // allow for the bitmap characters to
                                    // display at the top of the screen
  for (char c : startMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }

  std::string levelMessage = "Level: ";
  if (currentLevel == EASY) {
    levelMessage += "Easy (Press Up/Down to change)";
  } else {
    levelMessage += "Medium (Press Up/Down to change)";
  }

  glRasterPos2f(1, gridHeight - 2);
  for (char c : levelMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
}

void drawScore() {
  glColor3f(1.0, 1.0, 1.0);
  glRasterPos2f(1, gridHeight - 1);
  std::string scoreText;
  scoreText = "Score: " + std::to_string(score);
  for (char c : scoreText) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
}

void drawGameOverScreen() {
  glColor3f(1.0, 1.0, 1.0);
  std::string gameOverMessage =
      "Game Over! Your score: " + std::to_string(score);

  glRasterPos2f(1, gridHeight - 1); // Display at the top-left corner
  for (char c : gameOverMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }

  std::string highestScoreMessage =
      "Highest score: " + std::to_string(highestScore);
  glRasterPos2f(1, gridHeight - 2);
  for (char c : highestScoreMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }

  std::string exitMessage = "Press End to Exit";
  glRasterPos2f(1, gridHeight - 3);
  for (char c : exitMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }

  std::string startOverMessage = "Press Insert to Start Over";
  glRasterPos2f(1, gridHeight - 4);
  for (char c : startOverMessage) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT);

  if (gameState == IN_GAME) {
    // draw the snake
    drawSnake();

    // draw the food
    drawFood(foodX, foodY);

    // Draw obstacle for medium level
    if (currentLevel == MEDIUM) {
      drawMediumLevelObstacle();
    }

    // draw the score
    drawScore();
  } else if (gameState == START_SCREEN) {
    drawStartScreen();
  } else if (gameState == GAME_OVER) {
    drawGameOverScreen();
  }

  glutSwapBuffers();
}

void update(int value) {
  if (gameState == IN_GAME) {
    // Update snake position
    SnakeSegment head = snake.front();
    switch (direction) {
    case 0: // move the snake to the up
      if (previousDirectionState != 2) {
        head.y++;
      } else {
        direction = previousDirectionState;
        head.y--;
      }
      break;
    case 1: // move the snake to the right
      if (previousDirectionState != 3) {
        head.x++;
      } else {
        direction = previousDirectionState;
        head.x--;
      }
      break;
    case 2: // move the snake to the down
      if (previousDirectionState != 0) {
        head.y--;
      } else {
        direction = previousDirectionState;
        head.y++;
      }
      break;
    case 3: // move the snake to the left
      if (previousDirectionState != 1) {
        head.x--;
      } else {
        direction = previousDirectionState;
        head.x++;
      }
      break;
    }

    // Check for collisions with own body
    for (int i = 1; i < snake.size(); ++i) {
      if (head.x == snake[i].x && head.y == snake[i].y) {
        gameState = GAME_OVER;
        glutPostRedisplay();
        return;
      }
    }

    // Check for collisions with walls
    if (head.x < 0 || head.x >= gridWidth || head.y < 0 ||
        head.y >= gridHeight) {
      gameState = GAME_OVER;
      glutPostRedisplay();
      return;
    }

    if (currentLevel == MEDIUM) {
      if (head.x >= squareLeft && head.x <= squareRight &&
          head.y >= squareBottom && head.y <= squareTop) {
        gameState = GAME_OVER;
        glutPostRedisplay();
        return;
      }
    }

    // Check for collision with food
    if (head.x == foodX && head.y == foodY) {
      score++;

      // Create a temporary SnakeSegment object and push it back
      SnakeSegment newSegment = {foodX, foodY};
      snake.push_back(newSegment);

      generateFood();
    }

    // Move the snake
    snake.pop_back();
    snake.insert(snake.begin(), head);

    glutPostRedisplay();
    glutTimerFunc(100, update, 0); // Update every 100 milliseconds
  }
}

void generateFood() {
  if (currentLevel == MEDIUM) {
    // Determine valid range of x and y coordinates for the food
    int minX = squareRight + 1;
    int maxX = gridWidth;
    int minY = squareTop + 1;
    int maxY = gridHeight;

    // Generate random coordinates within the valid range
    foodX = minX + rand() % (maxX - minX);
    foodY = minY + rand() % (maxY - minY);

  } else {
    foodX = rand() % gridWidth;
    foodY = rand() % gridHeight;
  }
}

void specialKeys(int key, int x, int y) {
  if (gameState == START_SCREEN) {
    if (key == GLUT_KEY_INSERT) {
      gameState = IN_GAME;
      snake.push_back({0, 0});
      generateFood();
      update(0);
    } else if (key == GLUT_KEY_UP || key == GLUT_KEY_DOWN) {
      currentLevel = (currentLevel == EASY) ? MEDIUM : EASY;
      glutPostRedisplay();
    }
  } else if (gameState == IN_GAME) {
    switch (key) {
    case GLUT_KEY_UP:
      previousDirectionState = direction;
      direction = 0;
      break;
    case GLUT_KEY_DOWN:
      previousDirectionState = direction;
      direction = 2;
      break;
    case GLUT_KEY_RIGHT:
      previousDirectionState = direction;
      direction = 1;
      break;
    case GLUT_KEY_LEFT:
      previousDirectionState = direction;
      direction = 3;
      break;
    }
  } else if (gameState == GAME_OVER) {
    if (key == GLUT_KEY_END) { // END KEY
      exit(0);
    } else if (key == GLUT_KEY_INSERT) {
      gameState = START_SCREEN;
      highestScore = score;
      score = 0;
      snake.clear();
      glutPostRedisplay();
    }
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(800, 600);
  glutCreateWindow("Simple Snake Game");

  glutDisplayFunc(display);
  glutSpecialFunc(specialKeys);

  glClearColor(0.0, 0.0, 0.0, 1.0);
  gluOrtho2D(0, gridWidth, 0, gridHeight);

  glutMainLoop();

  return 0;
}
