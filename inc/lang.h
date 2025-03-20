#ifndef __LANG_H__
#define __LANG_H__

#include <stdint.h>

#define VEC_ADD(type, A, B) \
	((type){ .x = A.x + B.x, .y = A.y + B.y, .z = A.z + B.z })
#define VEC_EQ(A, B) (A.x == B.x && A.y == B.y && A.z == B.z)
#define FRANGELOOP(type, iname, init, until) \
	for (type iname = (init); iname < (until); (iname++))
#define FRU(iname, until) FRANGELOOP(u32, iname, 0, until)
#define FIU(until) FRU(i, until)

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

#endif // __LANG_H__