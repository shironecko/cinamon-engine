#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>

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
#include <gl_render.c>

#include <stdlib.h>

/* ---STB rect pack--- */
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>

/* ---STB truetype--- */
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

/* ---Nuclear IMGUI--- */
#define NK_BYTE u8
#define NK_INT16 s16
#define NK_UINT16 u16
#define NK_INT32 s32
#define NK_UINT32 u32
#define NK_SIZE_TYPE usize
#define NK_POINTER_TYPE uptr

#define NK_IMPLEMENTATION
#define NK_PRIVATE
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_BUTTON_TRIGGER_ON_RELEASE
#define NK_ASSERT cn_assert
#pragma warning(push)
#pragma warning(disable : 4127)
#include "nuklear.h"
#pragma warning(pop)

#define REF_W 1280
#define REF_H 720

#include "game.c"

typedef struct {
    struct nk_user_font nk_font;
    float height;
    float ascent;
    float spacing;
    stbtt_packedchar packed_chars[256];
    GLuint texture;
    u32 tex_w, tex_h;
} ui_font;

typedef struct {
    float position[2];
    float uv[2];
    u8 col[4];
} ui_vertex;

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

    //****************UI****************//
    struct {
        GLuint id;
        GLint u_mvp;
        GLint a_pos;
        GLint a_col;
        GLuint v_array;
    } minimal_shader;

    struct {
        GLuint id;
        GLint u_tex;
        GLint u_mvp;
        GLint a_pos;
        GLint a_uv;
        GLint a_col;
        GLuint font_tex;
        GLuint v_array, i_array;
    } ui_shader;

    ui_font ui_font;

    struct nk_context ui_context;
    struct nk_buffer ui_cmd_buff;
    struct nk_draw_null_texture ui_draw_null_tex;
    struct nk_allocator ui_font_atlas_alloc;
    struct nk_panel ui_panel;

    // TODO: figure out some good numbers
    u8 ui_cmd_buff_mem[1 * Mb];
    u8 ui_pool_buff_mem[1 * Mb];

    u8 ui_vertex_buffer[512 * 1024];
    u8 ui_element_buffer[128 * 1024];
} engine_state;

float ui_text_width_fn(nk_handle userdata, float height, const char *str, int len) {
    ui_font *font = userdata.ptr;
    float xpos = 0, ypos = 0;
    for (s32 i = 0; i < len; ++i) {
        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(font->packed_chars, font->tex_w, font->tex_h, str[i], &xpos, &ypos,
            &quad, 0);
    }

    float scale = height / font->height;
    return xpos * scale;
}

void ui_query_font_glyph_fn(nk_handle userdata, float height, struct nk_user_font_glyph *glyph,
    nk_rune codepoint, nk_rune next_codepoint) {
    ui_font *font = userdata.ptr;
    float xpos = 0, ypos = 0;
    float scale = height / font->height;
    stbtt_aligned_quad quad;
    stbtt_GetPackedQuad(font->packed_chars, font->tex_w, font->tex_h, codepoint, &xpos, &ypos,
        &quad, 0);
    glyph->uv[0].x = quad.s0;
    glyph->uv[0].y = quad.t0;
    glyph->uv[1].x = quad.s1;
    glyph->uv[1].y = quad.t1;
    glyph->offset.x = quad.x0 * scale;
    glyph->offset.y = (quad.y0 + font->ascent) * scale;
    glyph->width = (quad.x1 - quad.x0) * scale;
    glyph->height = (quad.y1 - quad.y0) * scale;
    glyph->xadvance = xpos * scale;
}

enum nk_keys sdlkey_to_nkkey(u32 sdlkey) {
    switch (sdlkey) {
        case SDLK_RETURN:       return NK_KEY_ENTER;
        case SDLK_BACKSPACE:    return NK_KEY_BACKSPACE;
        case SDLK_TAB:          return NK_KEY_TAB;
        case SDLK_INSERT:       return NK_KEY_TEXT_INSERT_MODE;
        case SDLK_HOME:         return NK_KEY_TEXT_LINE_START;
        case SDLK_PAGEUP:       return NK_KEY_SCROLL_START;
        case SDLK_DELETE:       return NK_KEY_DEL;
        case SDLK_END:          return NK_KEY_TEXT_LINE_END;
        case SDLK_PAGEDOWN:     return NK_KEY_SCROLL_DOWN;
        case SDLK_RIGHT:        return NK_KEY_RIGHT;
        case SDLK_LEFT:         return NK_KEY_LEFT;
        case SDLK_DOWN:         return NK_KEY_DOWN;
        case SDLK_UP:           return NK_KEY_UP;
        case SDLK_KP_ENTER:     return NK_KEY_ENTER;
        case SDLK_AGAIN:        return NK_KEY_TEXT_REDO;
        case SDLK_UNDO:         return NK_KEY_TEXT_UNDO;
        case SDLK_CUT:          return NK_KEY_CUT;
        case SDLK_COPY:         return NK_KEY_COPY;
        case SDLK_PASTE:        return NK_KEY_PASTE;
        case SDLK_KP_TAB:       return NK_KEY_TAB;
        case SDLK_KP_BACKSPACE: return NK_KEY_BACKSPACE;
        case SDLK_LCTRL:        return NK_KEY_CTRL;
        case SDLK_LSHIFT:       return NK_KEY_SHIFT;
        case SDLK_RCTRL:        return NK_KEY_CTRL;
        case SDLK_RSHIFT:       return NK_KEY_SHIFT;
    }

    return NK_KEY_NONE;
}

const char* get_mode_str(s32 mode) {
    switch (mode) {
    case SDL_PIXELFORMAT_UNKNOWN:
        return "UNKNOWN";
    case SDL_PIXELFORMAT_INDEX1LSB:
        return "INDEX1LSB";
    case SDL_PIXELFORMAT_INDEX1MSB:
        return "INDEX1MSB";
    case SDL_PIXELFORMAT_INDEX4LSB:
        return "INDEX4LSB";
    case SDL_PIXELFORMAT_INDEX4MSB:
        return "INDEX4MSB";
    case SDL_PIXELFORMAT_INDEX8:
        return "INDEX8";
    case SDL_PIXELFORMAT_RGB332:
        return "RGB332";
    case SDL_PIXELFORMAT_RGB444:
        return "RGB444";
    case SDL_PIXELFORMAT_RGB555:
        return "RGB555";
    case SDL_PIXELFORMAT_BGR555:
        return "BGR555";
    case SDL_PIXELFORMAT_ARGB4444:
        return "ARGB4444";
    case SDL_PIXELFORMAT_RGBA4444:
        return "RGBA4444";
    case SDL_PIXELFORMAT_ABGR4444:
        return "ABGR4444";
    case SDL_PIXELFORMAT_BGRA4444:
        return "BGRA4444";
    case SDL_PIXELFORMAT_ARGB1555:
        return "ARGB1555";
    case SDL_PIXELFORMAT_RGBA5551:
        return "RGBA5551";
    case SDL_PIXELFORMAT_ABGR1555:
        return "ABGR1555";
    case SDL_PIXELFORMAT_BGRA5551:
        return "BGRA5551";
    case SDL_PIXELFORMAT_RGB565:
        return "RGB565";
    case SDL_PIXELFORMAT_BGR565:
        return "BGR565";
    case SDL_PIXELFORMAT_RGB24:
        return "RGB24";
    case SDL_PIXELFORMAT_BGR24:
        return "BGR24";
    case SDL_PIXELFORMAT_RGB888:
        return "RGB888";
    case SDL_PIXELFORMAT_RGBX8888:
        return "RGBX8888";
    case SDL_PIXELFORMAT_BGR888:
        return "BGR888";
    case SDL_PIXELFORMAT_BGRX8888:
        return "BGRX8888";
    case SDL_PIXELFORMAT_ARGB8888:
        return "ARGB8888";
    case SDL_PIXELFORMAT_RGBA8888:
        return "RGBA8888";
    case SDL_PIXELFORMAT_ABGR8888:
        return "ABGR8888";
    case SDL_PIXELFORMAT_BGRA8888:
        return "BGRA8888";
    case SDL_PIXELFORMAT_ARGB2101010:
        return "ARGB2101010";
    case SDL_PIXELFORMAT_YV12:
        return "YV12";
    case SDL_PIXELFORMAT_IYUV:
        return "IYUV";
    case SDL_PIXELFORMAT_YUY2:
        return "YUY2";
    case SDL_PIXELFORMAT_UYVY:
        return "UYVY";
    case SDL_PIXELFORMAT_YVYU:
        return "YVYU";
    case SDL_PIXELFORMAT_NV12:
        return "NV12";
    case SDL_PIXELFORMAT_NV21:
        return "NV21";
    }

    return "wtf?";
}

engine_state* get_engine_state(engine_data* data) {
    return (engine_state *)data->memory;
}

engine_data game_data_from_engine_data(engine_data* engine_d) {
    engine_data game_d = *engine_d;
    engine_state *state = get_engine_state(engine_d);
    game_d.memory = state->game_memory;
    game_d.memory_size = state->game_memory_size;
    return game_d;
}

GLOBAL engine_state* g_engine_state;

b32 engine_update(engine_data *data, gl_functions gl_fns, float delta_time) {
    engine_state *state = get_engine_state(data);
    g_engine_state = state;
    if (!state->initialized) {
        SDL_Log("Initializing game state...");
        state->initialized = true;

        SDL_SetWindowSize(data->window, REF_W, REF_H);
        // TODO: properly enter and exit text input mode
        //SDL_StartTextInput();

        u32 engine_scratch_pool_size = 10 * Mb;
        state->pool =
            new_mem_pool((u8*)data->memory + sizeof(*state), engine_scratch_pool_size);
        state->game_memory = (u8*)data->memory  + sizeof(*state) + engine_scratch_pool_size;
        state->game_memory_size = data->memory_size - sizeof(*state) - engine_scratch_pool_size;

        // minimal shader
        {
            const char *vertex_shader_src = "#version 100\n"
                "uniform mat4 mvp;\n"
                "attribute vec2 pos;\n"
                "attribute vec4 col;\n"
                "varying vec4 frag_col;\n"
                "void main() {\n"
                "   frag_col = col;\n"
                "   gl_Position = mvp * vec4(pos.xy, 0, 1);\n"
                "}\n";

            const char *fragment_shader_src = "#version 100\n"
                "precision mediump float;\n"
                "varying vec4 frag_col;\n"
                "void main(){\n"
                "   gl_FragColor = frag_col;\n"
                "}\n";

            b32 result = compile_shader_program(gl_fns, &state->minimal_shader.id,
                vertex_shader_src, fragment_shader_src);
            cn_assert(result);

            gl(glGenBuffers(1, &state->minimal_shader.v_array));
        }
        gl_set(state->minimal_shader.u_mvp, glGetUniformLocation(state->minimal_shader.id, "mvp"));
        gl_set(state->minimal_shader.a_pos, glGetAttribLocation(state->minimal_shader.id, "pos"));
        gl_set(state->minimal_shader.a_col, glGetAttribLocation(state->minimal_shader.id, "col"));

        // UI shader
        {
            const char *vertex_shader_src = "#version 100\n"
                "uniform mat4 mvp;\n"
                "attribute vec2 pos;\n"
                "attribute vec2 uv;\n"
                "attribute vec4 col;\n"
                "varying vec2 frag_uv;\n"
                "varying vec4 frag_col;\n"
                "void main() {\n"
                "   frag_uv = uv;\n"
                "   frag_col = col;\n"
                "   gl_Position = mvp * vec4(pos.xy, 0, 1);\n"
                "}\n";

            const char *fragment_shader_src =
                "#version 100\n"
                "precision mediump float;\n"
                "uniform sampler2D tex;\n"
                "varying vec2 frag_uv;\n"
                "varying vec4 frag_col;\n"
                "void main(){\n"
                "   gl_FragColor = frag_col * texture2D(tex, frag_uv.st);\n"
                "}\n";

            b32 result = compile_shader_program(gl_fns, &state->ui_shader.id, vertex_shader_src,
                fragment_shader_src);
            cn_assert(result);
        }

        gl_set(state->ui_shader.u_tex, glGetUniformLocation(state->ui_shader.id, "tex"));
        gl_set(state->ui_shader.u_mvp, glGetUniformLocation(state->ui_shader.id, "mvp"));
        gl_set(state->ui_shader.a_pos, glGetAttribLocation(state->ui_shader.id, "pos"));
        gl_set(state->ui_shader.a_uv, glGetAttribLocation(state->ui_shader.id, "uv"));
        gl_set(state->ui_shader.a_col, glGetAttribLocation(state->ui_shader.id, "col"));

        gl(glGenBuffers(1, &state->ui_shader.v_array));
        gl(glGenBuffers(1, &state->ui_shader.i_array));

        // initialize Nuclear
        {
            state->ui_cmd_buff.type = NK_BUFFER_FIXED;
            state->ui_cmd_buff.memory.ptr = state->ui_cmd_buff_mem;
            state->ui_cmd_buff.memory.size = state->ui_cmd_buff.size =
                sizeof(state->ui_cmd_buff_mem);
            state->ui_cmd_buff.memory = (struct nk_memory) {
                .ptr = state->ui_cmd_buff_mem,
                    .size = sizeof(state->ui_cmd_buff_mem)
            };

            int nk_init_result =
                nk_init_fixed(&state->ui_context, state->ui_pool_buff_mem,
                    sizeof(state->ui_pool_buff_mem), &state->ui_font.nk_font);
            cn_assert(nk_init_result);
        }

        engine_hotload(data);
    }

    // WINDOW RESIZE HANDLING
    if (state->window_w != data->window_w || state->window_h != data->window_h) {
        char buff[1024];
        stb_snprintf(buff, 1024, "res change from(%d, %d), to(%d, %d)", state->window_w,
            state->window_h, data->window_w, data->window_h);
        SDL_Log("%s", buff);

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

        // minimal shader MVP setup
        gl(glUseProgram(state->minimal_shader.id));
        gl(glUniformMatrix4fv(state->minimal_shader.u_mvp, 1, false, mvp));

        // ui shader MVP setup
        gl(glUseProgram(state->ui_shader.id));
        gl(glUniformMatrix4fv(state->ui_shader.u_mvp, 1, false, mvp));

        gl(glViewport(0, 0, data->window_w, data->window_h));

        state->window_w = data->window_w;
        state->window_h = data->window_h;

        // bake fonts
        {
            mem_pool pool = state->pool;
            struct stbtt_fontinfo font_info;
            SDL_RWops *font_file = SDL_RWFromFile("assets/fonts/hack.ttf", "rb");
            cn_assert(font_file);
            u64 font_size = SDL_RWsize(font_file);
            void *font_contents = mem_push(&pool, font_size);
            SDL_RWread(font_file, font_contents, 1, SDL_RWsize(font_file));
            SDL_RWclose(font_file);

            s32 result = stbtt_InitFont(&font_info, (u8 *)font_contents, 0);
            cn_assert(result);

            u32 w = 512, h = 512;
            u8 *font_texture_alpha = mem_push(&pool, w * h);
            struct stbtt_pack_context pack_context;
            result = stbtt_PackBegin(&pack_context, font_texture_alpha, w, h, 0, 1, 0);
            cn_assert(result);

            float bake_height = 16.0f * scale;
            result = stbtt_PackFontRange(&pack_context, font_contents, 0, bake_height, 0x0000,
                arr_len(state->ui_font.packed_chars),
                state->ui_font.packed_chars);
            cn_assert(result);
            stbtt_PackEnd(&pack_context);

            u32 *font_texture = mem_push(&pool, w * h * sizeof(*font_texture));
            for (u32 y = 0; y < h; ++y) {
                for (u32 x = 0; x < w; ++x) {
                    u32 idx = y * w + x;
                    font_texture[idx] = ((u32)font_texture_alpha[idx] << 24) | 0x00FFFFFF;
                }
            }

            state->ui_font.tex_w = w;
            state->ui_font.tex_h = h;
            gl(glGenTextures(1, &state->ui_font.texture));
            gl(glBindTexture(GL_TEXTURE_2D, state->ui_font.texture));
            gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
            gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                font_texture));

            struct nk_user_font nk_font;
            nk_font.userdata.ptr = &state->ui_font;
            nk_font.height = 16.0f;
            nk_font.width = ui_text_width_fn;
            nk_font.query = ui_query_font_glyph_fn;
            nk_font.texture.id = (int)state->ui_font.texture;
            float font_scale = stbtt_ScaleForPixelHeight(&font_info, bake_height);
            int unscaled_ascent;
            stbtt_GetFontVMetrics(&font_info, &unscaled_ascent, 0, 0);
            state->ui_font.ascent = unscaled_ascent * font_scale;

            state->ui_font.nk_font = nk_font;
            state->ui_font.height = bake_height;
            state->ui_font.spacing = 0;

            GLuint null_texture;
            gl(glGenTextures(1, &null_texture));
            gl(glBindTexture(GL_TEXTURE_2D, null_texture));
            gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            gl(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            u8 null_texture_data[] ={ 255, 255, 255, 255 };
            gl(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                null_texture_data));
            state->ui_draw_null_tex.texture.id = null_texture;
            state->ui_draw_null_tex.uv.x = 0;
            state->ui_draw_null_tex.uv.y = 0;
        }
    }

    // reset callbacks and do other stuff that needs to be done after every hot-reload of the code
    {
        state->ui_font.nk_font.width = ui_text_width_fn;
        state->ui_font.nk_font.query = ui_query_font_glyph_fn;
    }

    // pump events into UI
    {
        float ref_aspect = (float)REF_H / (float)REF_W;
        float actual_aspect = (float)data->window_h / (float)data->window_w;
        float scale;
        if (ref_aspect > actual_aspect)
            scale = (float)data->window_h / (float)REF_H;
        else
            scale = (float)data->window_w / (float)REF_W;

        float shift_x = (data->window_w - REF_W * scale) * 0.5f;
        float shift_y = (data->window_h - REF_H * scale) * 0.5f;

        s32 mx = (s32)((data->mouse.x - shift_x) * (1.0f / scale));
        s32 my = (s32)((data->mouse.y - shift_y) * (1.0f / scale));

        nk_input_begin(&state->ui_context);

        nk_input_motion(&state->ui_context, mx, my);
        nk_input_button(&state->ui_context, NK_BUTTON_LEFT, mx, my, data->mouse.lmb);
        nk_input_button(&state->ui_context, NK_BUTTON_RIGHT, mx, my, data->mouse.rmb);

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // HACK: kinda hacky...
            if (event.type == SDL_QUIT) return false;

            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                SDL_KeyboardEvent kb_event = event.key;
                SDL_Keysym keysym = kb_event.keysym;
                b32 is_keydown = event.type == SDL_KEYDOWN;
                if (keysym.mod & KMOD_CTRL) {
                    if (keysym.sym == SDLK_LEFT) {
                        nk_input_key(&state->ui_context, NK_KEY_TEXT_WORD_LEFT, is_keydown);
                    } else if (keysym.sym == SDLK_RIGHT) {
                        nk_input_key(&state->ui_context, NK_KEY_TEXT_WORD_RIGHT, is_keydown);
                    } else if (keysym.sym == SDLK_z) {
                        nk_input_key(&state->ui_context, NK_KEY_TEXT_UNDO, is_keydown);
                    } else if (keysym.sym == SDLK_a) {
                        nk_input_key(&state->ui_context, NK_KEY_TEXT_SELECT_ALL, is_keydown);
                    } else if (keysym.sym == SDLK_x) {
                        nk_input_key(&state->ui_context, NK_KEY_CUT, is_keydown);
                    } else if (keysym.sym == SDLK_c) {
                        nk_input_key(&state->ui_context, NK_KEY_COPY, is_keydown);
                    } else if (keysym.sym == SDLK_v) {
                        nk_input_key(&state->ui_context, NK_KEY_PASTE, is_keydown);
                    }
                } else {
                    enum nk_keys nk_k = sdlkey_to_nkkey(keysym.sym);
                    if (nk_k != NK_KEY_NONE)
                        nk_input_key(&state->ui_context, nk_k, is_keydown);
                }
            } else if (event.type == SDL_TEXTINPUT) {
                SDL_TextInputEvent text_ev = event.text;
                nk_input_char(&state->ui_context, text_ev.text[0]);
            }
        }

        nk_input_end(&state->ui_context);
    }

    // draw UI
    {
        struct nk_context *ctx = &state->ui_context;

        // perf stats
        state->accumulated_dt += delta_time;
        ++state->frames_tracked;
        const float dt_update_period = 0.5;
        if (state->accumulated_dt >= dt_update_period) {
            state->average_dt = state->accumulated_dt / (float)state->frames_tracked;
            state->accumulated_dt = 0;
            state->frames_tracked = 0;
        }

        if (nk_begin(ctx, "stats", nk_rect(10, 10, 110, 90),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
            nk_layout_row_dynamic(ctx, 15.0f, 1);
            nk_labelf(ctx, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM, "%u:%u", state->window_w,
                state->window_h);
            nk_labelf(ctx, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM, "%05.2fms",
                state->average_dt * 1000.0f);
        }
        nk_end(ctx);

        // update the game
        engine_data game_d = game_data_from_engine_data(data);
        if (!game_update(&game_d, gl_fns, ctx, delta_time)) {
            return false;
        }
    }

    gl(glClearColor(0.7f, 0.7f, 0.7f, 1.0f));
    gl(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // render UI
    {
        /* setup GLOBAL state */
        gl(glEnable(GL_BLEND));
        gl(glBlendEquation(GL_FUNC_ADD));
        gl(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
        gl(glDisable(GL_DEPTH_TEST));
        gl(glActiveTexture(GL_TEXTURE0));

        /* setup program */
        gl(glUseProgram(state->ui_shader.id));
        gl(glUniform1i(state->ui_shader.u_tex, 0));
        {
            /* convert from command queue into draw list and draw to screen */
            const struct nk_draw_command *cmd;
            const nk_draw_index *offset = NULL;

            /* load draw vertices & elements directly into vertex + element buffer */
            {
                /* fill converting configuration */
                struct nk_convert_config config;
                const struct nk_draw_vertex_layout_element vertex_layout[] ={
                    { NK_VERTEX_POSITION, NK_FORMAT_FLOAT, NK_OFFSETOF(ui_vertex, position) },
                    { NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, NK_OFFSETOF(ui_vertex, uv) },
                    { NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, NK_OFFSETOF(ui_vertex, col) },
                    { NK_VERTEX_LAYOUT_END } };
                NK_MEMSET(&config, 0, sizeof(config));
                config.vertex_layout = vertex_layout;
                config.vertex_size = sizeof(ui_vertex);
                config.vertex_alignment = NK_ALIGNOF(ui_vertex);
                config.circle_segment_count = 22;
                config.curve_segment_count = 22;
                config.arc_segment_count = 22;
                config.global_alpha = 1.0f;
                config.shape_AA = NK_ANTI_ALIASING_ON;
                config.line_AA = NK_ANTI_ALIASING_OFF;
                config.null = state->ui_draw_null_tex;

                /* setup buffers to load vertices and elements */
                {
                    struct nk_buffer vbuf, ebuf;
                    nk_buffer_init_fixed(&vbuf, state->ui_vertex_buffer,
                        sizeof(state->ui_vertex_buffer));
                    nk_buffer_init_fixed(&ebuf, state->ui_element_buffer,
                        sizeof(state->ui_element_buffer));
                    nk_convert(&state->ui_context, &state->ui_cmd_buff, &vbuf, &ebuf, &config);
                }
            }

            /* buffer setup */
            GLsizei vs = sizeof(ui_vertex);
            size_t vp = offsetof(ui_vertex, position);
            size_t vt = offsetof(ui_vertex, uv);
            size_t vc = offsetof(ui_vertex, col);

            gl(glBindBuffer(GL_ARRAY_BUFFER, state->ui_shader.v_array));
            gl(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, state->ui_shader.i_array));

            gl(glEnableVertexAttribArray((GLuint)state->ui_shader.a_pos));
            gl(glEnableVertexAttribArray((GLuint)state->ui_shader.a_uv));
            gl(glEnableVertexAttribArray((GLuint)state->ui_shader.a_col));

            gl(glVertexAttribPointer((GLuint)state->ui_shader.a_pos, 2, GL_FLOAT, GL_FALSE, vs,
                (void *)vp));
            gl(glVertexAttribPointer((GLuint)state->ui_shader.a_uv, 2, GL_FLOAT, GL_FALSE, vs,
                (void *)vt));
            gl(glVertexAttribPointer((GLuint)state->ui_shader.a_col, 4, GL_UNSIGNED_BYTE, GL_TRUE,
                vs, (void *)vc));

            gl(glBufferData(GL_ARRAY_BUFFER, sizeof(state->ui_vertex_buffer),
                state->ui_vertex_buffer, GL_STREAM_DRAW));
            gl(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(state->ui_element_buffer),
                state->ui_element_buffer, GL_STREAM_DRAW));

            /* iterate over and execute each draw command */
            nk_draw_foreach(cmd, &state->ui_context, &state->ui_cmd_buff) {
                if (!cmd->elem_count)
                    continue;
                gl(glBindTexture(GL_TEXTURE_2D, (GLuint)cmd->texture.id));
                gl(glDrawElements(GL_TRIANGLES, (GLsizei)cmd->elem_count, GL_UNSIGNED_SHORT,
                    offset));
                offset += cmd->elem_count;
            }
            nk_clear(&state->ui_context);
        }

        /* default OpenGL state */
        gl(glUseProgram(0));
        gl(glBindBuffer(GL_ARRAY_BUFFER, 0));
        gl(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        gl(glEnable(GL_DEPTH_TEST));
        gl(glDisable(GL_BLEND));
    }

    SDL_GL_SwapWindow(data->window);

    return true;
}

void engine_hotload(engine_data *data) {
    engine_data game_d = game_data_from_engine_data(data);
    game_hotload(&game_d);
}

void engine_hotunload(engine_data *data) {
    engine_data game_d = game_data_from_engine_data(data);
    game_hotload(&game_d);
}