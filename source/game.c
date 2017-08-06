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

enum {
    MAX_STATEFUL_VARS = 64,
    MAX_STATEFUL_VAR_NAME_LEN = 32
};

typedef struct {
    char name[MAX_STATEFUL_VAR_NAME_LEN];
    void *ptr;
} stateful_var;

typedef struct {

    stateful_var vars[MAX_STATEFUL_VARS];
    u32 vars_count;
    mem_pool pool;
} stateful_vars_table;

void *add_stateful_var(stateful_vars_table *table, u32 size, char *name) {
    cn_assert(table && size && name);
    cn_assert(table->vars_count < MAX_STATEFUL_VARS);
    cn_assert(strlen(name) < MAX_STATEFUL_VAR_NAME_LEN);

    void *var_ptr = mem_push(&table->pool, size);
    table->vars[table->vars_count].ptr = var_ptr;
    stb_strncpy(table->vars[table->vars_count].name, name, MAX_STATEFUL_VAR_NAME_LEN);
    ++table->vars_count;

    return var_ptr;
}

void *get_stateful_var(stateful_vars_table *table, char *name) {
    cn_assert(table && name);
    for (u32 i = 0; i < table->vars_count; ++i) {
        if (strcmp(name, table->vars[i].name) == 0) {
            return table->vars[i].ptr;
        }
    }

    return 0;
}

#define stateful(type, variable, initial_value) { \
        void *ptr = get_stateful_var(&g_state->stateful_vars, #variable); \
        if (!ptr) { \
            ptr = add_stateful_var(&g_state->stateful_vars, sizeof(type), #variable); \
            *((type*)ptr) = (initial_value); \
        } \
        variable = (type*)ptr; \
    }

typedef struct {
    //****************BASICS****************//
    b32 initialized;
    mem_pool pool;
    stateful_vars_table stateful_vars;

    //****************RENDER****************//
    bgfx_program_handle_t test_program;
    bgfx_vertex_decl_t test_vertex_decl;

    //****************GAME****************//
    character protagonist;
    tile game_map[16 * 16];
} game_state;

GLOBAL game_state *g_state;
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
    g_state = state;
    g_scratch_pool = &state->pool;
    g_bgfx = data->bgfx;
}

void game_initialize(game_data *data, u32 reserved_memory_size) {
    game_state *state = get_game_state(data);
    state->pool = new_mem_pool(
        (u8*)data->memory + reserved_memory_size,
        data->memory_size - reserved_memory_size);
    u64 stateful_vars_pool_size = 1 * Mb;
    state->stateful_vars.pool = new_mem_pool(mem_push(&state->pool, stateful_vars_pool_size), stateful_vars_pool_size);
    game_reset_globals(data);

    g_bgfx->vertex_decl_begin(&state->test_vertex_decl, BGFX_RENDERER_TYPE_NOOP);
    g_bgfx->vertex_decl_add(&state->test_vertex_decl, BGFX_ATTRIB_POSITION, 3, BGFX_ATTRIB_TYPE_FLOAT, false, false);
    g_bgfx->vertex_decl_add(&state->test_vertex_decl, BGFX_ATTRIB_COLOR0, 4, BGFX_ATTRIB_TYPE_UINT8, true, false);
    g_bgfx->vertex_decl_end(&state->test_vertex_decl);

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

typedef struct {
    float position[3];
    u32 abgr;
} test_vertex;

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

    kmVec3 at ={ 0, 0, 0 };
    kmVec3 eye ={ 0, 0, 1 };
    kmVec3 up ={ 0, 1, 0 };
    kmMat4 view = {0};
    kmMat4LookAt(&view, &eye, &at, &up);
    kmMat4 proj = {0};
    // TODO: find out why is everything stretched in Y axis and remove magic numbers from projection calculation
    kmMat4OrthographicProjection(&proj, data->window_w * -0.5f, data->window_w * 0.5f, data->window_h * -0.602f, data->window_h * 0.602f, -1.0f, 1.0f);
    g_bgfx->set_view_transform(0, view.mat, proj.mat);

    g_bgfx->set_state(BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_BLEND_ALPHA, 0);

    if (false) {
        kmMat4 scale = {0};
        kmMat4Scaling(&scale, 100, 100, 100);
        float *rotation_x, *rotation_y, *rotation_z;
        stateful(float, rotation_x, 0);
        stateful(float, rotation_y, 0);
        stateful(float, rotation_z, 0);
        *rotation_x += 1.0f * delta_time;
        *rotation_y += 1.5f * delta_time;
        *rotation_z += 2.0f * delta_time;
        kmMat4 rotate = {0};
        kmMat4RotationYawPitchRoll(&rotate, *rotation_x, *rotation_y, *rotation_z);
        kmMat4 transform = {0};
        kmMat4Multiply(&transform, &scale, &rotate);
        g_bgfx->set_transform(transform.mat, 1);
    } else {
        kmMat4 scale ={ 0 };
        kmMat4Scaling(&scale, 100, 100, 1);
        kmMat4 rotate ={ 0 };
        kmMat4RotationYawPitchRoll(&rotate, 0, 0, 0);
        kmMat4 translate ={ 0 };
        kmMat4Translation(&translate, 0, 0, 0);
        kmMat4 temp ={ 0 };
        kmMat4Multiply(&temp, &rotate, &scale);
        kmMat4 transform ={ 0 };
        kmMat4Multiply(&transform, &translate, &temp);
        g_bgfx->set_transform(transform.mat, 1);
    }

    bgfx_transient_vertex_buffer_t vertex_buff = {0};
    g_bgfx->alloc_transient_vertex_buffer(&vertex_buff, 256, &state->test_vertex_decl);
    bgfx_transient_index_buffer_t index_buff = {0};
    g_bgfx->alloc_transient_index_buffer(&index_buff, 256);

    test_vertex quad_vertices[] =
    {
        { -1, -1, 0, 0xff0000ff },
        { 1, -1, 0, 0xff00ff00 },
        { 1, 1, 0, 0xffff0000 },
        { -1, 1, 0, 0xffffffff }
    };
    test_vertex *vertex = (test_vertex*)vertex_buff.data;
    memcpy(vertex, quad_vertices, sizeof(quad_vertices));

    uint16_t quad_indices[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    u32 *index = (u32*)index_buff.data;
    memcpy(index, quad_indices, sizeof(quad_indices));

    g_bgfx->set_transient_vertex_buffer(0, &vertex_buff, 0, arr_len(quad_vertices));
    g_bgfx->set_transient_index_buffer(&index_buff, 0, arr_len(quad_indices));
    g_bgfx->submit(0, state->test_program, 0, false);

    u32 *stateful_test;
    stateful(u32, stateful_test, 0);
    ++(*stateful_test);
    g_bgfx->dbg_text_printf(0, 0, 0x4f, "stateful_test = %d", *stateful_test);

    g_bgfx->frame(false);
    return true;
}

void game_hotload(game_data *data) {
    game_reset_globals(data);
}

void game_hotunload(game_data *data) {
}
