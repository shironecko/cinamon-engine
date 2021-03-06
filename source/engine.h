#include <bgfx/c99/bgfx.h>
#include <bgfx/c99/platform.h>

typedef struct {
    void *bgfx_lib;

    void (*set_platform_data)(const bgfx_platform_data_t* _data);
    void (*vertex_decl_begin)(bgfx_vertex_decl_t* _decl, bgfx_renderer_type_t _renderer);
    void (*vertex_decl_add)(bgfx_vertex_decl_t* _decl, bgfx_attrib_t _attrib, uint8_t _num, bgfx_attrib_type_t _type, bool _normalized, bool _asInt);
    void (*vertex_decl_skip)(bgfx_vertex_decl_t* _decl, uint8_t _num);
    void (*vertex_decl_end)(bgfx_vertex_decl_t* _decl);
    void (*vertex_pack)(const float _input[4], bool _inputNormalized, bgfx_attrib_t _attr, const bgfx_vertex_decl_t* _decl, void* _data, uint32_t _index);
    void (*vertex_unpack)(float _output[4], bgfx_attrib_t _attr, const bgfx_vertex_decl_t* _decl, const void* _data, uint32_t _index);
    void (*vertex_convert)(const bgfx_vertex_decl_t* _destDecl, void* _destData, const bgfx_vertex_decl_t* _srcDecl, const void* _srcData, uint32_t _num);
    uint16_t (*weld_vertices)(uint16_t* _output, const bgfx_vertex_decl_t* _decl, const void* _data, uint16_t _num, float _epsilon);
    uint32_t (*topology_convert)(bgfx_topology_convert_t _conversion, void* _dst, uint32_t _dstSize, const void* _indices, uint32_t _numIndices, bool _index32);
    void (*topology_sort_tri_list)(bgfx_topology_sort_t _sort, void* _dst, uint32_t _dstSize, const float _dir[3], const float _pos[3], const void* _vertices, uint32_t _stride, const void* _indices, uint32_t _numIndices, bool _index32);
    uint8_t (*get_supported_renderers)(uint8_t _max, bgfx_renderer_type_t* _enum);
    const char* (*get_renderer_name)(bgfx_renderer_type_t _type);
    bool (*init)(bgfx_renderer_type_t _type, uint16_t _vendorId, uint16_t _deviceId, bgfx_callback_interface_t* _callback, bgfx_allocator_interface_t* _allocator);
    void (*shutdown)();
    void (*reset)(uint32_t _width, uint32_t _height, uint32_t _flags);
    uint32_t (*frame)(bool _capture);
    bgfx_renderer_type_t (*get_renderer_type)();
    const bgfx_caps_t* (*get_caps)();
    const bgfx_hmd_t* (*get_hmd)();
    const bgfx_stats_t* (*get_stats)();
    const bgfx_memory_t* (*alloc)(uint32_t _size);
    const bgfx_memory_t* (*copy)(const void* _data, uint32_t _size);
    const bgfx_memory_t* (*make_ref)(const void* _data, uint32_t _size);
    const bgfx_memory_t* (*make_ref_release)(const void* _data, uint32_t _size, bgfx_release_fn_t _releaseFn, void* _userData);
    void (*set_debug)(uint32_t _debug);
    void (*dbg_text_clear)(uint8_t _attr, bool _small);
    void (*dbg_text_printf)(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, ...);
    void (*dbg_text_vprintf)(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, va_list _argList);
    void (*dbg_text_image)(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const void* _data, uint16_t _pitch);
    bgfx_index_buffer_handle_t (*create_index_buffer)(const bgfx_memory_t* _mem, uint16_t _flags);
    void (*destroy_index_buffer)(bgfx_index_buffer_handle_t _handle);
    bgfx_vertex_buffer_handle_t (*create_vertex_buffer)(const bgfx_memory_t* _mem, const bgfx_vertex_decl_t* _decl, uint16_t _flags);
    void (*destroy_vertex_buffer)(bgfx_vertex_buffer_handle_t _handle);
    bgfx_dynamic_index_buffer_handle_t (*create_dynamic_index_buffer)(uint32_t _num, uint16_t _flags);
    bgfx_dynamic_index_buffer_handle_t (*create_dynamic_index_buffer_mem)(const bgfx_memory_t* _mem, uint16_t _flags);
    void (*update_dynamic_index_buffer)(bgfx_dynamic_index_buffer_handle_t _handle, uint32_t _startIndex, const bgfx_memory_t* _mem);
    void (*destroy_dynamic_index_buffer)(bgfx_dynamic_index_buffer_handle_t _handle);
    bgfx_dynamic_vertex_buffer_handle_t (*create_dynamic_vertex_buffer)(uint32_t _num, const bgfx_vertex_decl_t* _decl, uint16_t _flags);
    bgfx_dynamic_vertex_buffer_handle_t (*create_dynamic_vertex_buffer_mem)(const bgfx_memory_t* _mem, const bgfx_vertex_decl_t* _decl, uint16_t _flags);
    void (*update_dynamic_vertex_buffer)(bgfx_dynamic_vertex_buffer_handle_t _handle, uint32_t _startVertex, const bgfx_memory_t* _mem);
    void (*destroy_dynamic_vertex_buffer)(bgfx_dynamic_vertex_buffer_handle_t _handle);
    uint32_t (*get_avail_transient_index_buffer)(uint32_t _num);
    uint32_t (*get_avail_transient_vertex_buffer)(uint32_t _num, const bgfx_vertex_decl_t* _decl);
    uint32_t (*get_avail_instance_data_buffer)(uint32_t _num, uint16_t _stride);
    void (*alloc_transient_index_buffer)(bgfx_transient_index_buffer_t* _tib, uint32_t _num);
    void (*alloc_transient_vertex_buffer)(bgfx_transient_vertex_buffer_t* _tvb, uint32_t _num, const bgfx_vertex_decl_t* _decl);
    bool (*alloc_transient_buffers)(bgfx_transient_vertex_buffer_t* _tvb, const bgfx_vertex_decl_t* _decl, uint32_t _numVertices, bgfx_transient_index_buffer_t* _tib, uint32_t _numIndices);
    const bgfx_instance_data_buffer_t* (*alloc_instance_data_buffer)(uint32_t _num, uint16_t _stride);
    bgfx_indirect_buffer_handle_t (*create_indirect_buffer)(uint32_t _num);
    void (*destroy_indirect_buffer)(bgfx_indirect_buffer_handle_t _handle);
    bgfx_shader_handle_t (*create_shader)(const bgfx_memory_t* _mem);
    uint16_t (*get_shader_uniforms)(bgfx_shader_handle_t _handle, bgfx_uniform_handle_t* _uniforms, uint16_t _max);
    void (*get_uniform_info)(bgfx_uniform_handle_t _handle, bgfx_uniform_info_t* _info);
    void (*destroy_shader)(bgfx_shader_handle_t _handle);
    bgfx_program_handle_t (*create_program)(bgfx_shader_handle_t _vsh, bgfx_shader_handle_t _fsh, bool _destroyShaders);
    bgfx_program_handle_t (*create_compute_program)(bgfx_shader_handle_t _csh, bool _destroyShaders);
    void (*destroy_program)(bgfx_program_handle_t _handle);
    bool (*is_texture_valid)(uint16_t _depth, bool _cubeMap, uint16_t _numLayers, bgfx_texture_format_t _format, uint32_t _flags);
    void (*calc_texture_size)(bgfx_texture_info_t* _info, uint16_t _width, uint16_t _height, uint16_t _depth, bool _cubeMap, bool _hasMips, uint16_t _numLayers, bgfx_texture_format_t _format);
    bgfx_texture_handle_t (*create_texture)(const bgfx_memory_t* _mem, uint32_t _flags, uint8_t _skip, bgfx_texture_info_t* _info);
    bgfx_texture_handle_t (*create_texture_2d)(uint16_t _width, uint16_t _height, bool _hasMips, uint16_t _numLayers, bgfx_texture_format_t _format, uint32_t _flags, const bgfx_memory_t* _mem);
    bgfx_texture_handle_t (*create_texture_2d_scaled)(bgfx_backbuffer_ratio_t _ratio, bool _hasMips, uint16_t _numLayers, bgfx_texture_format_t _format, uint32_t _flags);
    bgfx_texture_handle_t (*create_texture_3d)(uint16_t _width, uint16_t _height, uint16_t _depth, bool _hasMips, bgfx_texture_format_t _format, uint32_t _flags, const bgfx_memory_t* _mem);
    bgfx_texture_handle_t (*create_texture_cube)(uint16_t _size, bool _hasMips, uint16_t _numLayers, bgfx_texture_format_t _format, uint32_t _flags, const bgfx_memory_t* _mem);
    void (*update_texture_2d)(bgfx_texture_handle_t _handle, uint16_t _layer, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const bgfx_memory_t* _mem, uint16_t _pitch);
    void (*update_texture_3d)(bgfx_texture_handle_t _handle, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _z, uint16_t _width, uint16_t _height, uint16_t _depth, const bgfx_memory_t* _mem);
    void (*update_texture_cube)(bgfx_texture_handle_t _handle, uint16_t _layer, uint8_t _side, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const bgfx_memory_t* _mem, uint16_t _pitch);
    uint32_t (*read_texture)(bgfx_texture_handle_t _handle, void* _data, uint8_t _mip);
    void (*destroy_texture)(bgfx_texture_handle_t _handle);
    bgfx_frame_buffer_handle_t (*create_frame_buffer)(uint16_t _width, uint16_t _height, bgfx_texture_format_t _format, uint32_t _textureFlags);
    bgfx_frame_buffer_handle_t (*create_frame_buffer_scaled)(bgfx_backbuffer_ratio_t _ratio, bgfx_texture_format_t _format, uint32_t _textureFlags);
    bgfx_frame_buffer_handle_t (*create_frame_buffer_from_handles)(uint8_t _num, const bgfx_texture_handle_t* _handles, bool _destroyTextures);
    bgfx_frame_buffer_handle_t (*create_frame_buffer_from_attachment)(uint8_t _num, const bgfx_attachment_t* _attachment, bool _destroyTextures);
    bgfx_frame_buffer_handle_t (*create_frame_buffer_from_nwh)(void* _nwh, uint16_t _width, uint16_t _height, bgfx_texture_format_t _depthFormat);
    bgfx_texture_handle_t (*get_texture)(bgfx_frame_buffer_handle_t _handle, uint8_t _attachment);
    void (*destroy_frame_buffer)(bgfx_frame_buffer_handle_t _handle);
    bgfx_uniform_handle_t (*create_uniform)(const char* _name, bgfx_uniform_type_t _type, uint16_t _num);
    void (*destroy_uniform)(bgfx_uniform_handle_t _handle);
    bgfx_occlusion_query_handle_t (*create_occlusion_query)();
    bgfx_occlusion_query_result_t (*get_result)(bgfx_occlusion_query_handle_t _handle, int32_t* _result);
    void (*destroy_occlusion_query)(bgfx_occlusion_query_handle_t _handle);
    void (*set_palette_color)(uint8_t _index, const float _rgba[4]);
    void (*set_view_name)(uint8_t _id, const char* _name);
    void (*set_view_rect)(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);
    void (*set_view_rect_auto)(uint8_t _id, uint16_t _x, uint16_t _y, bgfx_backbuffer_ratio_t _ratio);
    void (*set_view_scissor)(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);
    void (*set_view_clear)(uint8_t _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil);
    void (*set_view_clear_mrt)(uint8_t _id, uint16_t _flags, float _depth, uint8_t _stencil, uint8_t _0, uint8_t _1, uint8_t _2, uint8_t _3, uint8_t _4, uint8_t _5, uint8_t _6, uint8_t _7);
    void (*set_view_mode)(uint8_t _id, bgfx_view_mode_t _mode);
    void (*set_view_frame_buffer)(uint8_t _id, bgfx_frame_buffer_handle_t _handle);
    void (*set_view_transform)(uint8_t _id, const void* _view, const void* _proj);
    void (*set_view_transform_stereo)(uint8_t _id, const void* _view, const void* _projL, uint8_t _flags, const void* _projR);
    void (*set_view_order)(uint8_t _id, uint8_t _num, const void* _order);
    void (*reset_view)(uint8_t _id);
    void (*set_marker)(const char* _marker);
    void (*set_state)(uint64_t _state, uint32_t _rgba);
    void (*set_condition)(bgfx_occlusion_query_handle_t _handle, bool _visible);
    void (*set_stencil)(uint32_t _fstencil, uint32_t _bstencil);
    uint16_t (*set_scissor)(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);
    void (*set_scissor_cached)(uint16_t _cache);
    uint32_t (*set_transform)(const void* _mtx, uint16_t _num);
    uint32_t (*alloc_transform)(bgfx_transform_t* _transform, uint16_t _num);
    void (*set_transform_cached)(uint32_t _cache, uint16_t _num);
    void (*set_uniform)(bgfx_uniform_handle_t _handle, const void* _value, uint16_t _num);
    void (*set_index_buffer)(bgfx_index_buffer_handle_t _handle, uint32_t _firstIndex, uint32_t _numIndices);
    void (*set_dynamic_index_buffer)(bgfx_dynamic_index_buffer_handle_t _handle, uint32_t _firstIndex, uint32_t _numIndices);
    void (*set_transient_index_buffer)(const bgfx_transient_index_buffer_t* _tib, uint32_t _firstIndex, uint32_t _numIndices);
    void (*set_vertex_buffer)(uint8_t _stream, bgfx_vertex_buffer_handle_t _handle, uint32_t _startVertex, uint32_t _numVertices);
    void (*set_dynamic_vertex_buffer)(uint8_t _stream, bgfx_dynamic_vertex_buffer_handle_t _handle, uint32_t _startVertex, uint32_t _numVertices);
    void (*set_transient_vertex_buffer)(uint8_t _stream, const bgfx_transient_vertex_buffer_t* _tvb, uint32_t _startVertex, uint32_t _numVertices);
    void (*set_instance_data_buffer)(const bgfx_instance_data_buffer_t* _idb, uint32_t _num);
    void (*set_instance_data_from_vertex_buffer)(bgfx_vertex_buffer_handle_t _handle, uint32_t _startVertex, uint32_t _num);
    void (*set_instance_data_from_dynamic_vertex_buffer)(bgfx_dynamic_vertex_buffer_handle_t _handle, uint32_t _startVertex, uint32_t _num);
    void (*set_texture)(uint8_t _stage, bgfx_uniform_handle_t _sampler, bgfx_texture_handle_t _handle, uint32_t _flags);
    uint32_t (*touch)(uint8_t _id);
    uint32_t (*submit)(uint8_t _id, bgfx_program_handle_t _handle, int32_t _depth, bool _preserveState);
    uint32_t (*submit_occlusion_query)(uint8_t _id, bgfx_program_handle_t _program, bgfx_occlusion_query_handle_t _occlusionQuery, int32_t _depth, bool _preserveState);
    uint32_t (*submit_indirect)(uint8_t _id, bgfx_program_handle_t _handle, bgfx_indirect_buffer_handle_t _indirectHandle, uint16_t _start, uint16_t _num, int32_t _depth, bool _preserveState);
    void (*set_image)(uint8_t _stage, bgfx_uniform_handle_t _sampler, bgfx_texture_handle_t _handle, uint8_t _mip, bgfx_access_t _access, bgfx_texture_format_t _format);
    void (*set_compute_index_buffer)(uint8_t _stage, bgfx_index_buffer_handle_t _handle, bgfx_access_t _access);
    void (*set_compute_vertex_buffer)(uint8_t _stage, bgfx_vertex_buffer_handle_t _handle, bgfx_access_t _access);
    void (*set_compute_dynamic_index_buffer)(uint8_t _stage, bgfx_dynamic_index_buffer_handle_t _handle, bgfx_access_t _access);
    void (*set_compute_dynamic_vertex_buffer)(uint8_t _stage, bgfx_dynamic_vertex_buffer_handle_t _handle, bgfx_access_t _access);
    void (*set_compute_indirect_buffer)(uint8_t _stage, bgfx_indirect_buffer_handle_t _handle, bgfx_access_t _access);
    uint32_t (*dispatch)(uint8_t _id, bgfx_program_handle_t _handle, uint32_t _numX, uint32_t _numY, uint32_t _numZ, uint8_t _flags);
    uint32_t (*dispatch_indirect)(uint8_t _id, bgfx_program_handle_t _handle, bgfx_indirect_buffer_handle_t _indirectHandle, uint16_t _start, uint16_t _num, uint8_t _flags);
    void (*discard)();
    void (*blit)(uint8_t _id, bgfx_texture_handle_t _dst, uint8_t _dstMip, uint16_t _dstX, uint16_t _dstY, uint16_t _dstZ, bgfx_texture_handle_t _src, uint8_t _srcMip, uint16_t _srcX, uint16_t _srcY, uint16_t _srcZ, uint16_t _width, uint16_t _height, uint16_t _depth);
    void (*request_screen_shot)(bgfx_frame_buffer_handle_t _handle, const char* _filePath);
} bgfx_api;

typedef struct {
    s32 x, y;
    b32 lmb, rmb;
} mouse_info;

typedef struct {
    void *memory;
    u64 memory_size;

    const u8 *kb;
    mouse_info mouse;

    bgfx_api bgfx;

    SDL_Window *window;
    s32 window_w;
    s32 window_h;
} engine_data;

#define cn_assert SDL_assert

b32 engine_update(engine_data *data, float delta_time);

void engine_hotload(engine_data *data);
void engine_hotunload(engine_data *data);
