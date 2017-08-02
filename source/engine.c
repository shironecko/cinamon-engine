#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <SDL_syswm.h>

/* ---STB lib--- */
#define STB_DEFINE
#define STB_NO_REGISTRY
#pragma warning(push, 1)
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4701)
#include <stb.h>
#pragma warning(pop)

#include <platform.h>
#include <engine.h>
#include <math3d.c>
#include <utility.c>

#include <stdlib.h>

/* ---STB rect pack--- */
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

/* ---STB truetype--- */
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

#define REF_W 1280
#define REF_H 720

#include "game.c"

typedef struct {
    //****************BASICS****************//
    b32 initialized;

    s32 window_w;
    s32 window_h;

    mem_pool pool;

    u8 *game_memory;
    u32 game_memory_size;

    float accumulated_dt;
    float average_dt;
    u32 frames_tracked;
} engine_state;

LOCAL engine_state* get_engine_state(engine_data* data) {
    return (engine_state *)data->memory;
}

LOCAL engine_data game_data_from_engine_data(engine_data* engine_d) {
    engine_data game_d = *engine_d;
    engine_state *state = get_engine_state(engine_d);
    game_d.memory = state->game_memory;
    game_d.memory_size = state->game_memory_size;
    return game_d;
}

LOCAL bool bgfx_sdl_set_window(bgfx_api *bgfx, SDL_Window* _window)
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(_window, &wmi))
    {
        return false;
    }

    // TODO: clean this up using SDL platform field instead of these defines
    bgfx_platform_data_t pd;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    pd.ndt          = wmi.info.x11.display;
    pd.nwh          = (void*)(uintptr_t)wmi.info.x11.window;
#	elif BX_PLATFORM_OSX
    pd.ndt          = NULL;
    pd.nwh          = wmi.info.cocoa.window;
#	elif BX_PLATFORM_WINDOWS
    pd.ndt          = NULL;
    pd.nwh          = wmi.info.win.window;
#	elif BX_PLATFORM_STEAMLINK
    pd.ndt          = wmi.info.vivante.display;
    pd.nwh          = wmi.info.vivante.window;
#	endif // BX_PLATFORM_
    pd.context      = NULL;
    pd.backBuffer   = NULL;
    pd.backBufferDS = NULL;
    bgfx->set_platform_data(&pd);

    return true;
}

GLOBAL engine_state* g_engine_state;

b32 engine_update(engine_data *data, float delta_time) {
    engine_state *state = get_engine_state(data);
    bgfx_api *bgfx = &data->bgfx;
    g_engine_state = state;
    if (!state->initialized) {
        SDL_Log("Initializing game state...");
        state->initialized = true;

        SDL_SetWindowSize(data->window, REF_W, REF_H);

        u32 engine_scratch_pool_size = 10 * Mb;
        state->pool =
            new_mem_pool((u8*)data->memory + sizeof(*state), engine_scratch_pool_size);
        state->game_memory = (u8*)data->memory  + sizeof(*state) + engine_scratch_pool_size;
        state->game_memory_size = data->memory_size - sizeof(*state) - engine_scratch_pool_size;

        bgfx_sdl_set_window(bgfx, data->window);
        b32 bgfx_init_result = bgfx->init(BGFX_RENDERER_TYPE_COUNT, BGFX_PCI_ID_NONE, 0, NULL, NULL);
        cn_assert(bgfx_init_result);
        bgfx->set_debug(BGFX_DEBUG_TEXT);

        engine_hotload(data);
    }

    // WINDOW RESIZE HANDLING
    if (state->window_w != data->window_w || state->window_h != data->window_h) {
        char buff[1024];
        stb_snprintf(buff, arr_len(buff), "res change from(%d, %d), to(%d, %d)", state->window_w,
            state->window_h, data->window_w, data->window_h);
        SDL_Log("%s", buff);

        bgfx->reset(data->window_w, state->window_h, BGFX_RESET_VSYNC);

        float ref_aspect = (float)REF_H / (float)REF_W;
        float actual_aspect = (float)data->window_h / (float)data->window_w;
        float scale;
        if (ref_aspect > actual_aspect)
            scale = (float)data->window_h / (float)REF_H;
        else
            scale = (float)data->window_w / (float)REF_W;

        float hw = data->window_w * 0.5f;
        float hh = data->window_h * 0.5f;

        float scale_x = 1.0f / hw * scale;
        float scale_y = -1.0f / hh * scale;
        float shift_x = (data->window_w - REF_W * scale) / data->window_w - 1.0f;
        float shift_y = 1.0f - (data->window_h - REF_H * scale) / data->window_h;

        const float mvp[] ={ scale_x, 0, 0, 0, 0,       scale_y, 0, 0,
            0,       0, 1, 0, shift_x, shift_y, 0, 1 };

        state->window_w = data->window_w;
        state->window_h = data->window_h;
    }

    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // HACK: kinda hacky...
            if (event.type == SDL_QUIT) return false;

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                SDL_KeyboardEvent kb_event = event.key;
                SDL_Keysym keysym = kb_event.keysym;
                b32 is_keydown = event.type == SDL_KEYDOWN;
            }
        }
    }

    {
        bgfx->set_view_clear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
        bgfx->set_view_rect(0, 0, 0, (uint16_t)state->window_w, (uint16_t)state->window_h);
        bgfx->touch(0);

        // perf stats
        state->accumulated_dt += delta_time;
        ++state->frames_tracked;
        const float dt_update_period = 0.5;
        if (state->accumulated_dt >= dt_update_period) {
            state->average_dt = state->accumulated_dt / (float)state->frames_tracked;
            state->accumulated_dt = 0;
            state->frames_tracked = 0;
        }

        bgfx->dbg_text_clear(0, false);
        bgfx->dbg_text_printf(0, 1, 0x4f, "%dx%d", state->window_w, state->window_h);
        bgfx->dbg_text_printf(0, 2, 0x6f, "%0.2fms", state->average_dt * 1000);

        engine_data game_d = game_data_from_engine_data(data);
        if (!game_update(&game_d, delta_time)) {
            return false;
        }

        bgfx->frame(false);
    }

    return true;
}

void engine_hotload(engine_data *data) {
    engine_data game_d = game_data_from_engine_data(data);
    game_hotload(&game_d);
}

void engine_hotunload(engine_data *data) {
    engine_data game_d = game_data_from_engine_data(data);
    game_hotunload(&game_d);
}