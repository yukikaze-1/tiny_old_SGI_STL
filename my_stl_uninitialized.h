#ifndef __MY_STL_UNINITIALIZED_H__
#define __MY_STL_UNINITIALIZED_H__ 1

#include "my_stl_iterator.h"
#include "my_stl_type_traits.h"
#include "my_stl_algorithm.h"
#include "my_stl_construct.h"
#include "my_stl_iterator.h"    


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//_uninitialized_fill_n内部调用函数，情况1：不是POD类型，对迭代器范围内的元素依次调用其构造函数
template <class ForwardIterator,class Size,class _T>
inline ForwardIterator 
_uninitialized_fill_n_aux(ForwardIterator _begin,Size _size,const _T& _value,__false_type){
    ForwardIterator _cur = _begin;
    while(_size){
        construct(&(*_cur),_value);
        --_size;
    }
    return _cur;
}

//_uninitialized_fill_n内部调用函数，情况2：是POD类型，
//直接调用stl fill_n()算法，高效。[此处调用的为我自己实现的my_fill_n()算法]
template <class ForwardIterator,class Size,class _T>
inline ForwardIterator 
_uninitialized_fill_n_aux(ForwardIterator _begin,Size _size,const _T& _value,__true_type){
    return my_fill_n(_begin,_size,_value);
}

//uninitialized_fill_n内部调用函数，判断元素类型是否为POD类型即含有默认的构造、析构、复制、赋值   函数）
template <class ForwardIterator,class Size,class _T,class _TYPE>
inline ForwardIterator 
_uninitialized_fill_n(ForwardIterator _begin,Size _size,const _T& _value,_TYPE* ){
    using is_POD = typename __type_traits<_TYPE>::_is_POD_type;
    return _uninitialized_fill_n_aux(_begin,_size,_value,is_POD());
}

//返回被填入区间的最后一个元素的下一个位置（超尾）
template <class ForwardIterator,class Size,class _T>
inline ForwardIterator 
uninitialized_fill_n(ForwardIterator _begin,Size _size,const _T& _value){
    //先萃取处迭代器所指的元素类型
    return _uninitialized_fill_n(_begin,_size,_value,iterator_value_type(_begin));
}
 


////////////////////////////////////////////////////////////////////////////////////////////////////////////

//_uninitialized_fill内部调用函数，情况1：不是POD类型，对迭代器范围内的元素依次调用其构造函数
template <class ForwardIterator,class _T>
inline void 
_uninitialized_fill_aux(ForwardIterator _begin,ForwardIterator _end,const _T& _value,__false_type){
    ForwardIterator _cur = _begin;
    while(_cur != _end){
        construct(&(*_cur),_value);
        ++_cur;
    }
}

//_uninitialized_fill内部调用函数，情况2：是POD类型，
//直接调用stl fill()算法，高效。[此处调用的为我自己实现的my_fill()算法]
template <class ForwardIterator,class _T>
inline void 
_uninitialized_fill_aux(ForwardIterator _begin,ForwardIterator _end,const _T& _value,__true_type){
    my_fill(_begin,_end,_value);
}

//uninitialized_fill内部调用函数，判断元素类型是否为POD类型即含有默认的构造、析构、复制、赋值   函数）
template <class ForwardIterator,class _T,class _TYPE>
inline void 
_uninitialized_fill(ForwardIterator _begin,ForwardIterator _end,const _T& _value,_TYPE*){
    using is_POD = typename __type_traits<_TYPE>::_is_POD_type;
    _uninitialized_fill_aux(_begin,_end,_value,is_POD());
}


//返回被填入区间的最后一个元素的下一个位置（超尾）
template <class ForwardIterator,class _T>
inline void 
uninitialized_fill(ForwardIterator _begin,ForwardIterator _end,const _T& _value){
    //先萃取处迭代器所指的元素类型
    _uninitialized_fill(_begin,_end,_value,iterator_value_type(_begin));
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////

//_uninitialized_copy内部调用函数，情况1：不是POD类型，对迭代器范围内的元素依次调用其构造函数
template <class InputIterator,class ForwardIterator>
inline ForwardIterator
_uninitialized_copy_aux(InputIterator _begin,InputIterator _end,
                           ForwardIterator _result,__false_type){
    InputIterator _cur = _result;
    while(_begin != _end){
        construct(&(*_cur),*_begin);
        ++_begin;
        ++_cur;
    }
    return _cur;
}

//_uninitialized_copy内部调用函数，情况2：是POD类型，
//直接调用stl copy()算法，高效。[此处调用的为我自己实现的my_copy()算法]
template <class InputIterator,class ForwardIterator>
inline ForwardIterator
_uninitialized_copy_aux(InputIterator _begin,InputIterator _end,
                           ForwardIterator _result,__true_type){
    return my_copy(_begin,_end,_result);
}

//_uninitialized_copy内部调用函数，判断元素类型是否是POD类型（即含有默认的构造、析构、复制、赋值   函数）
template <class InputIterator,class ForwardIterator,class _TYPE>
inline ForwardIterator
_uninitialized_copy(InputIterator _begin,InputIterator _end,
                       ForwardIterator _result,_TYPE*){
    using is_POD = typename __type_traits<_TYPE>::_is_POD_type;
    return _uninitialized_copy_aux(_begin,_end,_result,is_POD());
}


//返回被填入区间的最后一个元素的下一个位置（超尾）
template <class InputIterator,class ForwardIterator>
inline ForwardIterator 
uninitialized_copy(InputIterator _begin,InputIterator _end,
                      ForwardIterator _result){
     //先萃取处迭代器所指的元素类型
    return _uninitialized_copy(_begin,_end,_result,iterator_value_type(_begin));
}



/*以下为针对char*和w_chart*的特化版*/

//char*特化版
/*
inline char*  
uninitialized_copy(const char* _begin,const char* _end,char* _result){
    memmove(_result,_begin,_end - _begin);
    return _result + (_end - _begin);
}

//w_chart*特化版
inline wchar_t* 
uninitialized_copy(const wchar_t* _begin,const wchar_t* _end,wchar_t* _result){
    memmove(_result,_begin,sizeof(wchar_t) * (_end - _begin));
    return _result + (_end - _begin);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif /* __MY_STL_UNINITIALIZED_H__ */