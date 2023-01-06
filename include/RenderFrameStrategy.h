#pragma once
#include "ncurses.h"
#include <_types/_uint8_t.h>

// interface for rendering a frame incase we want different render ascii with
// color, sound, etc.
class RenderFrameStrategy {
public:
  virtual void renderAsciiFrame(int outputHeight, int outputWidth,
                                uint8_t *frameData) = 0;
};
