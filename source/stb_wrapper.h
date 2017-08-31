#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

/* ---STB lib--- */
#define STB_DEFINE
#define STB_NO_REGISTRY
#pragma warning(push, 0)
#include <stb.h>
#pragma warning(pop)

/* ---STB rect pack--- */
#ifdef STB_WRAPPER_RECT_PACK
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#endif // STB_WRAPPER_RECT_PACK

/* ---STB true type--- */
#ifdef STB_WRAPPER_TRUE_TYPE
#define STBTT_STATIC
#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>
#endif // STB_WRAPPER_TRUE_TYPE

/* ---STB image--- */
#ifdef STB_WRAPPER_IMAGE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif // STB_WRAPPER_IMAGE
