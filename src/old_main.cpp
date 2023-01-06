// #include "../include/video_reader.h"
// #include "ncurses.h"
// #include <_types/_uint8_t.h>
// #include <chrono>
// #include <iostream>
// using namespace std::chrono;
//
// int main(int argc, char *argv[]) {
//   initscr();
//   curs_set(0);
//
//   if (argc == 2) {
//     std::atomic_bool continue_render = true;
//     VideoReaderState vr_state;
//     vr_state.sws_scalar_ctx = NULL;
//     if (!video_reader_open(&vr_state, argv[1])) {
//       printf("Couldn't open the file");
//       return 1;
//     }
//     uint8_t *frame_data = new uint8_t[vr_state.scaled_dimensions[0] *
//                                       vr_state.scaled_dimensions[1] * 4];
//     long frames_since_start = 0;
//     int64_t pts;
//     steady_clock::time_point start;
//     bool first_frame = true;
//
//     int inity, initx;
//     getmaxyx(stdscr, inity, initx);
//     WINDOW *win = newwin(
//         vr_state.scaled_dimensions[1] + 1, vr_state.scaled_dimensions[0] + 1,
//         0, round((initx / 2.0f) - (vr_state.scaled_dimensions[0] / 2.0f)));
//     cbreak();
//     noecho();
//     scrollok(win, TRUE);
//     nodelay(win, TRUE);
//     refresh();
//
//     do {
//       while (1) {
//         if (!video_reader_read_frame(&vr_state, frame_data, &pts, win)) {
//           break;
//         }
//
//         if (wgetch(win) == 'q') {
//           continue_render = false;
//           break;
//         }
//
//         frames_since_start++;
//         if ((frames_since_start - 1) % vr_state.num_frames == 0) {
//           start = steady_clock::now();
//         }
//         double expected_frame_time = pts * (double)vr_state.time_base.num /
//                                      (double)vr_state.time_base.den * 1000;
//         while (expected_frame_time >
//                (duration_cast<milliseconds>(steady_clock::now() - start))
//                    .count()) {
//         }
//       }
//     } while (seek_to_beginning(&vr_state) && continue_render);
//
//     video_reader_close(&vr_state);
//     wrefresh(win);
//   } else if (argc < 2) {
//     move(0, 0);
//     printw("No file was entered\n");
//   } else {
//     move(0, 0);
//     printw("MOre than one file was entered\n");
//   }
//   return 0;
// };
