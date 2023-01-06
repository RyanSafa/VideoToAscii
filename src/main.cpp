#include "../include/OutputConsoleGreyScale.h"
#include "../include/RenderFrameStrategy.h"
#include "../include/VideoReader.h"
#include "ncurses.h"
#include <_types/_uint8_t.h>
#include <atomic>
#include <chrono>
#include <iostream>
#include <unistd.h>
using namespace std::chrono;

int main(int argc, char *argv[]) {
  initscr();
  if (argc == 2) {
    int initY, initX;
    getmaxyx(stdscr, initY, initX);
    VideoReader *videoReader;
    try {
      videoReader = new VideoReader(argv[1], initY);
    } catch (const char *failureMessage) {
      endwin();
      std::cout << failureMessage;
      return 1;
    }

    int scaledWidth = videoReader->getScaledWidth();
    int scaledHeight = videoReader->getScaledHeight();

    WINDOW *win = newwin(scaledHeight + 1, scaledWidth + 1, 0,
                         round((initX / 2.0) - (scaledWidth / 2.0)));
    curs_set(0);
    cbreak();
    noecho();
    scrollok(win, TRUE);
    nodelay(win, TRUE);
    wrefresh(win);

    RenderFrameStrategy *renderFrameStrategy = new OutputConsoleGreyScale(win);
    std::atomic_bool continueRender = true;
    uint8_t *frameBuffer = new uint8_t[scaledWidth * scaledHeight * 4];
    int framesSinceStart = 0;
    int64_t pts;
    steady_clock::time_point start;

    do {
      while (1) {

        if (!videoReader->read_file(frameBuffer, &pts, renderFrameStrategy)) {
          break;
        }

        if (wgetch(win) == 'q') {
          continueRender = false;
          break;
        }

        framesSinceStart++;

        if ((framesSinceStart - 1) % videoReader->getNumFrames() == 0) {
          start = steady_clock::now();
        }

        double expectedFrameTime =
            pts * (double)videoReader->getTimeBase().num /
            (double)videoReader->getTimeBase().den * 1000;

        while (expectedFrameTime >
               (duration_cast<milliseconds>(steady_clock::now() - start))
                   .count()) {
          // do nothing
        }
      }
    } while (videoReader->seek(0) && continueRender);

    delete videoReader;
    endwin();
  } else {
    endwin();
    std::cout << "Please enter a full path to a video file\n";
    return 1;
  }
  return 0;
}
