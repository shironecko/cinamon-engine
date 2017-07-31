#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>

#include <platform.h>
#include <engine.h>

#include <bgfx/c99/bgfx.h>

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

LOCAL void load_bgfx_api(bgfx_api *bgfx, const char* path) {
    SDL_assert(bgfx);
    bgfx->bgfx_lib = SDL_LoadObject(path);
    SDL_assert(bgfx->bgfx_lib);

    bgfx->vertex_decl_begin = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_decl_begin"); SDL_assert(bgfx->vertex_decl_begin);
    bgfx->vertex_decl_add = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_decl_add"); SDL_assert(bgfx->vertex_decl_add);
    bgfx->vertex_decl_skip = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_decl_skip"); SDL_assert(bgfx->vertex_decl_skip);
    bgfx->vertex_decl_end = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_decl_end"); SDL_assert(bgfx->vertex_decl_end);
    bgfx->vertex_pack = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_pack"); SDL_assert(bgfx->vertex_pack);
    bgfx->vertex_unpack = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_unpack"); SDL_assert(bgfx->vertex_unpack);
    bgfx->vertex_convert = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_vertex_convert"); SDL_assert(bgfx->vertex_convert);
    bgfx->weld_vertices = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_weld_vertices"); SDL_assert(bgfx->weld_vertices);
    bgfx->topology_convert = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_topology_convert"); SDL_assert(bgfx->topology_convert);
    bgfx->topology_sort_tri_list = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_topology_sort_tri_list"); SDL_assert(bgfx->topology_sort_tri_list);
    bgfx->get_supported_renderers = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_supported_renderers"); SDL_assert(bgfx->get_supported_renderers);
    bgfx->get_renderer_name = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_renderer_name"); SDL_assert(bgfx->get_renderer_name);
    bgfx->init = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_init"); SDL_assert(bgfx->init);
    bgfx->shutdown = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_shutdown"); SDL_assert(bgfx->shutdown);
    bgfx->reset = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_reset"); SDL_assert(bgfx->reset);
    bgfx->frame = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_frame"); SDL_assert(bgfx->frame);
    bgfx->get_renderer_type = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_renderer_type"); SDL_assert(bgfx->get_renderer_type);
    bgfx->get_caps = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_caps"); SDL_assert(bgfx->get_caps);
    bgfx->get_hmd = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_hmd"); SDL_assert(bgfx->get_hmd);
    bgfx->get_stats = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_stats"); SDL_assert(bgfx->get_stats);
    bgfx->alloc = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc"); SDL_assert(bgfx->alloc);
    bgfx->copy = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_copy"); SDL_assert(bgfx->copy);
    bgfx->make_ref = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_make_ref"); SDL_assert(bgfx->make_ref);
    bgfx->make_ref_release = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_make_ref_release"); SDL_assert(bgfx->make_ref_release);
    bgfx->set_debug = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_debug"); SDL_assert(bgfx->set_debug);
    bgfx->dbg_text_clear = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dbg_text_clear"); SDL_assert(bgfx->dbg_text_clear);
    bgfx->dbg_text_printf = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dbg_text_printf"); SDL_assert(bgfx->dbg_text_printf);
    bgfx->dbg_text_vprintf = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dbg_text_vprintf"); SDL_assert(bgfx->dbg_text_vprintf);
    bgfx->dbg_text_image = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dbg_text_image"); SDL_assert(bgfx->dbg_text_image);
    bgfx->create_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_index_buffer"); SDL_assert(bgfx->create_index_buffer);
    bgfx->destroy_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_index_buffer"); SDL_assert(bgfx->destroy_index_buffer);
    bgfx->create_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_vertex_buffer"); SDL_assert(bgfx->create_vertex_buffer);
    bgfx->destroy_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_vertex_buffer"); SDL_assert(bgfx->destroy_vertex_buffer);
    bgfx->create_dynamic_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_dynamic_index_buffer"); SDL_assert(bgfx->create_dynamic_index_buffer);
    bgfx->create_dynamic_index_buffer_mem = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_dynamic_index_buffer_mem"); SDL_assert(bgfx->create_dynamic_index_buffer_mem);
    bgfx->update_dynamic_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_update_dynamic_index_buffer"); SDL_assert(bgfx->update_dynamic_index_buffer);
    bgfx->destroy_dynamic_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_dynamic_index_buffer"); SDL_assert(bgfx->destroy_dynamic_index_buffer);
    bgfx->create_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_dynamic_vertex_buffer"); SDL_assert(bgfx->create_dynamic_vertex_buffer);
    bgfx->create_dynamic_vertex_buffer_mem = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_dynamic_vertex_buffer_mem"); SDL_assert(bgfx->create_dynamic_vertex_buffer_mem);
    bgfx->update_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_update_dynamic_vertex_buffer"); SDL_assert(bgfx->update_dynamic_vertex_buffer);
    bgfx->destroy_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_dynamic_vertex_buffer"); SDL_assert(bgfx->destroy_dynamic_vertex_buffer);
    bgfx->get_avail_transient_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_avail_transient_index_buffer"); SDL_assert(bgfx->get_avail_transient_index_buffer);
    bgfx->get_avail_transient_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_avail_transient_vertex_buffer"); SDL_assert(bgfx->get_avail_transient_vertex_buffer);
    bgfx->get_avail_instance_data_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_avail_instance_data_buffer"); SDL_assert(bgfx->get_avail_instance_data_buffer);
    bgfx->alloc_transient_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc_transient_index_buffer"); SDL_assert(bgfx->alloc_transient_index_buffer);
    bgfx->alloc_transient_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc_transient_vertex_buffer"); SDL_assert(bgfx->alloc_transient_vertex_buffer);
    bgfx->alloc_transient_buffers = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc_transient_buffers"); SDL_assert(bgfx->alloc_transient_buffers);
    bgfx->alloc_instance_data_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc_instance_data_buffer"); SDL_assert(bgfx->alloc_instance_data_buffer);
    bgfx->create_indirect_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_indirect_buffer"); SDL_assert(bgfx->create_indirect_buffer);
    bgfx->destroy_indirect_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_indirect_buffer"); SDL_assert(bgfx->destroy_indirect_buffer);
    bgfx->create_shader = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_shader"); SDL_assert(bgfx->create_shader);
    bgfx->get_shader_uniforms = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_shader_uniforms"); SDL_assert(bgfx->get_shader_uniforms);
    bgfx->get_uniform_info = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_uniform_info"); SDL_assert(bgfx->get_uniform_info);
    bgfx->destroy_shader = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_shader"); SDL_assert(bgfx->destroy_shader);
    bgfx->create_program = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_program"); SDL_assert(bgfx->create_program);
    bgfx->create_compute_program = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_compute_program"); SDL_assert(bgfx->create_compute_program);
    bgfx->destroy_program = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_program"); SDL_assert(bgfx->destroy_program);
    bgfx->is_texture_valid = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_is_texture_valid"); SDL_assert(bgfx->is_texture_valid);
    bgfx->calc_texture_size = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_calc_texture_size"); SDL_assert(bgfx->calc_texture_size);
    bgfx->create_texture = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_texture"); SDL_assert(bgfx->create_texture);
    bgfx->create_texture_2d = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_texture_2d"); SDL_assert(bgfx->create_texture_2d);
    bgfx->create_texture_2d_scaled = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_texture_2d_scaled"); SDL_assert(bgfx->create_texture_2d_scaled);
    bgfx->create_texture_3d = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_texture_3d"); SDL_assert(bgfx->create_texture_3d);
    bgfx->create_texture_cube = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_texture_cube"); SDL_assert(bgfx->create_texture_cube);
    bgfx->update_texture_2d = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_update_texture_2d"); SDL_assert(bgfx->update_texture_2d);
    bgfx->update_texture_3d = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_update_texture_3d"); SDL_assert(bgfx->update_texture_3d);
    bgfx->update_texture_cube = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_update_texture_cube"); SDL_assert(bgfx->update_texture_cube);
    bgfx->read_texture = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_read_texture"); SDL_assert(bgfx->read_texture);
    bgfx->destroy_texture = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_texture"); SDL_assert(bgfx->destroy_texture);
    bgfx->create_frame_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_frame_buffer"); SDL_assert(bgfx->create_frame_buffer);
    bgfx->create_frame_buffer_scaled = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_frame_buffer_scaled"); SDL_assert(bgfx->create_frame_buffer_scaled);
    bgfx->create_frame_buffer_from_handles = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_frame_buffer_from_handles"); SDL_assert(bgfx->create_frame_buffer_from_handles);
    bgfx->create_frame_buffer_from_attachment = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_frame_buffer_from_attachment"); SDL_assert(bgfx->create_frame_buffer_from_attachment);
    bgfx->create_frame_buffer_from_nwh = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_frame_buffer_from_nwh"); SDL_assert(bgfx->create_frame_buffer_from_nwh);
    bgfx->get_texture = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_texture"); SDL_assert(bgfx->get_texture);
    bgfx->destroy_frame_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_frame_buffer"); SDL_assert(bgfx->destroy_frame_buffer);
    bgfx->create_uniform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_uniform"); SDL_assert(bgfx->create_uniform);
    bgfx->destroy_uniform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_uniform"); SDL_assert(bgfx->destroy_uniform);
    bgfx->create_occlusion_query = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_create_occlusion_query"); SDL_assert(bgfx->create_occlusion_query);
    bgfx->get_result = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_get_result"); SDL_assert(bgfx->get_result);
    bgfx->destroy_occlusion_query = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_destroy_occlusion_query"); SDL_assert(bgfx->destroy_occlusion_query);
    bgfx->set_palette_color = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_palette_color"); SDL_assert(bgfx->set_palette_color);
    bgfx->set_view_name = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_name"); SDL_assert(bgfx->set_view_name);
    bgfx->set_view_rect = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_rect"); SDL_assert(bgfx->set_view_rect);
    bgfx->set_view_rect_auto = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_rect_auto"); SDL_assert(bgfx->set_view_rect_auto);
    bgfx->set_view_scissor = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_scissor"); SDL_assert(bgfx->set_view_scissor);
    bgfx->set_view_clear = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_clear"); SDL_assert(bgfx->set_view_clear);
    bgfx->set_view_clear_mrt = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_clear_mrt"); SDL_assert(bgfx->set_view_clear_mrt);
    bgfx->set_view_mode = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_mode"); SDL_assert(bgfx->set_view_mode);
    bgfx->set_view_frame_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_frame_buffer"); SDL_assert(bgfx->set_view_frame_buffer);
    bgfx->set_view_transform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_transform"); SDL_assert(bgfx->set_view_transform);
    bgfx->set_view_transform_stereo = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_transform_stereo"); SDL_assert(bgfx->set_view_transform_stereo);
    bgfx->set_view_order = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_view_order"); SDL_assert(bgfx->set_view_order);
    bgfx->reset_view = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_reset_view"); SDL_assert(bgfx->reset_view);
    bgfx->set_marker = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_marker"); SDL_assert(bgfx->set_marker);
    bgfx->set_state = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_state"); SDL_assert(bgfx->set_state);
    bgfx->set_condition = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_condition"); SDL_assert(bgfx->set_condition);
    bgfx->set_stencil = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_stencil"); SDL_assert(bgfx->set_stencil);
    bgfx->set_scissor = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_scissor"); SDL_assert(bgfx->set_scissor);
    bgfx->set_scissor_cached = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_scissor_cached"); SDL_assert(bgfx->set_scissor_cached);
    bgfx->set_transform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_transform"); SDL_assert(bgfx->set_transform);
    bgfx->alloc_transform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_alloc_transform"); SDL_assert(bgfx->alloc_transform);
    bgfx->set_transform_cached = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_transform_cached"); SDL_assert(bgfx->set_transform_cached);
    bgfx->set_uniform = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_uniform"); SDL_assert(bgfx->set_uniform);
    bgfx->set_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_index_buffer"); SDL_assert(bgfx->set_index_buffer);
    bgfx->set_dynamic_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_dynamic_index_buffer"); SDL_assert(bgfx->set_dynamic_index_buffer);
    bgfx->set_transient_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_transient_index_buffer"); SDL_assert(bgfx->set_transient_index_buffer);
    bgfx->set_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_vertex_buffer"); SDL_assert(bgfx->set_vertex_buffer);
    bgfx->set_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_dynamic_vertex_buffer"); SDL_assert(bgfx->set_dynamic_vertex_buffer);
    bgfx->set_transient_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_transient_vertex_buffer"); SDL_assert(bgfx->set_transient_vertex_buffer);
    bgfx->set_instance_data_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_instance_data_buffer"); SDL_assert(bgfx->set_instance_data_buffer);
    bgfx->set_instance_data_from_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_instance_data_from_vertex_buffer"); SDL_assert(bgfx->set_instance_data_from_vertex_buffer);
    bgfx->set_instance_data_from_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_instance_data_from_dynamic_vertex_buffer"); SDL_assert(bgfx->set_instance_data_from_dynamic_vertex_buffer);
    bgfx->set_texture = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_texture"); SDL_assert(bgfx->set_texture);
    bgfx->touch = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_touch"); SDL_assert(bgfx->touch);
    bgfx->submit = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_submit"); SDL_assert(bgfx->submit);
    bgfx->submit_occlusion_query = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_submit_occlusion_query"); SDL_assert(bgfx->submit_occlusion_query);
    bgfx->submit_indirect = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_submit_indirect"); SDL_assert(bgfx->submit_indirect);
    bgfx->set_image = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_image"); SDL_assert(bgfx->set_image);
    bgfx->set_compute_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_compute_index_buffer"); SDL_assert(bgfx->set_compute_index_buffer);
    bgfx->set_compute_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_compute_vertex_buffer"); SDL_assert(bgfx->set_compute_vertex_buffer);
    bgfx->set_compute_dynamic_index_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_compute_dynamic_index_buffer"); SDL_assert(bgfx->set_compute_dynamic_index_buffer);
    bgfx->set_compute_dynamic_vertex_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_compute_dynamic_vertex_buffer"); SDL_assert(bgfx->set_compute_dynamic_vertex_buffer);
    bgfx->set_compute_indirect_buffer = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_set_compute_indirect_buffer"); SDL_assert(bgfx->set_compute_indirect_buffer);
    bgfx->dispatch = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dispatch"); SDL_assert(bgfx->dispatch);
    bgfx->dispatch_indirect = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_dispatch_indirect"); SDL_assert(bgfx->dispatch_indirect);
    bgfx->discard = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_discard"); SDL_assert(bgfx->discard);
    bgfx->blit = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_blit"); SDL_assert(bgfx->blit);
    bgfx->request_screen_shot = SDL_LoadFunction(bgfx->bgfx_lib, "bgfx_request_screen_shot"); SDL_assert(bgfx->request_screen_shot);
}

typedef struct {
	b32 (*engine_update)(engine_data *data, float delta_time);
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
	                     SDL_WINDOW_RESIZABLE);

    load_bgfx_api(&engine_data.bgfx, "..\\build\\bgfx.dll");

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

		    continueRunning = engine_lib.engine_update(&engine_data, delta_time);
        }

	} while (continueRunning);

	return 0;
}
