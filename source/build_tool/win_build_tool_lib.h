typedef struct {
    char trigger_key;
    const char *description;
    void (*command)(b32* should_reload_build_lib);
} build_command;

#define MAX_BUILD_COMMANDS 32

typedef struct {
    build_command commands[MAX_BUILD_COMMANDS];
    u32 commands_count;
} build_commands;

build_commands get_build_commands();
