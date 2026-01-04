#pragma once

namespace forge {

  struct Command {
    enum class Type { Build, Run, Clean };
    Type type;
  };

  class Interface {
  public:
    static void parse_command(const char* command);
    static void run_command(const Command& command);

    static void print_help();
  };
}
