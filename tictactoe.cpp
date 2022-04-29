#include "FL/Fl.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Window.H"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <set>
#include <vector>

enum BoxStates { Empty, Cross, Circle };
const char *getLabel(BoxStates state) {
  if (state == Empty) {
    return "";
  }
  return state == Cross ? "X" : "O";
}

class GameButton : public Fl_Button {
  BoxStates currState = Empty;

public:
  GameButton(int x, int y, int w, int h, const char *label = 0);
  int index;
};

class GameBoard : public Fl_Group {
  GameButton *gameButtons[8];
  BoxStates playerChoice = Empty;
  std::vector<int> playerMoves;
  std::vector<int> cpuMoves;

  // We need a static callback, so we can get the context of the
  // current object (since it's getting called from the outside)
  // yes it's gross and hacky and it uses only void pointers
  static void staticGameCallback(Fl_Widget *widget, void *context);
  void gameCallback(Fl_Widget *widget);
  friend GameButton;
  friend void chooserCallback(Fl_Widget *button, void *voidPtr);

public:
  GameBoard(int x, int y, int w, int h, const char *label = 0);
};

GameBoard::GameBoard(int x, int y, int w, int h, const char *label)
    : Fl_Group(x, y, w, h) {
  int buttonWidth = w / 3;
  int buttonHeight = h / 3;
  int buttonX = 0;
  int buttonY = 0;
  for (int i = 0; i < 9; i++) {
    int buttonX = (i % 3) * buttonWidth;
    int buttonY = (i / 3) * buttonHeight;
    gameButtons[i] =
        new GameButton(buttonX, buttonY, buttonWidth, buttonHeight);
    gameButtons[i]->callback(staticGameCallback, (void *)this);
    gameButtons[i]->labelsize(50);
    gameButtons[i]->index = i;
  }
  end();
}

GameButton::GameButton(int x, int y, int w, int h, const char *label)
    : Fl_Button(x, y, w, h, label) {}

// A fun way to check if board is a winning board!
// for more info see https://mathworld.wolfram.com/MagicSquare.html
const int MAGICSQUARE[9] = {8, 1, 6, 3, 5, 7, 4, 9, 2};
const bool isWinning(std::vector<int> moves) {
  int setSize = moves.size();
  // can't win with less than 3 moves
  if (setSize < 3) {
    return false;
  }
  int sum = 0;

  // helper vector to generate combinations
  std::vector<bool> comb(setSize);
  std::fill(comb.begin(), comb.begin() + 3, true);

  // check every combination of 3 moves.
  // if it equals 15 (the sum of our magic square),
  // return true. else return false
  do {
    sum = 0;
    for (int i = 0; i < setSize; ++i) {
      if (comb[i]) {
        sum += MAGICSQUARE[moves[i] - 1];
      }
    }
    if (sum == 15) {
      return true;
    }
  } while (std::prev_permutation(comb.begin(), comb.end()));

  return false;
}

int miniMax(std::vector<int> playerMoves, std::vector<int> cpuMoves) {}

void GameBoard::staticGameCallback(Fl_Widget *widget, void *context) {
  ((GameBoard *)context)->gameCallback(widget);
}
void GameBoard::gameCallback(Fl_Widget *widget) {
  widget->label(getLabel(this->playerChoice));
  this->playerMoves.insert(this->playerMoves.end(),
                           ((GameButton *)widget)->index + 1);
}

struct Screens {
  GameBoard *currGame;
  Fl_Group *currChooser;
};

void chooserCallback(Fl_Widget *button, void *voidPtr) {
  // very ugly, blame FLTK for only allowing void pointers
  // to be passed in callback functions
  Screens *screenPtr = static_cast<Screens *>(voidPtr);
  if (not strcmp(button->label(), "X")) {
    screenPtr->currGame->playerChoice = Cross;
  } else {
    screenPtr->currGame->playerChoice = Circle;
    // guaranteed to be the optimal move for X   :)
    screenPtr->currGame->gameButtons[0]->label("X");
    screenPtr->currGame->cpuMoves.insert(screenPtr->currGame->cpuMoves.end(),
                                         1);
  }
  screenPtr->currChooser->hide();
  screenPtr->currGame->show();
}

int main() {
  Fl_Window mainWindow(500, 500, "Tic Tac Toe");
  GameBoard mainGame(0, 0, 500, 500);
  mainGame.hide();
  Fl_Group chooser(0, 0, 500, 500);
  Fl_Box dialogBox(50, 50, 400, 70, "Select an option\n to start the game:");
  dialogBox.labelsize(25);
  Fl_Button choiceX(200, 130, 100, 30, "X");
  Fl_Button choiceO(200, 170, 100, 30, "O");
  choiceX.labelsize(20);
  choiceO.labelsize(20);

  Screens currScreens = {&mainGame, &chooser};
  choiceX.callback(chooserCallback, &currScreens);
  choiceO.callback(chooserCallback, &currScreens);
  chooser.end();
  mainWindow.show();
  return Fl::run();
}
