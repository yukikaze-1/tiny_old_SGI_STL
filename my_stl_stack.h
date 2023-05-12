#ifndef __MY_STL_STACK_H__
#define __MY_STL_STACK_H__ 1

#include "my_stl_deque.h"
#include "my_memory.h"


template<class T,class Sequence = deque<T>>
class stack{
    public:
        using _value_type      = T;
        using _size_type       = typename Sequence::_size_type;
        using _reference       = typename Sequence::_reference;
        using _const_reference = typename Sequence::_const_reference;
        using _container_type  = Sequence;

    protected:
        Sequence container;

    public:
        template<class _T,class _Sequence>
        friend bool
        operator== (const stack<_T,_Sequence>&,const stack<_T,_Sequence>&);

        template<class _T,class _Sequence>
        friend bool 
        operator!= (const stack<_T,_Sequence>&,const stack<_T,_Sequence>&);

    public:
        bool empty() { return container.empty(); }
        _size_type size(){return container.size();}
        _reference top() { return container.back(); }
        _const_reference top()const { return container.back(); }

        void push(){container.push_back();}
        void pop(){container.pop_back();}

};

template<class T,class Sequence>
inline
bool operator==(const stack<T,Sequence>& _this,const stack<T,Sequence>& _other){
    return _this.container == _other.container;
}

template<class T,class Sequence>
inline
bool operator!=(const stack<T,Sequence>& _this,const stack<T,Sequence>& _other){
    return _this.container != _other.container;
}

#endif  /* __MY_STL_STACK_H__ */