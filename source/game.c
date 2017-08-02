typedef enum {
    TT_TERRAIN
} tile_type;

typedef struct {
    tile_type type;
    b32 traversable;
} tile;

typedef struct {
    u32 x;
    u32 y;
} position;

typedef struct {
    position pos;
} character;

typedef struct {
    //****************BASICS****************//
    b32 initialized;
    mem_pool pool;

    //****************RENDER****************//
    bgfx_program_handle_t test_program;

    //****************GAME****************//
    character protagonist;
    tile game_map[16 * 16];
} game_state;

GLOBAL mem_pool *g_scratch_pool;
GLOBAL bgfx_api *g_bgfx;

LOCAL game_state* get_game_state(game_data *data)
{
    return (game_state*)data->memory;
}

LOCAL bgfx_shader_handle_t load_shader(const char* path)
{
    u64 file_size = get_file_size(path);
    const bgfx_memory_t* mem = g_bgfx->alloc((u32)file_size + 1);
    load_file(path, mem->data, file_size);
    ((u8*)mem->data)[file_size] = 0;

    return g_bgfx->create_shader(mem);
}

LOCAL bgfx_program_handle_t load_program(const char* vs_path, const char* fs_path)
{
    bgfx_shader_handle_t vsh = load_shader(vs_path);
    bgfx_shader_handle_t fsh = load_shader(fs_path);

    return g_bgfx->create_program(vsh, fsh, true);
}

void game_reset_globals(game_data *data) {
    game_state *state = get_game_state(data);
    g_scratch_pool = &state->pool;
    g_bgfx = data->bgfx;
}

void game_initialize(game_data *data, u32 reserved_memory_size) {
    game_state *state = get_game_state(data);
    state->pool = new_mem_pool(
        (u8*)data->memory + reserved_memory_size,
        data->memory_size - reserved_memory_size);
    game_reset_globals(data);

    state->test_program = load_program("./assets/shaders/vs_test.bin", "./assets/shaders/fs_test.bin");
    cn_assert(state->test_program.idx != UINT16_MAX);

    state->protagonist.pos = (position){ 8, 8 };
    char *map =
        "1111111111111111"
        "1001001000000001"
        "1001001000000001"
        "1001001000000001"
        "1000001000000001"
        "1110111000000001"
        "1000000000000001"
        "1000000000000001"
        "1000000000000001"
        "1000000000000001"
        "1000000000000001"
        "1111111111111101"
        "1000000000000001"
        "1011111111111111"
        "1000000000000001"
        "1111111111111111";
    for (u32 i = 0; i < arr_len(map); ++i) {
        state->game_map[i].traversable = !map[i];
    }
}

b32 game_update(game_data *data, float delta_time) {
    if (data->kb[SDL_SCANCODE_ESCAPE])
        return false;

    game_state *state = get_game_state(data);
    const u32 reserved_memory_size = 8 * Mb;
    cn_assert(sizeof(*state) < reserved_memory_size);
    if (!state->initialized) {
        game_initialize(data, reserved_memory_size);
        state->initialized = true;
    }

    mat4 cam = lookat_cam((vec3) { 0, 0, -1.0f }, (vec3) { 0, 0, 0.0f }, (vec3) { 0, 1.0f, 0 });
    mat4 ortho = orthogonal(0.5f, data->window_w  + 0.5f, data->window_h + 0.5f, 0.5f, 0.0f, 100.0f, 0.0f, false);
    g_bgfx->set_view_transform(0, &cam, &ortho);

    for (u32 y = 0; y < 16; y++) {
        for (u32 x = 0; x < 16; x++) {
        }
    }

    return true;
}

void game_hotload(game_data *data) {
    game_reset_globals(data);
}

void game_hotunload(game_data *data) {
}
