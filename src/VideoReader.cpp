#include "../include/VideoReader.h"
#include "RenderFrameStrategy.h"
#include "ncurses.h"
extern "C" {
#include "inttypes.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

void VideoReader::get_scaled_dimensions(int initTerminalRows) {
  double reduced = (double)initTerminalRows / srcHeight * 100;
  this->scaledWidth = floor(srcWidth * reduced / 100);
  this->scaledHeight = floor(srcHeight * reduced / 100);
}

VideoReader::VideoReader(const char *filename, int initTerminalRows) {
  avFormatContext = avformat_alloc_context();
  swsScalarContext = NULL;
  if (!avFormatContext) {
    throw "Failed to allocate AVFormatContext\n";
  }

  if (avformat_open_input(&avFormatContext, filename, NULL, NULL) != 0) {
    throw "Failed to open the video file\n";
  }

  videoStreamIndex = -1;
  AVCodecParameters *avCodecParams;
  const AVCodec *avCodec;

  for (int i = 0; i < avFormatContext->nb_streams; i++) {
    auto stream = avFormatContext->streams[i];
    avCodecParams = stream->codecpar;
    avCodec = avcodec_find_decoder(avCodecParams->codec_id);

    if (!avCodec) {
      continue;
    }

    if (avCodecParams->codec_type == AVMEDIA_TYPE_VIDEO) {
      videoStreamIndex = i;
      srcHeight = avCodecParams->height;
      srcWidth = avCodecParams->width;
      timeBase = avFormatContext->streams[i]->time_base;
      break;
    }
  }

  if (videoStreamIndex == -1) {
    throw "Failed to find video decoder\n";
  }

  avCodecContext = avcodec_alloc_context3(avCodec);

  if (!avCodecContext) {
    throw "Failed to allocate AVCodecContext\n";
  }

  if (avcodec_parameters_to_context(avCodecContext, avCodecParams) < 0) {
    throw "Failed to initialize AVCodecContext\n";
  }

  if (avcodec_open2(avCodecContext, avCodec, NULL) < 0) {
    throw "Failed to open codec\n";
  }

  avFrame = av_frame_alloc();

  if (!avFrame) {
    throw "Failed to allocate frame\n";
  }
  avPacket = av_packet_alloc();

  if (!avPacket) {
    throw "Failed to allocate packet\n";
  }

  numFrames = avFormatContext->streams[videoStreamIndex]->nb_frames;
  get_scaled_dimensions(initTerminalRows);
}

bool VideoReader::read_file(uint8_t *frameBuffer, int64_t *pts,
                            RenderFrameStrategy *renderFrameStrategy) {
  int res;
  bool endOfFile = true;

  while (av_read_frame(avFormatContext, avPacket) >= 0) {

    if (avPacket->stream_index != videoStreamIndex) {
      av_packet_unref(avPacket);
      continue;
    }

    endOfFile = false;
    res = avcodec_send_packet(avCodecContext, avPacket);

    if (res < 0) {
      return false;
    }

    res = avcodec_receive_frame(avCodecContext, avFrame);

    if (res == AVERROR(EAGAIN)) {
      av_packet_unref(avPacket);
      continue;
    } else if (res == AVERROR_EOF || res < 0) {
      return false;
    }

    av_packet_unref(avPacket);
    break;
  }

  *pts = avFrame->pts;

  if (endOfFile) {
    return false;
  }

  if (!swsScalarContext) {
    swsScalarContext = sws_getContext(
        srcWidth, srcHeight, avCodecContext->pix_fmt, scaledWidth, scaledHeight,
        AV_PIX_FMT_RGB0, SWS_BILINEAR, NULL, NULL, NULL);
  }

  if (!swsScalarContext) {
    return false;
  }

  uint8_t *dest[4] = {frameBuffer, NULL, NULL, NULL};
  int dest_linesize[4] = {scaledWidth * 4, 0, 0, 0};
  sws_scale(swsScalarContext, avFrame->data, avFrame->linesize, 0, srcHeight,
            dest, dest_linesize);

  renderFrameStrategy->renderAsciiFrame(scaledHeight, scaledWidth, frameBuffer);
  return true;
}

bool VideoReader::seek(int64_t frame) {
  int64_t tb;
  tb = (int64_t(timeBase.num) * AV_TIME_BASE) / int64_t(timeBase.den);
  int64_t seekTarget = frame * tb;

  if (av_seek_frame(avFormatContext, videoStreamIndex, seekTarget,
                    AVSEEK_FLAG_FRAME) >= 0) {
    return true;
  }
  return false;
}

VideoReader::~VideoReader() {
  sws_freeContext(swsScalarContext);
  avcodec_free_context(&avCodecContext);
  avformat_close_input(&avFormatContext);
  avformat_free_context(avFormatContext);
  av_frame_free(&avFrame);
  av_packet_free(&avPacket);
}
