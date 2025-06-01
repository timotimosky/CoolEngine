
#include "include\geometry.h"

//模板定义后，就跟泛型编程一样，可以直接隐式实例化。
//当满足不了的情况下（处理一些特殊需求）才需要模板专门化

//模板专门化 : 也叫模板特化，描述时不用模板参数，用于实例化模板，分为全特化和偏特化 

//以 template<typename T1, typename T2>声明为例
//全特化就是限定死模板实现的具体类型  ：template <>  
//偏特化就是如果这个模板有多个类型，那么只限定其中的一部分： template <> 
//而对于函数模板，却只有全特化，不能偏特化：因为偏特化的功能可以通过函数的重载完成。

//其实偏特化 并没有实例化模板，跟函数重载类似，是一个新的模板

//全特化，才真正的实例化了模板。

//所以我们看到 全特化 要写在CPP文件。 偏特化还是要写在H头文件

// 因为 构造函数用float 构造为Int，所以使用两个模板 来分别指定类型
template <> template <> 
vec<3, int>  ::vec(const vec<3, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)), z(int(v.z + .5f)) {}

template <> template <> 
vec<3, float>::vec(const vec<3, int>& v) : x(v.x), y(v.y), z(v.z) {}

template <> template <>
vec<4, float>::vec(const vec<4, int>& v) : x(v.x), y(v.y), z(v.z),w(v.w) {}

//template <> template <> 
//vec<2, int>  ::vec(const vec<2, float>& v) : x(int(v.x + .5f)), y(int(v.y + .5f)) {}
//
//template <> template <> 
//vec<2, float>::vec(const vec<2, int>& v) : x(v.x), y(v.y) {}