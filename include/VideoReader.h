#pragma once
#include "./RenderFrameStrategy.h"
#include <_types/_uint64_t.h>
#include <_types/_uint8_t.h>
#include <libavutil/rational.h>
extern "C" {
#include "inttypes.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

class VideoReader {
private:
  // instance variables
  AVFormatContext *avFormatContext;
  AVCodecContext *avCodecContext;
  AVFrame *avFrame;
  AVPacket *avPacket;
  SwsContext *swsScalarContext;
  AVRational timeBase;
  int videoStreamIndex, srcWidth, srcHeight, scaledWidth, scaledHeight,
      numFrames;

  // private variables
  void get_scaled_dimensions(int initTerminalRows);

public:
  VideoReader(const char *filename, int initTerminalRows);
  bool read_file(uint8_t *frameBuffer, int64_t *pts,
                 RenderFrameStrategy *renderFrameStrategy);
  bool seek(int64_t frame);
  ~VideoReader();
  int getScaledWidth() { return scaledWidth; }
  int getScaledHeight() { return scaledHeight; }
  int getNumFrames() { return numFrames; }
  AVRational getTimeBase() { return timeBase; }
};
