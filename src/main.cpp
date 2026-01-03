#include "forge.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <filesystem>

int main() {
    forge::Project project;
    std::cout << "⚒️  Forge: Planning build...\n";

    for (const auto& target : project.get_targets()) {
        std::cout << "📦 Target: " << target.name << "\n";

        std::string objects = "";
        if (std::filesystem::exists(target.name)) {
            std::cout << "  ✅ Up to date.\n";
            continue;
        }

        for (const auto& src : target.sources) {
            std::string cmd = "clang++ " + target.flags[0] + " -c " + src + " -o " + src + ".o";
            std::cout << "  🔍 " << cmd << "\n";
            std::system(cmd.c_str());
            objects += src + ".o ";
        }

        std::string link_cmd = "clang++ " + objects + "-o " + target.name;
        std::cout << "  🔗 Linking " << target.name << "...\n";
        std::system(link_cmd.c_str());
    }
    std::cout << "✨ Build Finished.\n";

    return EXIT_SUCCESS;
}
