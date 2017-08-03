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

#include "game.h"
#include "game.c"

typedef struct {
    //****************BASICS****************//
    b32 initialized;

    s32 window_w;
    s32 window_h;

    mem_pool global_pool;
    mem_pool scratch_pool;

    float accumulated_dt;
    float average_dt;
    u32 frames_tracked;

    bgfx_callback_interface_t bgfx_callback_i;
    bgfx_callback_vtbl_t bgfx_callback_vt;

    game_data game_data;
} engine_state;

LOCAL engine_state* get_engine_state(engine_data* data) {
    return (engine_state *)data->memory;
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

// BGFX callback API
void bgfx_callback_fatal(bgfx_callback_interface_t* _this, bgfx_fatal_t _code, const char* _str) {
    char buff[1024];
    stb_snprintf(buff, arr_len(buff), "[BGFX FATAL] %s", _str);
    SDL_Log(buff);
    cn_assert(!"BGFX FATAL");
}

void bgfx_callback_trace_vargs(bgfx_callback_interface_t* _this, const char* _filePath, uint16_t _line, const char* _format, va_list _argList) {
    char buff[1024];
    vsnprintf(buff, arr_len(buff), _format, _argList);
    char buff1[2048];
    stb_snprintf(buff1, arr_len(buff1), "[BGFX TRACE] %s (%d): %s", _filePath, _line, buff);
    SDL_Log(buff1);
}

uint32_t bgfx_callback_cache_read_size(bgfx_callback_interface_t* _this, uint64_t _id) {
    return 0;
}

bool bgfx_callback_cache_read(bgfx_callback_interface_t* _this, uint64_t _id, void* _data, uint32_t _size) {
    return false;
}

void bgfx_callback_cache_write(bgfx_callback_interface_t* _this, uint64_t _id, const void* _data, uint32_t _size) {
}

void bgfx_callback_screen_shot(bgfx_callback_interface_t* _this, const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) {
}

void bgfx_callback_capture_begin(bgfx_callback_interface_t* _this, uint32_t _width, uint32_t _height, uint32_t _pitch, bgfx_texture_format_t _format, bool _yflip) {
}

void bgfx_callback_capture_end(bgfx_callback_interface_t* _this) {
}

void bgfx_callback_capture_frame(bgfx_callback_interface_t* _this, const void* _data, uint32_t _size) {
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
        state->global_pool = new_mem_pool((u8*)data->memory + sizeof(*state), data->memory_size - sizeof(*state));
        state->scratch_pool = new_mem_pool(mem_push(&state->global_pool, engine_scratch_pool_size), engine_scratch_pool_size);

        game_data *gd = &state->game_data;
        gd->bgfx = bgfx;
        gd->window = data->window;
        gd->memory_size = state->global_pool.hi - state->global_pool.low;
        gd->memory = mem_push(&state->global_pool, gd->memory_size);

        bgfx_sdl_set_window(bgfx, data->window);

        state->bgfx_callback_vt.fatal = bgfx_callback_fatal;
        state->bgfx_callback_vt.trace_vargs = bgfx_callback_trace_vargs;
        state->bgfx_callback_vt.cache_read_size = bgfx_callback_cache_read_size;
        state->bgfx_callback_vt.cache_read = bgfx_callback_cache_read;
        state->bgfx_callback_vt.cache_write = bgfx_callback_cache_write;
        state->bgfx_callback_vt.screen_shot = bgfx_callback_screen_shot;
        state->bgfx_callback_vt.capture_begin = bgfx_callback_capture_begin;
        state->bgfx_callback_vt.capture_end = bgfx_callback_capture_end;
        state->bgfx_callback_vt.capture_frame = bgfx_callback_capture_frame;
        state->bgfx_callback_i.vtbl = &state->bgfx_callback_vt;
        b32 bgfx_init_result = bgfx->init(BGFX_RENDERER_TYPE_DIRECT3D9, BGFX_PCI_ID_NONE, 0, &state->bgfx_callback_i, NULL);
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

        state->window_w = data->window_w;
        state->window_h = data->window_h;
    }

    {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // HACK: kinda hacky...
            if (event.type == SDL_QUIT) return false;

            //if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            //    SDL_KeyboardEvent kb_event = event.key;
            //    SDL_Keysym keysym = kb_event.keysym;
            //    b32 is_keydown = event.type == SDL_KEYDOWN;
            //}
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

        game_data *gd = &state->game_data;
        gd->kb = data->kb;
        gd->mouse = data->mouse;
        gd->window_w = data->window_w;
        gd->window_h = data->window_h;
        if (!game_update(gd, delta_time)) {
            return false;
        }

        //bgfx->frame(false);
    }

    return true;
}

void engine_hotload(engine_data *data) {
    engine_state *state = get_engine_state(data);
    game_hotload(&state->game_data);
}

void engine_hotunload(engine_data *data) {
    engine_state *state = get_engine_state(data);
    game_hotunload(&state->game_data);
}