#include "interface.h++"

#include <print>
#include <string_view>

#include "cli/log.h++"
#include "core/core.h++"

void forge::Interface::parse_command(const char *command) {
  std::string_view cmd(command);
  if (cmd == "build") {
    run_command({Command::Type::Build});
  } else if (cmd == "run") {
    run_command({Command::Type::Run});
  } else if (cmd == "clean") {
    run_command({Command::Type::Clean});
  } else {
    message::log(message::Level::Error,
                        std::format("Unknown command '{}'", command));
    print_help();
  }
}

void forge::Interface::run_command(const Command &command) {
  switch (command.type) {
  case Command::Type::Build:
    Builder::build_project();
    break;
  case Command::Type::Run:
    Builder::build_project(true);
    break;
  case Command::Type::Clean:
    Builder::clean_project();
    break;
  }
}

void forge::Interface::print_help() {
  std::print("Forge V0.0.3 - Modern C++ Build System\n");
  std::print("Usage: forge <command> [options]\n\n");
  std::print("Commands:\n");
  std::print("  build   Compile the current project\n");
  std::print("  run     Build and execute the target\n");
  std::print("  clean   Remove build artifacts\n");
}
