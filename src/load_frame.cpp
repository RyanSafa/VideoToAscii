#include <iostream>
extern "C" {
#include "inttypes.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
}
#include "../include/load_frame.h"

bool load_frame(const char *filename) {

  AVFormatContext *av_format_context = avformat_alloc_context();

  if (!av_format_context) {
    std::cout << "Couldn't create AVFormatContext\n";
    return false;
  }

  if (!avformat_open_input(&av_format_context, filename, NULL, NULL)) {
    std::cout << "Couldn't open the video file\n";
    return false;
  }
  std::cout << "Opened video file\n";
  return true;
}
