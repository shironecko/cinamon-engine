typedef struct {
    s32 x, y;
    b32 lmb, rmb;
} mouse_info;

typedef struct {
    void *memory;
    u32 memory_size;

    const u8 *kb;
    mouse_info mouse;

    SDL_Window *window;
    s32 window_w;
    s32 window_h;
} engine_data;

#if defined(__IPHONEOS__) || defined(__ANDROID__) || defined(__EMSCRIPTEN__) || defined(__NACL__)
#define HAVE_OPENGLES
#endif

#if defined(HAVE_OPENGLES)
#include <SDL_opengles2.h>
#else
#include <SDL_opengl.h>
#endif

typedef struct {
#define gl_function(ret, func, params) ret(APIENTRY *func) params;
#include <gl_functions.h>
#undef gl_function
} gl_functions;

#define cn_assert SDL_assert

#define arr_len(arr) (sizeof(arr) / sizeof(arr[0]))

b32 engine_update(engine_data *data, gl_functions gl, float delta_time);

void engine_hotload(engine_data *data);
void engine_hotunload(engine_data *data);
