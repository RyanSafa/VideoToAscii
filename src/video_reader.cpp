#include "../include/video_reader.h"
#include "../include/view.h"
#include <cinttypes>
#include <iostream>
#include <libavformat/avformat.h>
#include <libavutil/error.h>
#include <libavutil/rational.h>
#include <libswscale/swscale.h>
#include <sys/_types/_int64_t.h>

int *get_new_dimension(int width, int height) {
  int MAX_HEIGHT = 60;
  int MAX_WIDTH = 80;

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

bool video_reader_open(VideoReaderState *state, const char *filename) {
  auto &av_format_context = state->av_format_context;
  auto &av_codec_ctx = state->av_codec_ctx;
  auto &av_frame = state->av_frame;
  auto &av_packet = state->av_packet;
  auto &video_stream_index = state->video_stream_index;
  auto &width = state->width;
  auto &height = state->height;
  auto &time_base = state->time_base;
  auto &num_frames = state->num_frames;
  av_format_context = avformat_alloc_context();

  if (!av_format_context) {
    std::cout << "Failed to create AVFormatContext\n";
    return false;
  }

  if (avformat_open_input(&av_format_context, filename, NULL, NULL) != 0) {
    std::cout << "Failed to open the video file\n";
    return false;
  }

  video_stream_index = -1;
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
      time_base = av_format_context->streams[i]->time_base;
      break;
    }
  }

  if (video_stream_index == -1) {
    std::cout << "Failed to find decoder\n";
    return false;
  }

  av_codec_ctx = avcodec_alloc_context3(av_codec);

  if (!av_codec_ctx) {
    std::cout << "Failed to create AVCodecContext\n";
    return false;
  }
  if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
    std::cout << "Failed to initialize AVCodecContext\n";
  }

  if (avcodec_open2(av_codec_ctx, av_codec, NULL) < 0) {
    std::cout << "Failed to open codec\n";
    return false;
  }

  av_frame = av_frame_alloc();
  if (!av_frame) {
    std::cout << "Failed to allocate frame\n";
  }
  av_packet = av_packet_alloc();

  if (!av_packet) {
    std::cout << "Failed to allocate packet\n";
  }

  state->scaled_dimensions = get_new_dimension(width, height);

  num_frames = state->av_format_context->streams[video_stream_index]->nb_frames;
  return true;
}

bool video_reader_read_frame(VideoReaderState *state, uint8_t *frame_buffer,
                             int64_t *pts) {
  // unpack
  auto &av_format_context = state->av_format_context;
  auto &av_codec_ctx = state->av_codec_ctx;
  auto &av_frame = state->av_frame;
  auto &av_packet = state->av_packet;
  auto &video_stream_index = state->video_stream_index;
  auto &width = state->width;
  auto &height = state->height;
  auto &scaled_dimensions = state->scaled_dimensions;
  auto &sws_scalar_ctx = state->sws_scalar_ctx;

  int response;
  int inside = 0;

  while (av_read_frame(av_format_context, av_packet) >= 0) {
    if (av_packet->stream_index != video_stream_index) {
      av_packet_unref(av_packet);
      continue;
    }
    inside = 1;
    response = avcodec_send_packet(av_codec_ctx, av_packet);

    if (response == AVERROR_EOF) {
      std::cout << "NULL";
    }
    if (response < 0) {
      std::cout << "Failed to decode packet: " +
                       std::string(av_err2str(response)) + "\n";
      return false;
    }

    response = avcodec_receive_frame(av_codec_ctx, av_frame);

    if (response == AVERROR(EAGAIN)) {
      av_packet_unref(av_packet);
      continue;
    } else if (response == AVERROR_EOF) {
      std::cout << "EOF\n";
    } else if (response < 0) {
      std::cout << "Failed to decode packet: " +
                       std::string(av_err2str(response)) + "\n";
      return false;
    }
    av_packet_unref(av_packet);
    break;
  }
  *pts = av_frame->pts;

  if (!inside) {
    return false;
  } else {
    if (!sws_scalar_ctx) {
      sws_scalar_ctx =
          sws_getContext(width, height, av_codec_ctx->pix_fmt,
                         scaled_dimensions[0], scaled_dimensions[1],
                         AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
    }
    if (!sws_scalar_ctx) {
      std::cout << "Failed to create scalar context\n";
      return false;
    }

    uint8_t *dest[4] = {frame_buffer, NULL, NULL, NULL};
    int dest_linesize[4] = {scaled_dimensions[0] * 4, 0, 0, 0};
    sws_scale(sws_scalar_ctx, av_frame->data, av_frame->linesize, 0, height,
              dest, dest_linesize);

    print_to_console(frame_buffer, scaled_dimensions[0], scaled_dimensions[1]);
  }
  return true;
}

bool seek_to_beginning(VideoReaderState *state) {
  auto &time_base = state->time_base;
  auto &av_format_context = state->av_format_context;
  auto &video_stream_index = state->video_stream_index;

  int64_t timeBase;
  timeBase = (int64_t(state->time_base.num) * AV_TIME_BASE) /
             int64_t(state->time_base.den);
  int64_t seekTarget = int64_t(0) * timeBase;

  if (av_seek_frame(av_format_context, video_stream_index, seekTarget,
                    AVSEEK_FLAG_FRAME) >= 0) {
    return true;
  }
  return false;
}

bool video_reader_close(VideoReaderState *state) {
  sws_freeContext(state->sws_scalar_ctx);
  avcodec_free_context(&state->av_codec_ctx);
  avformat_close_input(&state->av_format_context);
  avformat_free_context(state->av_format_context);
  av_frame_free(&state->av_frame);
  av_packet_free(&state->av_packet);
  return true;
}
