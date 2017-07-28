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
    struct nk_context ui_context;

    //****************GAME****************//
    character protagonist;
    tile game_map[16 * 16];
} game_state;

game_state* get_game_state(engine_data *data)
{
    return (game_state*)data->memory;
}

void game_initialize(engine_data *data, gl_functions gl, struct nk_context *ui_context, u32 reserved_memory_size) {
    game_state *state = get_game_state(data);
    state->pool = new_mem_pool(
        (u8*)data->memory + reserved_memory_size,
        data->memory_size - reserved_memory_size);

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

b32 game_update(engine_data *data, gl_functions gl, struct nk_context *ui_context, float delta_time) {
    if (data->kb[SDL_SCANCODE_ESCAPE])
        return false;

    game_state *state = get_game_state(data);
    const u32 reserved_memory_size = 8 * Mb;
    cn_assert(sizeof(*state) < reserved_memory_size);
    if (!state->initialized) {
        game_initialize(data, gl, ui_context, reserved_memory_size);
        state->initialized = true;
    }

    for (u32 y = 0; y < 16; y++) {
        for (u32 x = 0; x < 16; x++) {
        }
    }

    return true;
}

void game_hotload(engine_data *data) {
}

void game_hotunload(engine_data *data) {
}
