#include "interface.h++"

#include <print>
#include <string_view>

#include "cli/log.h++"
#include "core/core.h++"

void forge::Interface::parse_command(const char *command) {
  std::string_view cmd(command);
  if (cmd == "build") {
    forge::Interface::run_command({Command::Type::Build});
  } else if (cmd == "run") {
    forge::Interface::run_command({Command::Type::Run});
  } else if (cmd == "clean") {
    forge::Interface::run_command({Command::Type::Clean});
  } else {
    forge::message::log(forge::message::Level::Error,
                        std::format("Unknown command '{}'", command));
    print_help();
  }
}

void forge::Interface::run_command(const Command &command) {
  switch (command.type) {
  case Command::Type::Build:
    forge::Builder::build_project();
    break;
  case Command::Type::Run:
    forge::Builder::build_project(true);
    break;
  case Command::Type::Clean:
    forge::message::log(forge::message::Level::Info,
                        "Clean command executed (placeholder).");
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
