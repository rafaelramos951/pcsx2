/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021 PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "GSVector.h"

#ifdef _WIN32
#include "common/RedtapeWindows.h"
inline std::string convert_utf16_to_utf8(const std::wstring& utf16_string)
{
	const int size = WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), utf16_string.size(), nullptr, 0, nullptr, nullptr);
	std::string converted_string(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, utf16_string.c_str(), utf16_string.size(), converted_string.data(), converted_string.size(), nullptr, nullptr);
	return converted_string;
}

inline std::wstring convert_utf8_to_utf16(const std::string& utf8_string)
{
	int size = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, nullptr, 0);
	std::vector<wchar_t> converted_string(size);
	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, converted_string.data(), converted_string.size());
	return {converted_string.data()};
}
#endif

/// Like `memcmp(&a, &b, sizeof(T)) == 0` but faster
template <typename T>
__forceinline bool BitEqual(const T& a, const T& b)
{
#if _M_SSE >= 0x501
	if (alignof(T) >= 32)
	{
		GSVector8i eq = GSVector8i::xffffffff();
		for (size_t i = 0; i < sizeof(T) / 32; i++)
			eq &= reinterpret_cast<const GSVector8i*>(&a)[i].eq8(reinterpret_cast<const GSVector8i*>(&b)[i]);
		return eq.alltrue();
	}
#endif
	GSVector4i eq = GSVector4i::xffffffff();
	if (alignof(T) >= 16)
	{
		for (size_t i = 0; i < sizeof(T) / 16; i++)
			eq &= reinterpret_cast<const GSVector4i*>(&a)[i].eq8(reinterpret_cast<const GSVector4i*>(&b)[i]);
		return eq.alltrue();
	}
	const char* ac = reinterpret_cast<const char*>(&a);
	const char* bc = reinterpret_cast<const char*>(&b);
	size_t i = 0;
	if (sizeof(T) >= 16)
	{
		for (; i < sizeof(T) - 15; i += 16)
			eq &= GSVector4i::load<false>(ac + i).eq8(GSVector4i::load<false>(bc + i));
	}
	if (i + 8 <= sizeof(T))
	{
		eq &= GSVector4i::loadl(ac + i).eq8(GSVector4i::loadl(bc + i));
		i += 8;
	}
	bool eqb = eq.alltrue();
	if (i + 4 <= sizeof(T))
	{
		u32 ai, bi;
		memcpy(&ai, ac + i, sizeof(ai));
		memcpy(&bi, bc + i, sizeof(bi));
		eqb = ai == bi && eqb;
		i += 4;
	}
	if (i + 2 <= sizeof(T))
	{
		u16 as, bs;
		memcpy(&as, ac + i, sizeof(as));
		memcpy(&bs, bc + i, sizeof(bs));
		eqb = as == bs && eqb;
		i += 2;
	}
	if (i != sizeof(T))
	{
		ASSERT(i + 1 == sizeof(T));
		eqb = ac[i] == bc[i] && eqb;
	}
	return eqb;
}

// _wfopen has to be used on Windows for pathnames containing non-ASCII characters.
inline FILE* px_fopen(const std::string& filename, const std::string& mode)
{
#ifdef _WIN32
	return _wfopen(convert_utf8_to_utf16(filename).c_str(), convert_utf8_to_utf16(mode).c_str());
#else
	return fopen(filename.c_str(), mode.c_str());
#endif
}

#ifdef ENABLE_ACCURATE_BUFFER_EMULATION
static const GSVector2i default_rt_size(2048, 2048);
#else
static const GSVector2i default_rt_size(0, 0);
#endif

// Maximum texture size to skip preload/hash path.
// This is the width/height from the registers, i.e. not the power of 2.
__fi static bool CanPreloadTextureSize(u32 tw, u32 th)
{
	static constexpr u32 MAXIMUM_SIZE_IN_ONE_DIRECTION = 10; // 1024
	static constexpr u32 MAXIMUM_SIZE_IN_OTHER_DIRECTION = 8; // 256
	static constexpr u32 MAXIMUM_SIZE_IN_BOTH_DIRECTIONS = 9; // 512

	// We use an area-based approach here. We want to hash long font maps,
	// like 128x1024 (used in FFX), but skip 1024x512 textures (e.g. Xenosaga).
	const u32 max_dimension = (tw > th) ? tw : th;
	const u32 min_dimension = (tw > th) ? th : tw;
	if (max_dimension <= MAXIMUM_SIZE_IN_BOTH_DIRECTIONS)
		return true;

	return (max_dimension <= MAXIMUM_SIZE_IN_ONE_DIRECTION &&
			min_dimension <= MAXIMUM_SIZE_IN_OTHER_DIRECTION);
}

// Maximum number of mipmap levels for a texture.
// PS2 has a max of 7 levels (1 base + 6 mips).
static constexpr int MAXIMUM_TEXTURE_MIPMAP_LEVELS = 7;

// Helper path to dump texture
extern const std::string root_sw;
extern const std::string root_hw;

extern std::string format(const char* fmt, ...);

extern void* vmalloc(size_t size, bool code);
extern void vmfree(void* ptr, size_t size);

extern void* fifo_alloc(size_t size, size_t repeat);
extern void fifo_free(void* ptr, size_t size, size_t repeat);

// clang-format off

#ifdef __POSIX__
	#include <zlib.h>
#else
	#include <zlib/zlib.h>
#endif

#ifdef _MSC_VER
	#define ALIGN_STACK(n) alignas(n) int dummy__; (void)dummy__;
#else
	#ifdef __GNUC__
		// GCC removes the variable as dead code and generates some warnings.
		// Stack is automatically realigned due to SSE/AVX operations
		#define ALIGN_STACK(n) (void)0;
	#else
		// TODO Check clang behavior
		#define ALIGN_STACK(n) alignas(n) int dummy__;
	#endif
#endif

#ifdef ENABLE_VTUNE
	#include "jitprofiling.h"
	#ifdef _WIN32
		#pragma comment(lib, "jitprofiling.lib")
	#endif
#endif

#ifdef _WIN32
	#define DIRECTORY_SEPARATOR '\\'
#else
	#include <sys/stat.h> // mkdir
	#define DIRECTORY_SEPARATOR '/'
#endif
