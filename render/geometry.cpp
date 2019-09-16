
#include "geometry.h"
//模板专门化 :template<>前缀表示这是一个专门化,描述时不用模板参数，用于实例化模板
template <> template <> 
vec<3, int>  ::vec(const vec<3, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}

template <> template <> 
vec<3, float>::vec(const vec<3, int>& v) : x(v.x), y(v.y), z(v.z) {}

template <> template <> 
vec<2, int>  ::vec(const vec<2, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)) {}

template <> template <> 
vec<2, float>::vec(const vec<2, int>& v) : x(v.x), y(v.y) {}