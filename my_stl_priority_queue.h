#ifndef __MT_STL_PRIORITY_QUEUE_H__
#define __MT_STL_PRIORITY_QUEUE_H__ 1

#include "my_stl_deque.h"
#include "my_stl_iterator.h"

template <class T,class Sequence = deque<T> ,class Compare = std::less<typename Sequence::_value_type> >
class
priority_queue{
    public:
        using _value_type      = T;
        using _size_type       = typename Sequence::_size_type;
        using _reference       = typename Sequence::_reference;
        using _const_reference = typename Sequence::_const_reference;
        using _container_type  = Sequence;

    protected:
        Sequence container;
        Compare compare;

    public:
        //默认构造函数
        priority_queue():container(){}
        //复制构造函数
        priority_queue(const priority_queue& _other):container(_other.container),compare(_other.compare){}
        //转换构造函数
        explicit priority_queue(const Compare& _other):container(),compare(_other) {}
        //移动构造函数,此处有疑问，如何实现？感觉我写的有问题
        priority_queue(priority_queue&& _other):container(std::move(_other.container)),compare(std::move(_other.compare)){}

        //复制构造函数
        template <class InputIterator>
        priority_queue(InputIterator first,InputIterator last,const Compare& cmp):container(first,last),compare(cmp){
            make_heap(container.begin(),container.end(),cmp);
        }

        //复制构造函数
        template <class InputIterator>
        priority_queue(InputIterator first,InputIterator last):container(first,last){
            make_heap(container.begin(),container.end(),this->compare);
        }

         template<class _T,class _Sequence>
        friend bool
        operator== (const priority_queue<T,Sequence>& _this,const priority_queue<T,Sequence>& _other);

        template<class _T,class _Sequence>
        friend bool 
        operator!= (const priority_queue<T,Sequence>& _this,const priority_queue<T,Sequence>& _other);

        bool empty() { return container.empty(); }
        _size_type size(){return container.size();}
        _const_reference top()const { return container.front(); }

        void push(const _value_type& value) {
            try{
                container.push_back(value);
                push_heap(container.begin(),container.end(),this->compare);
            }catch(...) {
                container.clear();
                throw("push failed,clear the container");
            }
        }

        void pop() {
            try{
                pop_heap(container.begin(),container.end(),compare);
                container.pop_back();
            }catch(...) {
                container.clear();
                throw("pop failed,clear the container");
            }
        }
};

template<class T,class Sequence,class Compare>
inline
bool operator==(const priority_queue<T,Sequence,Compare>& _this,const priority_queue<T,Sequence,Compare>& _other){
    return _this.container == _other.container;
}

template<class T,class Sequence,class Compare>
inline
bool operator!=(const priority_queue<T,Sequence,Compare>& _this,const priority_queue<T,Sequence,Compare>& _other){
    return _this.container != _other.container;
}

#endif  // __MT_STL_PRIORITY_QUEUE_H    