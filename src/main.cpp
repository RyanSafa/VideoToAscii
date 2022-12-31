#include "../include/video_reader.h"
#include <_types/_uint8_t.h>
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
    static bool first_frame = true;
    int num_frames = 0;
    int64_t pts;

    steady_clock::time_point start;
    while (true) {
      if (!video_reader_read_frame(&vr_state, frame_data, &pts)) {
        printf("Couldn't load the video frame");
        video_reader_close(&vr_state);
        return 1;
      }
      num_frames++;
      if (first_frame) {
        first_frame = false;
        start = steady_clock::now();
      }

      double pt_double =
          pts * (double)vr_state.time_base.num / (double)vr_state.time_base.den;
      while (
          pt_double * 1000000 >
          (duration_cast<microseconds>(steady_clock::now() - start)).count()) {
        // do noting
      }
      // std::cout << "frame number: " << num_frames << " | ";
      // printf("pt_double: %f (micro) | ", pt_double * 1000000);
      // std::cout
      //     << "micro since first frame: "
      //     << (duration_cast<microseconds>(steady_clock::now() -
      //     start)).count()
      //     << "\n";
    }
    video_reader_close(&vr_state);
    auto time_after_24_frames = steady_clock::now();
    std::cout
        << "Time difference =  "
        << duration_cast<microseconds>(time_after_24_frames - start).count()
        << "micro\n";
  } else if (argc < 2) {
    std::cout << "No file was entered\n";
  } else {
    std::cout << "More than one file was entered\n";
  }
  return 0;
};
