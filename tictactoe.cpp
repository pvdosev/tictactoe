#include "FL/Fl.H"
#include "FL/Fl_Box.H"
#include "FL/Fl_Button.H"
#include "FL/Fl_Group.H"
#include "FL/Fl_Window.H"
#include <cstring>

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
};

class GameBoard : public Fl_Group {
  GameButton *gameButtons[8];
  BoxStates playerChoice = Empty;
  BoxStates currentPlayer = Cross;

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
  }
  end();
}

GameButton::GameButton(int x, int y, int w, int h, const char *label)
    : Fl_Button(x, y, w, h, label) {}

void GameBoard::staticGameCallback(Fl_Widget *widget, void *context) {
  ((GameBoard *)context)->gameCallback(widget);
}
void GameBoard::gameCallback(Fl_Widget *widget) {
  widget->label(getLabel(this->playerChoice));
}

struct Screens {
  GameBoard *currGame;
  Fl_Group *currChooser;
};

void chooserCallback(Fl_Widget *button, void *voidPtr) {
  // very ugly, blame FLTK for only allowing void pointers
  // to be passed in callback functions
  Screens *screenPtr = static_cast<Screens *>(voidPtr);
  screenPtr->currGame->playerChoice =
      strcmp(button->label(), "X") ? Circle : Cross;
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
