#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>

#include <platform.h>
#include <engine.h>

#if defined(__WINDOWS__)
#define PT_ENGINE_DYNAMIC
#endif

#if !defined(PT_ENGINE_DYNAMIC)
#include <engine.c>
#endif

#if defined(__WINDOWS__)
#include <Windows.h>
#define platform_alloc(size, base_address)                                                         \
	VirtualAlloc((base_address), (size), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)
#else
#include <stdlib.h>
#define platform_alloc(size, base_address) malloc(size)
#endif

LOCAL s32 load_gl_functions(gl_functions *data) {
#if SDL_VIDEO_DRIVER_UIKIT || SDL_VIDEO_DRIVER_ANDROID || SDL_VIDEO_DRIVER_PANDORA
#define __SDL_NOGETPROCADDR__
#endif

#if defined __SDL_NOGETPROCADDR__
#define gl_function(ret, func, params) data->func = func;
#else
#define gl_function(ret, func, params)                                                             \
	do {                                                                                           \
		data->func = SDL_GL_GetProcAddress(#func);                                                 \
		if (!data->func) {                                                                         \
			return SDL_SetError("Couldn't load GL function %s: %s\n", #func, SDL_GetError());      \
		}                                                                                          \
	} while (0);
#endif /* __SDL_NOGETPROCADDR__ */

#include "gl_functions.h"
#undef gl_function
	return 0;
}

typedef struct {
	b32 (*engine_update)(engine_data *data, gl_functions gl, float delta_time);
    void (*engine_hotload)(engine_data *data);
    void (*engine_hotunload)(engine_data *data);

#if defined(PT_ENGINE_DYNAMIC)
	void *library;
	FILETIME last_write_time;
#endif
} engine_lib_info;

// TODO: make this shipping friendly way down the line
LOCAL void reload_engine_lib(char *src_lib_path, char *temp_lib_path, char *lock_file_path,
                           engine_lib_info *info, engine_data *data) {
#if defined(PT_ENGINE_DYNAMIC)
    b32 was_engine_lib_loaded = info->library ? true : false;
	SDL_RWops *lock_file = SDL_RWFromFile(lock_file_path, "r");
	if (lock_file) {
		SDL_assert(info->library);
		SDL_RWclose(lock_file);
		return;
	}

	HANDLE engine_lib_file = CreateFileA(src_lib_path, GENERIC_READ, FILE_SHARE_READ,
	                                   0, // security attributes
	                                   OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
	                                   0); // template file

	if (engine_lib_file == INVALID_HANDLE_VALUE) {
		SDL_assert(was_engine_lib_loaded);
		return;
	}

	FILETIME last_write_time;
	GetFileTime(engine_lib_file, 0, 0, &last_write_time);

	if (info->library &&
	    memcmp(&info->last_write_time, &last_write_time, sizeof(last_write_time)) == 0) {
		CloseHandle(engine_lib_file);
		return;
	}

	OutputDebugStringA("Reloading engine lib...\n");
    if (was_engine_lib_loaded && info->engine_hotunload) {
        info->engine_hotunload(data);
    }
	SDL_UnloadObject(info->library);
	HANDLE temp_lib_file = CreateFile(temp_lib_path, GENERIC_WRITE, FILE_SHARE_READ,
	                                  0, // security attributes
	                                  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
	                                  0); // template file

	// NOTE: all this hassle just cause CopyFile is failing for a second or so
	// after lib compile, thus adding lag to reload
	DWORD bytes_read;
	do {
		u8 buffer[64];
		ReadFile(engine_lib_file, buffer, sizeof(buffer), &bytes_read, 0);
		WriteFile(temp_lib_file, buffer, bytes_read, 0, 0);
	} while (bytes_read);

	CloseHandle(engine_lib_file);
	CloseHandle(temp_lib_file);

	info->library = SDL_LoadObject(temp_lib_path);
	info->engine_update = SDL_LoadFunction(info->library, "engine_update");
    info->engine_hotload = SDL_LoadFunction(info->library, "engine_hotload");
    info->engine_hotunload = SDL_LoadFunction(info->library, "engine_hotunload");
	info->last_write_time = last_write_time;

    // don't call hotload on first engine load
    if (was_engine_lib_loaded && info->engine_hotload) {
        info->engine_hotload(data);
    }
#else
	info->engine_update = engine_update;
    info->engine_hotload = engine_hotload;
    info->engine_hotunload = engine_hotunload;
#endif
}

int main(int argc, char **argv) {
	engine_data engine_data = {0};
	engine_data.memory_size = 128 * Mb;
	void *engine_mem_base_address =
#ifdef PT_DEV_BUILD
	    (void *)(2048ULL * Gb);
#else
	    0;
#endif
	engine_data.memory = platform_alloc(engine_data.memory_size, engine_mem_base_address);
	memset(engine_data.memory, 0, engine_data.memory_size);

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS);
	engine_data.window =
	    SDL_CreateWindow("demo window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600,
	                     SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(0);
	SDL_GLContext context = SDL_GL_CreateContext(engine_data.window);
	SDL_GL_MakeCurrent(engine_data.window, context);

	gl_functions gl_functions;
	s32 context_load_res = load_gl_functions(&gl_functions);
	SDL_assert(context_load_res >= 0);

	engine_lib_info engine_lib = {0};
	b32 continueRunning = true;
	u64 prev_frame_start_ticks = SDL_GetPerformanceCounter();

	do {
		u64 ticks = SDL_GetPerformanceCounter();
		u64 frequency = SDL_GetPerformanceFrequency();
		float delta_time = (float)((double)(ticks - prev_frame_start_ticks) / (double)frequency);
		prev_frame_start_ticks = ticks;

		/*SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) continueRunning = false;
		}*/

        if (continueRunning) {
		    // TODO: strip reloading from shipping version
		    reload_engine_lib("..\\build\\engine.dll", "..\\build\\engine_tmp.dll", "..\\build\\engine.lock",
		                    &engine_lib, &engine_data);
		    SDL_assert(engine_lib.engine_update);
		    SDL_GL_GetDrawableSize(engine_data.window, &engine_data.window_w, &engine_data.window_h);
		    engine_data.kb = SDL_GetKeyboardState(0);
		    s32 mouse_buttons = SDL_GetMouseState(&engine_data.mouse.x, &engine_data.mouse.y);
		    engine_data.mouse.lmb = mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		    engine_data.mouse.rmb = mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

		    continueRunning = engine_lib.engine_update(&engine_data, gl_functions, delta_time);
        }

	} while (continueRunning);

	return 0;
}
