typedef struct {
    void *memory;
    u64 memory_size;

    const u8 *kb;
    mouse_info mouse;

    bgfx_api *bgfx;

    SDL_Window *window;
    s32 window_w;
    s32 window_h;
} game_data;