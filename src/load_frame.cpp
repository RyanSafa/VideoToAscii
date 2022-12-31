#include <_types/_uint8_t.h>
#include <cmath>
#include <iostream>
extern "C" {
#include "inttypes.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include "../include/load_frame.h"

void print_to_console(uint8_t *rgb_data, int width, int height) {
  // std::string light_to_dark = " .:-=+*#%@";
  std::string light_to_dark =
      "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
  int ramp_length = light_to_dark.length();

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
    std::cout << row + "\n";
  }
}

int *get_new_dimension(int width, int height) {
  int MAX_HEIGHT = 120;
  int MAX_WIDTH = 140;

  if (height > MAX_HEIGHT) {
    int reduced_width = floor((width * MAX_HEIGHT) / height);
    int static tuple[] = {reduced_width, MAX_HEIGHT};
    return tuple;
  }
  if (width > MAX_WIDTH) {
    int reduced_height = floor((width * MAX_WIDTH) / width);
    int static tuple[] = {MAX_WIDTH, reduced_height};
    return tuple;
  }

  int static tuple[] = {width, height};
  return tuple;
}

bool load_frame(const char *filename) {

  AVFormatContext *av_format_context = avformat_alloc_context();

  if (!av_format_context) {
    std::cout << "Couldn't create AVFormatContext\n";
    return false;
  }

  if (avformat_open_input(&av_format_context, filename, NULL, NULL) != 0) {
    std::cout << "Couldn't open the video file\n";
    return false;
  }

  int video_stream_index = -1;
  int width;
  int height;
  AVCodecParameters *av_codec_params;
  const AVCodec *av_codec;

  for (int i = 0; i < av_format_context->nb_streams; i++) {
    auto stream = av_format_context->streams[i];
    av_codec_params = stream->codecpar;
    av_codec = avcodec_find_decoder(av_codec_params->codec_id);

    if (!av_codec) {
      continue;
    }

    if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
      video_stream_index = i;
      height = av_codec_params->height;
      width = av_codec_params->width;
      break;
    }
  }

  if (video_stream_index == -1) {
    std::cout << "Couldn't find decoder\n";
    return false;
  }

  AVCodecContext *av_codec_ctx = avcodec_alloc_context3(av_codec);

  if (!av_codec_ctx) {
    std::cout << "Couldn't create AVCodecContext\n";
    return false;
  }
  if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
    std::cout << "Couldn't initialize AVCodecContext\n";
  }

  if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    std::cout << "Couldn't open codec\n";
    return false;
  }

  AVFrame *av_frame = av_frame_alloc();
  if (!av_frame) {
    std::cout << "Couldn't allocate frame\n";
  }
  AVPacket *av_packet = av_packet_alloc();

  if (!av_packet) {
    std::cout << "Couldn't allocate packet\n";
  }

  int response;
  while (av_read_frame(av_format_context, av_packet) >= 0) {
    if (av_packet->stream_index != video_stream_index) {
      continue;
    }

    response = avcodec_send_packet(av_codec_ctx, av_packet);

    if (response < 0) {
      std::cout << "failed to decode packet: " +
                       std::string(av_err2str(response)) + "\n";
      return false;
    }

    response = avcodec_receive_frame(av_codec_ctx, av_frame);

    if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
      continue;
    } else if (response < 0) {
      std::cout << "Failed to decode packet: " +
                       std::string(av_err2str(response)) + "\n";
      return false;
    }

    av_packet_unref(av_packet);
    break;
  }

  int *new_dimensions = get_new_dimension(width, height);

  SwsContext *sws_scalar_ctx = sws_getContext(
      width, height, av_codec_ctx->pix_fmt, new_dimensions[0],
      new_dimensions[1], AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);

  if (!sws_scalar_ctx) {
    std::cout << "Couldnt scale";
    return false;
  }

  uint8_t *data = new uint8_t[new_dimensions[0] * new_dimensions[1] * 4];

  uint8_t *dest[4] = {data, NULL, NULL, NULL};
  int dest_linesize[4] = {new_dimensions[0] * 4, 0, 0, 0};
  sws_scale(sws_scalar_ctx, av_frame->data, av_frame->linesize, 0, height, dest,
            dest_linesize);

  print_to_console(data, new_dimensions[0], new_dimensions[1]);

  avformat_close_input(&av_format_context);
  avformat_free_context(av_format_context);
  av_frame_free(&av_frame);
  av_packet_free(&av_packet);
  avcodec_free_context(&av_codec_ctx);

  return true;
}
