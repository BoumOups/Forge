#include "cli/interface.h++"

#include <cstdlib>

int main(const int argc, const char *argv[]) {
  if (argc < 2) {
    forge::Interface::print_help();
  }
  forge::Interface::parse_command(argv[1]);

  return EXIT_SUCCESS;
}