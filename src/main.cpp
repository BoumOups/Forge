#include "cache/hasher.hpp"
#include "forge.hpp"
#include "manifest.hpp"
#include "scheduler/executor.hpp"

#include <cstdlib>
#include <format>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <set>

bool load_and_run_script(forge::Project& project);
bool load_and_run_wasm_script(forge::Project& project);

bool graph_validation(const forge::Project& project) {
    const auto& targets = project.get_targets();
    if (targets.empty()) {
        std::print("❌ No targets found.\n");
        return false;
    }

    std::set<std::string> target_names;
    for (const auto& target : targets) {
        if (target_names.contains(target.name)) {
            std::print("❌ Duplicate target name: {}\n", target.name);
            return false;
        }
        target_names.insert(target.name);

        for (const auto& src : target.sources) {
            if (!std::filesystem::exists(src)) {
                std::print("❌ Source file not found: {}\n", src);
                return false;
            }
        }

        if (target.sources.empty()) {
            std::print("❌ Target has no sources: {}\n", target.name);
            return false;
        }
    }

    return true;
}

bool compile_build_script() {
    std::string clang_path = "/opt/homebrew/Cellar/llvm@20/20.1.8/bin/clang++";

    std::filesystem::path script_path = std::filesystem::current_path() / "build.cpp";
    std::filesystem::path output_path = std::filesystem::current_path() / "build.wasm";

    if (!std::filesystem::exists(script_path)) {
        std::print("❌ Error: No 'build.cpp' found in {}\n", std::filesystem::current_path().string());
        return false;
    }

    std::string cmd = std::format(
        "{} --target=wasm32-wasi "
        "-O3 -nostdlib "
        "-I./include "
        "-Wl,--no-entry -Wl,--export-all -Wl,--allow-undefined "
        "{} -o {}",
        clang_path,
        script_path.string(),
        output_path.string()
    );
    std::print("   [CMD] {}\n", cmd);

    int result = std::system(cmd.c_str());
    return result == 0;
}

void ensure_directories() {
    if (!std::filesystem::exists("bin")) {
        std::filesystem::create_directory("bin");
    }
}

std::string join_objects(const std::vector<std::string>& objects) {
    std::ostringstream oss;
    for (size_t i = 0; i < objects.size(); i++) {
        oss << objects[i] << (i == objects.size() - 1 ? "" : " ");
    }
    return oss.str();
}

void print_help() {
    std::print("Forge V0.0.1 - Modern C++ Build System\n");
    std::print("Usage: forge <command> [options]\n\n");
    std::print("Commands:\n");
    std::print("  build   Compile the current project\n");
    std::print("  run     Build and execute the target\n");
    std::print("  clean   Remove build artifacts\n");
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_help();
        return EXIT_FAILURE;
    }

    std::string_view command = argv[1];

    std::cout << "⚒️  Forge: Planning build...\n";

    if (command == "build" || command == "run") {
        forge::Project project;
        std::print("🛠️  Starting build...\n");
        ensure_directories();

        if (!compile_build_script()) {
            std::print("❌ Failed to compile build.cpp\n");
            return EXIT_FAILURE;
        }

        if (!load_and_run_wasm_script(project)) {
            std::print("❌ Failed to load build script.\n");
            return EXIT_FAILURE;
        }

        if (!graph_validation(project)) {
            std::print("❌ Graph validation failed.\n");
            return EXIT_FAILURE;
        }

        std::vector<std::string> compile_commands;
        std::vector<std::string> objects;

        for (const auto& target : project.get_targets()) {
            auto manifest = forge::Manifest::load();
            bool need_linking = false;

            for (const auto& src : target.sources) {
                std::filesystem::path src_path(src);
                std::filesystem::path object_path = std::filesystem::path("bin") / src_path.filename().replace_extension(".o");

                std::string current_hash = forge::Hasher::hash_file(src, target.flags[0]);
                if (manifest[src] != current_hash) {
                    std::string compile_cmd = std::format("clang++ {} -c {} -o {}", target.flags_str(), src, object_path.string());
                    compile_commands.push_back(compile_cmd);

                    manifest[src] = current_hash;
                    need_linking = true;
                } else {
                    std::print("  ⏭️  Skipping {} (No changes)\n", src);
                }

                objects.push_back(object_path.string());
            }

            if (!forge::Executor::execute_parallel(compile_commands)) {
                std::print("❌ Compilation failed.\n");
                return EXIT_FAILURE;
            }

            if (!objects.empty() && need_linking) {
                forge::Manifest::save(manifest);

                std::string objects_str = join_objects(objects);
                std::string bin_path = (std::filesystem::path("bin") / target.name).string();

                std::string linker_command = std::format("clang++ {} -o {}", objects_str, bin_path);
                std::print(" 🔗[LINK] {}\n", target.name);

                int result = std::system(linker_command.c_str());
                if (result != 0) {
                    std::print("❌ Linking failed for {}\n", target.name);
                    return EXIT_FAILURE;
                }
            } else {
                std::print("✨ Project is up to date.\n");
            }

            std::print("\n✨ Build Successful\n");
            std::print("📦 Produced: {}\n", target.name);
        }
    }
    else if (command == "clean") {
        std::print("🧹 Removing build directory...\n");

        std::error_code ec;
        std::filesystem::remove_all("build", ec);
        if (ec) std::print("⚠️ Note: Could not delete build folder: {}\n", ec.message());
    }
    else {
        std::print("❌ Unknown command: {}\n", command);
        print_help();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
