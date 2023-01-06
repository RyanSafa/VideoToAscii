#include "../include/OutputConsoleGreyScale.h"
#include <_types/_uint8_t.h>
#include <cmath>
#include <string>

OutputConsoleGreyScale::OutputConsoleGreyScale(WINDOW *win) {
  this->win = win;
};

void OutputConsoleGreyScale::renderAsciiFrame(int outputHeight, int outputWidth,
                                              uint8_t *frameData) {
  std::string darkToLight = " .,:ilwW";
  int RAMP_LENGTH = darkToLight.length();
  std::string asciiFrame = "";

  for (int i = 0; i < outputHeight; i++) {
    for (int j = 0; j < outputWidth; j++) {
      int pixelIndex = (j + i * outputWidth) * 4;

      uint8_t r = frameData[pixelIndex];
      uint8_t g = frameData[pixelIndex + 1];
      uint8_t b = frameData[pixelIndex + 2];
      uint8_t greyScaleAverage = (r + g + b) / 3;

      int asciiIndex = floor(((RAMP_LENGTH - 1) * greyScaleAverage) / 255);
      asciiFrame += darkToLight[asciiIndex];
    }
    asciiFrame += "\n";
  }

  wprintw(win, "\%s", asciiFrame.c_str());
  wmove(win, 0, 0);
  wrefresh(win);
}
