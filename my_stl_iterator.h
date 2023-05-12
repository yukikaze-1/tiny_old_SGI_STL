#ifndef __MY_STL_ITERATOR_H__
#define __MY_STL_ITERATOR_H__ 1

#include <iostream>  // for std::static_cast<T>

#define ptrdiff_t long long


////////////////////////////////////////////////////////////////////////
//五种迭代器类型
class Input_iterator_tag{};
class Output_iterator_tag{};
class Forward_iterator_tag :public Input_iterator_tag{};
class Bidirectional_iterator_tag : public Forward_iterator_tag{};
class Random_iterator_tag : public Bidirectional_iterator_tag{};


//迭代器属性,自定义的迭代器从该类继承即可
//还需修改，将该类抽象出一个基类，该基类不含任何成员
//只含using声明。针对每一种容器，设计该容器自己的迭代器即可
//待修改*************************************************
template <class Category,class T,class Distance = ptrdiff_t
         ,class Pointer = T*,class Reference = T&>
class iterator{
    public:
        using _iterator_category    =  Category;
        using _value_type           =  T;
        using _difference_type      =  Distance;
        using _pointer              =  Pointer;
        using _reference            =  Reference;

    private:
        _pointer _current;

    public:
        iterator()=default;
        iterator(const iterator& _other):_current(_other.base()){}   

        _pointer base() const { return _current; }
        _reference operator*() const { auto _tmp = _current; return *(--_tmp); }
        _pointer operator->() const { return &(*(*this)); }

        iterator& operator++() { --_current; return *this; }
        iterator operator++(int) { auto _temp = *this; --_current; return _temp; }

        iterator& operator--() { ++_current; return *this; }
        iterator operator--(int) { auto _temp = *this; ++_current; return _temp; }

        friend bool operator==(const iterator& _lhs, const iterator& _rhs) {
            return _lhs._current_ == _rhs._current_;
        }

        friend bool operator!=(const iterator& _lhs, const iterator& _rhs) {
            return !(_lhs == _rhs);
        }

};

////////////////////////////////////////////////////////////////
//迭代器traits
template<class _T>
class iterator_traits{
    public:
    using _iterator_category =  typename _T::_iterator_category;
    using _value_type        =  typename _T::_value_type;
    using _difference_type   =  typename _T::_difference_type;
    using _pointer           =  typename _T::_pointer;
    using _reference         =  typename _T::_reference;
};

//迭代器萃取器
//原生指针特化版
template<class _T>
class iterator_traits <_T*>{
    public:
    using _iterator_category  = Random_iterator_tag;
    using _value_type         = _T;
    using _difference_type    = ptrdiff_t;
    using _pointer            = _T*;
    using _reference          = _T&;
};

//迭代器萃取器
//const指针特化版
template<class _T>
class iterator_traits <const _T*>{
    public:
    using _iterator_category    =   Random_iterator_tag;
    using _value_type           =   _T;
    using _difference_type      =   ptrdiff_t;
    using _pointer              =   const _T*;
    using _reference            =   const _T&;
};

////////////////////////////////////////////////////////////////////////////

//通过迭代器返回其迭代器类型（五中迭代器类型）
template <class Iterator>
inline typename iterator_traits<Iterator>::_iterator_category
iterator_category(const Iterator&){
    using _category = typename iterator_traits<Iterator>::_iterator_category; 
    return _category();
}

//返回迭代器所指内容的类型
template <class Iterator>
inline typename iterator_traits<Iterator>::_value_type*
iterator_value_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::_value_type*>(nullptr);
}

//返回迭代器的距离的类型
template <class Iterator>
inline typename iterator_traits<Iterator>::_difference_type*
iterator_distance_type(const Iterator&){
    return static_cast<typename iterator_traits<Iterator>::_difference_type*>(nullptr);   
}

////////////////////////////////////////////////////////////////////////////

//distance内部调用函数,InputIterator版
template <class InputIterator>
inline typename iterator_traits<InputIterator>::_difference_type 
_distance(InputIterator _begin,InputIterator _end,Input_iterator_tag){
    typename iterator_traits<InputIterator>::_difference_type _ret = 0;
    while(_begin != _end){
        ++_begin;
        ++_ret;
    }
    return _ret;
}

//distance内部调用函数,RandomIterator版
template <class InputIterator>
inline typename iterator_traits<InputIterator>::_difference_type 
_distance(InputIterator _begin,InputIterator _end,Random_iterator_tag){
    return _end - _begin;
}

//返回两个迭代器之间的距离
template <class InputIterator>
inline typename iterator_traits<InputIterator>::_difference_type 
distance(InputIterator _begin,InputIterator _end){
    //先萃取出迭代器是什么类型的迭代器
    using _category = typename iterator_traits<InputIterator>::_iterator_category ;
    return _distance(_begin,_end,_category());
}




//将迭代器移动指定的距离
template <class InputIterator,class Distance>
inline void
advance(InputIterator& _begin,Distance _distance){
    return _advance( _begin, _distance,iterator_category(_begin));
}

//advance内部调用函数,输入迭代器版本
template <class InputIterator,class Distance>
inline void
_advance(InputIterator& _begin,Distance _distance,Input_iterator_tag){
    while(_distance--){
        ++_begin;
    }
}

//advance内部调用函数,双向迭代器版本
template <class BidirectionalIterator,class Distance>
inline void
_advance(BidirectionalIterator& _begin,Distance _distance,
            Bidirectional_iterator_tag){
    if(_distance > 0){
        while(_distance--) ++_begin;
    }else{
        while(_distance++) --_begin;
    }
}


//advance内部调用函数,随机迭代器版本
template <class RandomIterator,class Distance>
inline void
_advance(RandomIterator& _begin,Distance _distance,Random_iterator_tag){
    _begin += _distance;
}

////////////////////////////////////////////////////////////////////////////
//反向迭代器

template<class Iterator>
class
reverse_iterator{
    
    public:
        using _iterator_type   = Iterator;
        using _value_type      = typename iterator_traits<Iterator>::_value_type;
        using _reference       = typename iterator_traits<Iterator>::_reference;
        using _pointer         = typename iterator_traits<Iterator>::_pointer;
        using _difference_type = typename iterator_traits<Iterator>::_difference_type;

    
    private:
        _iterator_type _current;
    
    public:

        reverse_iterator() : _current() {}
        explicit reverse_iterator(_iterator_type it) : _current(it) {}

        template <typename OtherIterator>
        reverse_iterator(const reverse_iterator<OtherIterator>& _other) : _current(_other.base()) {}

        _iterator_type base() const { return _current; }
        _reference operator*() const { auto _tmp = _current; return *(--_tmp); }
        _pointer operator->() const { return &(*(*this)); }

        reverse_iterator& operator++() { --_current; return *this; }
        reverse_iterator operator++(int) { auto _temp = *this; --_current; return _temp; }

        reverse_iterator& operator--() { ++_current; return *this; }
        reverse_iterator operator--(int) { auto _temp = *this; ++_current; return _temp; }

        reverse_iterator operator+(_difference_type n) const { return reverse_iterator(_current - n); }
        reverse_iterator& operator+=(_difference_type n) { _current -= n; return *this; }
        reverse_iterator operator-(_difference_type n) const { return reverse_iterator(_current + n); }
        reverse_iterator& operator-=(_difference_type n) { _current += n; return *this; }

        _reference operator[](_difference_type n) const { return *(*this + n); }


        friend bool operator==(const reverse_iterator& _lhs, const reverse_iterator& _rhs) {
            return _lhs.base() == _rhs.base();
        }

        friend bool operator!=(const reverse_iterator& _lhs, const reverse_iterator& _rhs) {
            return !(_lhs == _rhs);
        }


};




////////////////////////////////////////////////////////////////////////////
#endif /* __MY_STL_ITERATOR_H__ */