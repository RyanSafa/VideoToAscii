#include "../include/video_reader.h"
#include <_types/_uint8_t.h>
#include <arm/signal.h>
#include <chrono>
#include <iostream>
#include <ratio>
using namespace std::chrono;

int main(int argc, char *argv[]) {
  if (argc == 2) {
    VideoReaderState vr_state;
    vr_state.sws_scalar_ctx = NULL;
    if (!video_reader_open(&vr_state, argv[1])) {
      printf("Couldn't open the file");
      return 1;
    }
    uint8_t *frame_data = new uint8_t[vr_state.scaled_dimensions[0] *
                                      vr_state.scaled_dimensions[1] * 4];
    long frames_since_start = 0;
    int64_t pts;
    steady_clock::time_point start;
    bool first_frame = true;

    do {
      while (1) {
        if (!video_reader_read_frame(&vr_state, frame_data, &pts)) {
          break;
        }
        frames_since_start++;
        if ((frames_since_start - 1) % vr_state.num_frames == 0) {
          start = steady_clock::now();
        }
        double expected_frame_time = pts * (double)vr_state.time_base.num /
                                     (double)vr_state.time_base.den * 1000;
        while (expected_frame_time >
               (duration_cast<milliseconds>(steady_clock::now() - start))
                   .count()) {
        }
        std::cout << "frame number: " << frames_since_start << " | ";
        printf("Expected frame time: %f (ms) | ", expected_frame_time);
        std::cout << "ms since first frame (real): "
                  << (duration_cast<milliseconds>(steady_clock::now() - start))
                         .count()
                  << "\n";
      }
    } while (seek_to_beginning(&vr_state));

  } else if (argc < 2) {
    std::cout << "No file was entered\n";
  } else {
    std::cout << "More than one file was entered\n";
  }
  return 0;
};
