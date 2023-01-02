#pragma once
extern "C" {
#include "inttypes.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

struct VideoReaderState {
  int width, height;
  int *scaled_dimensions;
  AVRational time_base;
  int num_frames;

  AVFormatContext *av_format_context;
  AVCodecContext *av_codec_ctx;
  AVFrame *av_frame;
  int video_stream_index;
  AVPacket *av_packet;
  SwsContext *sws_scalar_ctx;
};

bool video_reader_open(VideoReaderState *state, const char *filename);

bool video_reader_read_frame(VideoReaderState *state, uint8_t *frame_buffer,
                             int64_t *pts);

bool video_reader_close(VideoReaderState *state);

int *get_new_dimension(int width, int height);

bool seek_to_beginning(VideoReaderState *state);
