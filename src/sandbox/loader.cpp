#include "forge.hpp"

#include <wasmtime.h>
#include <wasmtime.hh>

#include <fstream>
#include <ios>
#include <print>
#include <string>
#include <vector>

#include "cli/log.h++"
#include "sandbox/loader.h++"

bool forge::loader::load_and_run_wasm_script(Project &project,
                                             const std::string &wasm_path) {
  wasmtime::Engine engine;
  wasmtime::Store store(engine);
  wasmtime::Linker linker(engine);

  std::ifstream file(wasm_path, std::ios::binary);
  if (!file.is_open()) {
    message::log(message::Level::Error,
                        std::format("Could not open {}", wasm_path));
    return false;
  }

  std::vector<uint8_t> wasm_bytes((std::istreambuf_iterator(file)),
                                  std::istreambuf_iterator<char>());
  auto module_result = wasmtime::Module::compile(engine, wasm_bytes);

  if (!module_result) {
    message::log(message::Level::Error,
                        std::format("Failed to compile Wasm: {}",
                                    module_result.err().message()));
    return false;
  }
  auto module = module_result.ok();

  linker.define_wasi().unwrap();
  linker
      .func_wrap("env", "forge_host_add_executable",
                 [&project, &store](wasmtime::Caller caller, const int32_t name_ptr,
                                    const int32_t name_len, const int32_t src_ptr,
                                    const int32_t src_count) {
                   const auto export_item = caller.get_export("memory");
                   if (!export_item ||
                       !std::holds_alternative<wasmtime::Memory>(*export_item))
                     return;

                   const auto mem = std::get_if<wasmtime::Memory>(&*export_item);
                   if (!mem)
                     return;
                   uint8_t *base = mem->data(store).data();

                   std::string exe_name(
                       reinterpret_cast<char *>(base + name_ptr), name_len);

                   std::vector<std::string> sources;
                   struct RawWasmString {
                     uint32_t p;
                     uint32_t l;
                   };
                   auto *descriptors =
                       reinterpret_cast<RawWasmString *>(base + src_ptr);

                   for (size_t i = 0; i < static_cast<size_t>(src_count); ++i) {
                     const char *s_ptr =
                         reinterpret_cast<char *>(base + descriptors[i].p);
                     sources.emplace_back(s_ptr, descriptors[i].l);
                   }

                   project.add_executable(exe_name, sources);
                   message::log(
                       message::Level::Info,
                       std::format("Registered executable: {}", exe_name));
                 })
      .unwrap();

  linker
      .func_wrap("env", "forge_host_set_compiler",
                 [&project](wasmtime::Caller, int32_t compiler) {
                   project.set_compiler(static_cast<Compiler>(compiler));
                 })
      .unwrap();

  linker
      .func_wrap("env", "forge_host_get_os",
                 [] {
#if defined(__APPLE__)
                   return 1;
#elif defined(__linux__)
            return 2;
#elif defined(_WIN32)
            return 3;
#else
            return 0;
#endif
                 })
      .unwrap();

  auto instance_result = linker.instantiate(store, module);
  if (!instance_result) {
    message::log(message::Level::Error,
                        std::format("Failed to instantiate Wasm: {}",
                                    instance_result.err().message()));
    return false;
  }
  auto instance = instance_result.ok();

  if (const auto export_start = instance.get(store, "_start");
      export_start && std::holds_alternative<wasmtime::Func>(*export_start)) {
    auto &start_func = std::get<wasmtime::Func>(*export_start);

    if (auto start_result = start_func.call(store, {}); !start_result) {
      auto err = start_result.err();
      message::log(message::Level::Error,
                          std::format("Wasm Runtime: {}", err.message()));

      return false;
    }
  }

  return true;
}