#include "../platform.h"
#include "win_build_tool_lib.h"

#include <assert.h>

#include "../stb_wrapper.h"

#pragma warning(push, 0)
#include <windows.h>
#pragma warning (pop)

LOCAL void build_game();
LOCAL void configure_msvc();
LOCAL void copy_directory(char* source, char* destination);

void first_initialization() {
    configure_msvc();
}

LOCAL void add_build_command(build_commands* cmds, char key, char* desc, void (*command_fn)()) {
    assert(cmds && cmds->commands_count < MAX_BUILD_COMMANDS);
    build_command command = {0};
    command.trigger_key = key;
    command.description = desc;
    command.command = command_fn;
    cmds->commands[cmds->commands_count] = command;
    ++cmds->commands_count;
}

build_commands get_build_commands() {
    build_commands commands = {0};
    add_build_command(&commands, 'b', "build game", build_game);

    return commands;
}

void build_game() {
    configure_msvc();

    copy_directory("./source/3rdparty/bgfx", "./build/bgfx");
}

void configure_msvc() {
    char* msvc_configured = getenv("MSVC_CONFIGURED");
    if (msvc_configured)
        return;

    system("../source/build_tool/configure_msvc.bat");
    FILE *env_vars = fopen("env.txt", "r");
    assert(env_vars);
    {
        char buffer[1024 * 10]; // because PATH can be fucking huge
        while (fgets(buffer, sizeof(buffer), env_vars)) {
            char *var_value = buffer;
            do { ++var_value; } while (*var_value != '=');
            *var_value = 0;
            ++var_value;

            size_t n = strlen(var_value);
            assert(var_value[n - 1] == '\n');
            var_value[n - 1] = 0;

            SetEnvironmentVariable(buffer, var_value);
        }
    }
}

void copy_directory(char* source, char* destination)
{
    if (stb_fexists(destination)) {
        stb_delete_directory_recursive(destination);
    }
}
