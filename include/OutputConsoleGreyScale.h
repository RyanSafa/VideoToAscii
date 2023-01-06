#pragma once
#include "./RenderFrameStrategy.h"
#include "ncurses.h"
#include <_types/_uint8_t.h>

class OutputConsoleGreyScale : public RenderFrameStrategy {
private:
  WINDOW *win;

public:
  OutputConsoleGreyScale(WINDOW *win);
  void renderAsciiFrame(int outputWidth, int outputHeight, uint8_t *frameData);
};
