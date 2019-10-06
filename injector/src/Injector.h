#ifndef INJECTOR_INJECTOR_H
#define INJECTOR_INJECTOR_H


#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <unordered_map>

class Injector {
    using ProcInfo   = std::unordered_map<std::string, PROCESSENTRY32>;
    using ModuleInfo = std::unordered_map<std::string, MODULEENTRY32>;

public:
    Injector() = default;

    bool inject(const std::string &filename);
    bool inject(const uint32_t pid, const std::string &filename);

    bool parse_process_modules();
    bool find_target_process(const std::string &process_name);

    inline const ModuleInfo &get_loaded_modules() const;
    ModuleInfo parse_loaded_modules() const;
    void eject(const std::string &module_name);
    static ProcInfo parse_running_proccesses();
    static ModuleInfo parse_loaded_modules(const uint32_t pid);

    static std::string get_directory_file_path(const std::string &file);

private:
    uint32_t m_processId = 0;
    uintptr_t m_loadLibrary = 0;
    ModuleInfo m_loadedModules;
};

inline const Injector::ModuleInfo &Injector::get_loaded_modules() const {
    return m_loadedModules;
}

#endif //INJECTOR_INJECTOR_H
