#include "core.h++"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <print>
#include <set>
#include <string>

#include "cache/hasher.hpp"
#include "cache/manifest.hpp"
#include "cli/log.h++"
#include "core/paths.h++"
#include "forge.hpp"
#include "sandbox/loader.h++"
#include "scheduler/executor.hpp"

bool forge::Builder::compile_build_script() {
  std::filesystem::path clang_path = forge::Path::get_wasm_compiler_path();
  std::filesystem::path forge_include_path =
      forge::Path::get_forge_include_directory_path();

  const std::string_view SCRIPT_NAME = "build.cpp";

  std::filesystem::path script_path =
      std::filesystem::current_path() / SCRIPT_NAME;
  std::filesystem::path output_path =
      forge::Path::get_output_directory_path() /
      std::filesystem::path(SCRIPT_NAME).replace_extension(".wasm");

  if (!std::filesystem::exists(script_path)) {
    forge::message::log(
        forge::message::Level::Error,
        std::format("No 'build.cpp' found in {}", script_path.string()));
    return false;
  }

  std::string cmd =
      std::format("{} --target=wasm32-wasi "
                  "-O3 -nostdlib "
                  "-I{} "
                  "-Wl,--export-all -Wl,--allow-undefined "
                  "{} -o {}",
                  clang_path.string(), forge_include_path.string(),
                  script_path.string(), output_path.string());
  forge::message::log(forge::message::Level::Info,
                      std::format("Compiling build script: {}", cmd));

  int result = std::system(cmd.c_str());
  return result == 0;
}

bool forge::Builder::graph_validation(const Project &project) {
  const auto &targets = project.get_targets();
  if (targets.empty()) {
    forge::message::log(forge::message::Level::Error,
                        "No targets defined in the project.");
    return false;
  }

  std::set<std::string> target_names;
  for (const auto &target : targets) {
    if (target_names.contains(target.name)) {
      forge::message::log(
          forge::message::Level::Error,
          std::format("Duplicate target name found: {}", target.name));
      return false;
    }
    target_names.insert(target.name);
  }
  return true;
}

bool forge::Builder::compile_project(Project &project) {
  std::vector<std::string> compile_commands;
  std::vector<std::string> object_file;

  const std::string OUTPUT_DIR =
      forge::Path::get_output_directory_path().string();

  const std::string compiler = forge::utils::get_compiler(project);
  if (compiler.empty()) {
    forge::message::log(forge::message::Level::Error,
                        "No suitable compiler found.");
    return false;
  }

  for (auto &target : project.get_targets()) {
    auto manifest = forge::Manifest::load();
    bool need_linking = false;

    for (auto &src : target.sources) {
      std::filesystem::path src_path(src);
      std::filesystem::path object_path(
          OUTPUT_DIR / src_path.filename().replace_extension(".o"));

      std::string current_hash =
          forge::Hasher::hash_file(src, target.flags_str());
      if (manifest[src] != current_hash) {
        std::string compile_cmd =
            std::format("{} {} -c {} -o {}", compiler, target.flags_str(), src,
                        object_path.string());
        compile_commands.push_back(compile_cmd);

        manifest[src] = current_hash;
        need_linking = true;
      } else {
        forge::message::log(forge::message::Level::Info,
                            std::format("Skipping {} (No changes)", src));
      }
      object_file.push_back(object_path.string());

      if (!forge::Executor::execute_parallel(compile_commands)) {
        forge::message::log(forge::message::Level::Error,
                            "Compilation process encountered errors.");
        return false;
      }

      if (!object_file.empty() && need_linking) {
        forge::Manifest::save(manifest);

        std::string objects_str = forge::utils::join_objects(object_file);
        std::string bin_path =
            (std::filesystem::path(OUTPUT_DIR) / target.name).string();

        std::string linker_command =
            std::format("{} {} -o {}", compiler, objects_str, bin_path);
        forge::message::log(forge::message::Level::Info,
                            std::format("Linking command: {}", linker_command));

        int result = std::system(linker_command.c_str());
        if (result != 0) {
          forge::message::log(
              forge::message::Level::Error,
              std::format("Linking failed for {}", target.name));
          return false;
        }
      } else {
        forge::message::log(forge::message::Level::Info,
                            "Project is up to date.");
      }
    }
  }
  return true;
}

bool forge::Builder::run_project(Project &project) {
  const std::string OUTPUT_DIR =
      forge::Path::get_output_directory_path().string();

  const std::string EXE_NAME = project.get_targets().front().name;

  std::string file_extension = "";
#if defined(_WIN32) || defined(WIN64_)
  file_extension = ".exe";
#endif

  const std::string run_command =
      std::format("{}/{}{}", OUTPUT_DIR, EXE_NAME, file_extension);

  return std::system(run_command.c_str()) == 0;
}

bool forge::Builder::clean_project() {
  const std::string OUTPUT_DIR =
      forge::Path::get_output_directory_path().string();
  std::string clean_command = std::format("rm -rf {}", OUTPUT_DIR);
#if defined(_WIN32) || defined(WIN64_)
  clean_command = std::format("rmdir s /q {}", OUTPUT_DIR);
#endif

  return std::system(clean_command.c_str()) == 0;
}

bool forge::Builder::build_project(bool auto_execute) {
  const std::filesystem::path OUTPUT_DIR =
      forge::Path::get_output_directory_path();

  forge::Project project;

  if (!compile_build_script()) {
    forge::message::log(forge::message::Level::Error,
                        "Failed at compiling build script !");
    return false;
  }

  std::filesystem::path wasm_path =
      std::filesystem::current_path() / OUTPUT_DIR / "build.wasm";
  if (!forge::loader::load_and_run_wasm_script(project, wasm_path.string())) {
    forge::message::log(forge::message::Level::Error,
                        "Failed at loading and running wasm script !");
    return false;
  }

  if (!graph_validation(project)) {
    forge::message::log(forge::message::Level::Error,
                        "Graph validation failed !");
    return false;
  }

  if (!compile_project(project)) {
    forge::message::log(forge::message::Level::Error,
                        "Failed at compiling project !");
    return false;
  }

  if (auto_execute == true) {
    if (!run_project(project)) {
      forge::message::log(forge::message::Level::Error,
                          "Failed at running project !");
    }
  }

  return true;
}