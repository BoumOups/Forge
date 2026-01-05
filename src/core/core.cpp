#include "core.h++"

#include <filesystem>
#include <print>
#include <set>
#include <string>

#include "cache/hasher.hpp"
#include "cache/manifest.hpp"
#include "forge.hpp"
#include "sandbox/loader.h++"
#include "scheduler/executor.hpp"

bool forge::Builder::compile_build_script() {
#if defined(_WIN32) || defined(_WIN64)
  std::filesystem::path clang_path =
      std::filesystem::current_path() /
      "../vendor/wasi-sdk/wasi-sdk-29.0-x86_64-windows/bin/clang++";
#endif
#if defined(__linux__)
  std::filesystem::path clang_path =
      std::filesystem::current_path() /
      "../vendor/wasi-sdk/wasi-sdk-29.0-x86_64-linux/bin/clang++";
#endif
#if defined(__APPLE__)
  std::filesystem::path clang_path =
      std::filesystem::current_path() /
      "../vendor/wasi-sdk/wasi-sdk-29.0-ARM64-apple/bin/clang++";
#endif

  std::filesystem::path script_path =
      std::filesystem::current_path() / "build.cpp";
  std::filesystem::path output_path =
      std::filesystem::current_path() / OUTPUT_DIR / "build.wasm";

  if (!std::filesystem::exists(script_path)) {
    std::print("❌ Error: No 'build.cpp' found in {}\n",
               std::filesystem::current_path().string());
    return false;
  }

  std::string cmd = std::format("{} --target=wasm32-wasi "
                                "-O3 -nostdlib "
                                "-I./include "
                                "-Wl,--export-all -Wl,--allow-undefined "
                                "{} -o {}",
                                clang_path.string(), script_path.string(),
                                output_path.string());
  std::print("[CMD] {}\n", cmd);

  int result = std::system(cmd.c_str());
  return result == 0;
}

bool forge::Builder::graph_validation(const Project &project) {
  const auto &targets = project.get_targets();
  if (targets.empty()) {
    std::print("❌ No targets found.\n");
    return false;
  }

  std::set<std::string> target_names;
  for (const auto &target : targets) {
    if (target_names.contains(target.name)) {
      std::print("❌ Duplicate target name: {}\n", target.name);
      return false;
    }
    target_names.insert(target.name);
  }
  return true;
}

bool forge::Builder::compile_project(Project &project) {
  std::vector<std::string> compile_commands;
  std::vector<std::string> object_file;

  const std::string compiler = forge::utils::get_compiler(project);
  if (compiler.empty()) {
    std::print("❌ No suitable compiler found.\n");
    return false;
  }

  for (auto &target : project.get_targets()) {
    auto manifest = forge::Manifest::load();
    bool need_linking = false;

    for (auto &src : target.sources) {
      std::filesystem::path src_path(src);
      std::filesystem::path object_path(
          std::filesystem::path(OUTPUT_DIR) /
          src_path.filename().replace_extension(".o"));

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
        std::print("  ⏭️  Skipping {} (No changes)\n", src);
      }
      object_file.push_back(object_path.string());

      if (!forge::Executor::execute_parallel(compile_commands)) {
        std::print("❌ Compilation failed.\n");
        return false;
      }

      if (!object_file.empty() && need_linking) {
        forge::Manifest::save(manifest);

        std::string objects_str = forge::utils::join_objects(object_file);
        std::string bin_path =
            (std::filesystem::path(OUTPUT_DIR) / target.name).string();

        std::string linker_command =
            std::format("{} {} -o {}", compiler, objects_str, bin_path);
        std::print(" 🔗[LINK] {}\n", target.name);

        int result = std::system(linker_command.c_str());
        if (result != 0) {
          std::print("❌ Linking failed for {}\n", target.name);
          return false;
        }
      } else {
        std::print("✨ Project is up to date.\n");
      }
    }
  }
  return true;
}

bool forge::Builder::build_project() {
  forge::utils::ensure_directories();

  forge::Project project;

  if (!compile_build_script()) {
    std::print("❌ Failed at compiling build script !\n");
    return false;
  }

  if (!forge::loader::load_and_run_wasm_script(project)) {
    std::print("❌ Failed at loading and running wasm script !\n");
    return false;
  }

  if (!graph_validation(project)) {
    std::print("❌ Graph validation failed !\n");
    return false;
  }

  if (!compile_project(project)) {
    std::println("Failed at compiling project !");
    return false;
  }

  return true;
}