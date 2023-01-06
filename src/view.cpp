#include "../include/view.h"
#include "ncurses.h"
#include <_types/_uint8_t.h>
#include <cmath>
#include <iostream>
#include <string>

void print_to_console(uint8_t *rgb_data, int width, int height, WINDOW *win) {

  std::string dark_to_light = " .,:ilwW";
  int RAMP_LENGTH = dark_to_light.length();
  std::string ascii_frame = "";

  for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
      int pixel_index = (j + i * width) * 4;
      uint8_t r = rgb_data[pixel_index];
      uint8_t g = rgb_data[pixel_index + 1];
      uint8_t b = rgb_data[pixel_index + 2];
      uint8_t grey_scale_average = (r + g + b) / 3;

      int ascii_index = floor(((RAMP_LENGTH - 1) * grey_scale_average) / 255);
      ascii_frame += dark_to_light[ascii_index];
    }
    ascii_frame += "\n";
  }

  wprintw(win, "\%s", ascii_frame.c_str());
  wmove(win, 0, 0);
  wrefresh(win);
}
