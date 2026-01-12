#pragma once

#include <string>
#include <vector>

namespace forge {

enum class Compiler { Auto = 0, Clang = 1, GCC = 2, CL = 3 };

struct Target {
  std::string name;
  std::string output_type;

  std::vector<std::string> sources;
  std::vector<std::string> flags;

  [[nodiscard]] std::string flags_str() const {
    std::string str;
    for (const auto &flag : flags) {
      str += flag;
      str += " ";
    }

    return str;
  }
};

class Project {
public:
  void add_executable(const std::string &name, const std::vector<std::string> &sources) {
    targets.push_back(Target{name, "exe", sources, {"-std=c++23", "-O2"}});
  }

  template <typename... Args>
  void add_executable(const std::string &name, Args... sources) {
    add_executable(name, std::vector<std::string>{std::string(sources)...});
  }

  void set_compiler(const Compiler new_compiler) { this->compiler = new_compiler; }

  [[nodiscard]] const std::vector<Target> &get_targets() const { return targets; }
  [[nodiscard]] const Compiler &get_compiler() const { return compiler; }

private:
  std::vector<Target> targets;
  Compiler compiler = Compiler::Auto;
};

extern "C" {
void build(Project *pkg);
}

#define FORGE_MAIN()                                                           \
  void forge_build(forge::Project &pkg);                                       \
  namespace forge {                                                            \
  extern "C" void build(forge::Project *pkg) { ::forge_build(*pkg); }          \
  }                                                                            \
  int main() { return 0; }                                                     \
  void forge_build(forge::Project &pkg)
} // namespace forge