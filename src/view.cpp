#include "../include/view.h"
#include <_types/_uint8_t.h>
#include <cmath>
#include <iostream>
#include <string>

void print_to_console(uint8_t *rgb_data, int width, int height) {
  std::string light_to_dark =
      "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  int ramp_length = light_to_dark.length();
  std::string row = "";
  for (int j = 0; j < height; j++) {
    std::string row = "";
    for (int i = 0; i < width; i++) {
      int pixel_index = (i + j * width) * 4;
      uint8_t r = rgb_data[pixel_index];
      uint8_t g = rgb_data[pixel_index + 1];
      uint8_t b = rgb_data[pixel_index + 2];
      uint8_t grey_scale_average = (r + g + b) / 3;
      int ascii_index = floor(((ramp_length - 1) * grey_scale_average) / 255);
      row += light_to_dark[ascii_index];
    }
    printf("\%s + \n", row.c_str());
  }
}
