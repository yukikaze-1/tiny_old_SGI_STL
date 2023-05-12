#ifndef __MY_STL_ALGORITHM_H__
#define __MY_STL_ALGORITHM_H__ 1

#include "my_stl_iterator.h"
#include "my_stl_type_traits.h"
#include "my_stl_heap.h"

////////////////////////////////////////////////////////////////

//将_begin开始的_size个元素改为新值_value，
//返回被最后一个填入元素的下一个位置的迭代器
template <class OutputIterator,class Size,class _T>
OutputIterator
my_fill_n(OutputIterator _begin,Size _size,const _T& _value){
    while(_size){
        *_begin = _value;
        ++_begin;
        --_size;
    }
    return _begin;
}

//将[_begin,_end)内的所有元素改为新值(_value),不返回迭代器
template <class ForwardIterator,class _T>
void 
my_fill(ForwardIterator _begin,ForwardIterator _end,const _T& _value){
    while(_begin != _end){
        *_begin = _value;
        ++_begin;
    }
}



//从后往前依次复制元素到以_des为终点的目标区间
//返回copy后目标区间第一个元素之前的位置，类似于rend()
template <class BidirectionalIterator1,class BidirectionalIterator2>
BidirectionalIterator2
my_copy_backward( BidirectionalIterator1 _src_begin, BidirectionalIterator1 _src_end,
                  BidirectionalIterator2 _des ){
    while(_src_begin != _src_end){
        *(--_des) = *(--_src_end);
    }
    return _des;
}

////////////////////////////////////////////////////////////////////////////////////////////////
    /*copy的各种版本*/

//my_copy的泛化版本，当参数不为const char* 和const wchar_t*的时候
//且不为<T*,T*> <const T*,T*>时调用该模板
template<class InputIterator,class OutputIterator>
struct _my_copy_dispatch{
    OutputIterator operator()(InputIterator _src_begin,InputIterator _src_end,
                              OutputIterator _des){
        return _my_copy(_src_begin,_src_end,_des,iterator_category(_src_begin));
    }
};

//从前往后依次复制元素到以_des为起点的目标区间
//返回copy后目标区间最后一个元素的后一个位置
template <class InputIterator,class OutputIterator>
inline OutputIterator
my_copy(InputIterator _src_begin,InputIterator _src_end,OutputIterator _des){
    return _my_copy_dispatch<InputIterator,OutputIterator>()(_src_begin,_src_end,_des);
}



//_my_copy_dispatch内部调用函数
//此为输入迭代器版本，使用迭代器重载的 != 运算符来决定循环次数，速度慢
template <class InputIterator,class OutputIterator>
inline OutputIterator
_my_copy(InputIterator _src_begin,InputIterator _src_end,
         OutputIterator _des,Input_iterator_tag){
        while(_src_begin != _src_end){
            *_des = *_src_begin;
            ++_src_begin;
            ++_des;
        }
        return _des;
};

//_my_copy_dispatch内部调用函数
//此为随机迭代器版本，内部调用_my_copy_d函数，使用内置类型来决定循环次数，速度快
template <class RandomIterator,class OutputIterator>
inline OutputIterator
_my_copy(RandomIterator _src_begin,RandomIterator _src_end,
         OutputIterator _des,Random_iterator_tag){
    return _my_copy_d(_src_begin,_src_end,_des,iterator_distance_type(_src_begin));
};

//_my_copy内部调用函数
//_my_copy_t内部调用函数
template<class RandomIterator,class OutputIterator,class Distance>
inline OutputIterator
_my_copy_d(RandomIterator _src_begin,RandomIterator _src_end,
           OutputIterator _des,Distance*){
    Distance _distance = _src_end - _src_begin;
    while(_distance){
        *_des = *_src_begin;
        --_distance;
        ++_src_begin;
        ++_des;
    }
    return _des;
}

//有默认赋值构造函数，直接调用memmove()
template<class T>
inline T*
_my_copy_t(const T* _src_begin, const T* _src_end,T* _des,__true_type){
    memmove(_des,_src_begin,sizeof(T) * (_src_end - _src_begin));
    return _des + (_src_end - _src_begin);
}

//无默认复制构造函数，由于是指针（RandomIterator），故直接调用_my_copy_d()
template<class T>
inline T*
_my_copy_t(const T* _src_begin, const T* _src_end,T* _des,__false_type){
    return _my_copy_d(_src_begin,_src_end, _des,(ptrdiff_t*)(nullptr));
}

//my_copy的<T*,T*>特化版本，由于是指针类型，故要查看该类型是否有默认的赋值构造函数
//内部调用_my_copy_t()
template<class T>
struct _my_copy_dispatch<T*,T*>{
    T* operator()(T* _src_begin, T* _src_end,T* _des){
        using t = typename __type_traits<T>::_has_trivial_assignment_operator;
        return _my_copy_t(_src_begin, _src_end, _des,t());
    }
};


//my_copy的<const T*,T*>特化版本，由于是指针类型，故要查看该类型是否有默认的赋值构造函数
//内部调用_my_copy_t()
template<class T>
struct _my_copy_dispatch<const T*,T*>{
    T* operator()(const T* _src_begin,const T* _src_end,T* _des){
        using t = typename __type_traits<T>::_has_trivial_assignment_operator;
        return _my_copy_t(_src_begin, _src_end, _des,t());
    }
};

//my_copy的const char*的特化版，直接调用memmove()
inline char*
my_copy(const char* _src_begin,const char* _src_end,char* _des){
    memmove(_des,_src_begin,_src_end - _src_begin);
    return _des + (_src_end - _src_begin);
}

//my_copy的const wchar_t*的特化版，直接调用memmove()
inline wchar_t*
my_copy(const wchar_t* _src_begin,const wchar_t* _src_end,wchar_t* _des){
    memmove(_des,_src_begin,_src_end - _src_begin);
    return _des + (_src_end - _src_begin);
}


////////////////////////////////////////////////////////////////
#endif /* __MY_STL_ALGORITHM_H__ */