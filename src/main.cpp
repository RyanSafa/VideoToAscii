#include "../include/load_frame.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
  if (argc == 2) {
    load_frame(argv[1]);
  } else if (argc < 2) {
    std::cout << "No file was entered\n";
  } else {
    std::cout << "More than one file was entered\n";
  }
  return 0;
};
