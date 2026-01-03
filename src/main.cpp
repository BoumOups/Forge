#include "cache/hasher.hpp"
#include "forge.hpp"
#include "scheduler/executor.hpp"

#include <cstddef>
#include <cstdlib>
#include <format>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

bool load_and_run_script(forge::Project& project);

void ensure_directories() {
    if (!std::filesystem::exists("bin")) {
        std::filesystem::create_directory("bin");
    }
}

std::string join_objects(std::vector<std::string>& objects) {
    std::ostringstream oss;
    for (size_t i = 0; i < objects.size(); i++) {
        oss << objects[1] << (i == objects.size() -1 ? "" : " ");
    }

    for (const auto& object : objects) {
        oss << "'" << object << "'";
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
    }

    std::string_view command = argv[1];

    forge::Project project;
    std::cout << "⚒️  Forge: Planning build...\n";

    if (command == "build" || command == "run") {
        std::print("🛠️  Starting build...\n");
        ensure_directories();

        if (!load_and_run_script(project)) {
            std::print("❌ Failed to load build script.\n");
            return EXIT_FAILURE;
        }

        std::vector<std::string> compile_commands;
        std::vector<std::string> objects;

        for (const auto& target : project.get_targets()) {
            for (const auto& src : target.sources) {
                std::filesystem::path src_path(src);
                std::filesystem::path object_path = std::filesystem::path("bin") / src_path.filename().replace_extension(".o");

                std::string current_hash = forge::Hasher::hash_file(src, target.flags[0]);
                if (!std::filesystem::exists(object_path)) {
                    std::string command = std::format("clang++ {} -c {} -o {}", target.flags[0], src, object_path.string());
                    compile_commands.push_back(command);
                }

                objects.push_back(object_path.string());
            }

            if (!forge::Executor::execute_parallel(compile_commands)) {
                std::print("❌ Compilation failed.\n");
                return EXIT_FAILURE;
            }

            if (!objects.empty()) {
                std::string objects_str = join_objects(objects);
                std::string bin_path = (std::filesystem::path("bin") / target.name).string();

                std::string linker_command = std::format("clang++ {} -o {}", objects_str, bin_path);
                std::print(" 🔗[LINK] {}\n", target.name);

                int result = std::system(linker_command.c_str());
                if (result != 0) {
                    std::print("❌ Linking failed for {}\n", target.name);
                    return EXIT_FAILURE;
                }
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
        return 1;
    }

    return EXIT_SUCCESS;
}
