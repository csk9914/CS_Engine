#pragma once
#define PRIMITIVE_LIST \
DEFINE_TYPE(Plane, "Plane") \
DEFINE_TYPE(Cube, "Cube") \
DEFINE_TYPE(Sphere, "Sphere") \
DEFINE_TYPE(Capsule, "Capsule") \

enum class PrimitiveType
{
#define DEFINE_TYPE(id, str) id,
	PRIMITIVE_LIST
#undef DEFINE_TYPE

};