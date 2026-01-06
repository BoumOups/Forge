# Contributing to Forge

First off, thank you for helping us build Forge! I are excited to create a C++ build system that feels as native and powerful as Zig's build system, but for the C++ ecosystem.

## 🍎 Current Platform Support

> **Note:** Currently Forge can be used on AppleARM64, Linuxx86_64 and Windowsx86_64, so the major platforms or supported and with futur version the Windows ARM version will be supported too.

## 📜 License

By contributing to Forge, you agree that your contributions will be licensed under the **MIT License**.

## 🚀 How Can I Help?

### 1. Bug Reports

Since I are in early development, bugs are expected. Please open an issue with:

- Your OS.
- Your compiler version (ex: `clang --version`).
- How to reproduce the bug.
- Plus provide any valuable informations

### 2. Feature Ideas

Want to add support for a specific package manager or compiler flag abstraction? Open an issue to discuss the design. I aim for a "C++-first" approach where the build logic is as readable as the source code.

### 3. Pull Requests

1. **Fork** the repo and create your branch from `main`.
2. **Coding Standards:**
   - Use Modern C++ (C++23).
   - Follow the project's existing indentation and naming style.
   - Run `clang-format` if a configuration file is provided.
3. **Keep it Lightweight:** Forge aims to be fast with minimal dependencies. Avoid adding external libraries.
4. **Test:** Ensure your changes build and run correctly on your OS and also the others supported platforms.

## 🛠️ Getting Started (Local Setup)

  Since Forge is a build system for C++, you will need a C++23 compiler installed and CMake to build the project.

  1. **Clone the repository:**

   ```bash
   git clone [https://github.com/BoumOups/Forge.git](https://github.com/BoumOups/Forge.git)
   cd Forge
   cmake --build build

   ```

  2. **Test the build:**

   ```bash
   cd example
   ../build/forge build

   ```

  3. **Adding features**
   First create your own branch name feature/<the feature you want to implement>

  ```bash
    git checkout -b feature/<the incredible feature>
  ```

  After you've changes create a commit with an complete descrition of what you have implemented

  ```bash
    git commit -m "Amazing description"
  ```

  Then push this beautifull feature on tour fork of the repo

  ```bash
    git push -u origine <branch_name>
  ```

  Finally you can make a pull request that explain all your certainly good changes, and voila !

## ❤️ Thank you for contributing to Forge !

I wanted to thanks you for contributing to this project because this tell that Forge is not just my project but also yours 😁 !
