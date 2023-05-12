#ifndef __MY_STL_TYPE_TRAITS_H__
#define __MY_STL_TYPE_TRAITS_H__ 1

struct __false_type{};
struct __true_type{};

////////////////////////////////////////////////////////////////////////////

//__type_traits
template <class _T>
class __type_traits{
public:
    using _this_dummy_member_must_be_first   = __true_type;

    using  _has_trivial_constructor          = __false_type;
    using  _has_trivial_destructor           = __false_type;
    using  _has_trivial_copy_constructor     = __false_type;
    using  _has_trivial_assignment_operator  = __false_type;
    using  _is_POD_type                      = __false_type;
};

//原生指针特化版
template <class _T>
class __type_traits<_T*>{
public:
    using _this_dummy_member_must_be_first   = __true_type;

    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};

//////////////////////////////////////////////////////////////////
//针对系统内置类型的显示具体化版本
template <>struct __type_traits<char>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};

template <>struct __type_traits<signed char>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<unsigned char>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};

template <>struct __type_traits<short>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};

template <>struct __type_traits<unsigned short>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<int>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<unsigned int>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<long>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<unsigned long>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<float>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<double>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<long double>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};
template <>struct __type_traits<long long>{
    using  _has_trivial_constructor          = __true_type;
    using  _has_trivial_destructor           = __true_type;
    using  _has_trivial_copy_constructor     = __true_type;
    using  _has_trivial_assignment_operator  = __true_type;
    using  _is_POD_type                      = __true_type;
};

//特化版结束
////////////////////////////////////////////////////////////////
#endif  /* __MY_STL_TYPE_TRAITS_H__ */