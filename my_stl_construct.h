#ifndef __MY_STL_CONSTRUCT__
#define __MY_STL_CONSTRUCT__ 1

#include <iostream>
#include "my_stl_type_traits.h"
#include "my_stl_iterator.h"

////////////////////////////////////////////////////////////////////////////
//构造
//定位new，在_p处调用_T1的构造函数_T1::_T1(value)
template <class _T1 ,class _T2>
inline void construct(_T1* _p,const _T2& _value){
    new (_p) _T1(_value);
}

///////////////////////////////////////////////////////////////////////////////
//析构
/*指针版*/
template <class _T>
inline void 
destroy(_T* _p){
    _p->~_T();
}

/*迭代器版*/

//_destroy内部选用的函数，版本1，没有默认的析构函数
template <class ForwardIterator>
inline void 
_destroy_aux(ForwardIterator _first ,ForwardIterator _last,__false_type){
    //情况1：没有默认的析构函数，对迭代器范围内的元素依次调用其析构函数,即指针版本
    while(_first != _last){
        destroy(&(*_first));
        ++_first;
    }
}


//_my_destroy内部选用的函数，版本2，有默认的析构函数
template <class ForwardIterator>
inline void 
_destroy_aux(ForwardIterator _first ,ForwardIterator _last,__true_type){
    //情况2：有默认的析构函数，直接调用其默认的析构函数，即什么都不做
    /*什么也不做*/
}

//destroy内部调用函数
template <class ForwardIterator,class _Type>
inline void 
_destroy(ForwardIterator _first ,ForwardIterator _last,_Type*){
    //再查看该萃取出来的类型是否有默认的析构函数
    using trivial_destructor  = typename __type_traits<_Type>::_has_trivial_destructor;
    _destroy_aux(_first,_last,trivial_destructor());
}



//destroy[_first,_last）区间的所有元素
template <class ForwardIterator>
inline void 
destroy(ForwardIterator _first, ForwardIterator _last){
    //_my_destroy(_first, _last,_my_iterator_traits<_first>::_value_type);
    //先将迭代器所指的元素类型萃取出来
    _destroy(_first, _last,iterator_value_type(_first));
}





//char*特化版
inline void destroy(char*,char*){}
//wchar_t*特化版
inline void destroy(wchar_t*,wchar_t*){}

////////////////////////////////////////////////////////////////////////////


#endif //__MY_STL_CONSTRUCT_