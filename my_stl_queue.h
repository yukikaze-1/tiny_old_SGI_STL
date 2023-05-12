#ifndef __MY_STL_DEQUE_H__
#define __MY_STL_DEQUE_H__ 1

#include "my_memory.h"
#include "my_stl_deque.h"


template<class T,class Sequence>
class queue{
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
        operator== (const queue<T,Sequence>& _this,const queue<T,Sequence>& _other);

        template<class _T,class _Sequence>
        friend bool 
        operator!= (const queue<T,Sequence>& _this,const queue<T,Sequence>& _other);


    public:
        bool empty() { return container.empty(); }
        _size_type size(){return container.size();}

        _reference front() { return container.front(); }
        _const_reference front()const { return container.front(); }
        _reference back() { return container.back(); }
        _const_reference back()const { return container.back();}

        void push(const _value_type& _value){container.push_front(_value);}
        void pop(){container.pop_back();}
};


template<class T,class Sequence>
inline
bool operator==(const queue<T,Sequence>& _this,const queue<T,Sequence>& _other){
    return _this.container == _other.container;
}

template<class T,class Sequence>
inline
bool operator!=(const queue<T,Sequence>& _this,const queue<T,Sequence>& _other){
    return _this.container != _other.container;
}

#endif  /* __MY_STL_DEQUE_H__ */