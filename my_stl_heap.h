#ifndef __MY_STL_HEAP_H__
#define __MY_STL_HEAP_H__ 1

#include "my_stl_iterator.h"

template<class RandomAccessIterator,class Distance ,class T,class Compare>
inline void
_push_heap(RandomAccessIterator _first, Distance _topIndex, Distance _holeIndex,T _value,Compare _cmp){

    Distance _parent = (_holeIndex - 1) / 2;
    //当未达到顶端 且 父节点小于新值_value
    while(_holeIndex > _topIndex && _cmp(*(_first + _parent),_value)){
        //交换父子节点的值(实际上值把父节点的值复制给洞)
        *(_first + _holeIndex) = *(_first + _parent);
        //向上移动
        _holeIndex = _parent;
        _parent = (_holeIndex - 1) / 2;
    }

    //最后一步，新值赋值给_holeIndex所指的位置
    *(_first + _holeIndex) = _value;
}


//注意，调用该函数时，需保证新元素已被置于底层容器的尾部
template<class RandomAccessIterator,class Compare>
inline void
push_heap(RandomAccessIterator _first, RandomAccessIterator _last,Compare _cmp){

    using _DistanceType = typename iterator_traits<RandomAccessIterator>::_difference_type;
    using _ValueType    = typename iterator_traits<RandomAccessIterator>::_value_type;

    _ValueType _value = *(_last - 1);
    _push_heap(_first,_DistanceType((_last - _first) -1),_DistanceType(0),_value,_cmp);
}

//调整_holeIndex为首的树 为heap
//预调整的值为_value
template<class RandomAccessIterator, class Distance, class T,class Compare>
inline void
_adjust_heap(RandomAccessIterator _first,Distance _holeIndex,Distance _len ,T _value,Compare _cmp){
    Distance _topIndex = _holeIndex;
    //找到当前节点的右儿子节点
    Distance _secondChild = 2 * _holeIndex + 2;

    while(_secondChild < _len){
        //比较两个儿子节点的值，将secondChild指向其中较大的那一个
        //此处可优化，因为可能会调用值类型的构造函数，此处为按值传递
        if( _cmp(*(_first + _secondChild),*(_first + _secondChild -1)) ){
            --_secondChild;
        }
        //交换洞值和较大节点的值,实际上值将较大的值赋值到洞值上
        //表面上就是交换了两值
        *(_first + _holeIndex) = *(_first + _secondChild);
        //更新洞指针
        _holeIndex = _secondChild;
        //找到右儿子节点
        _secondChild = 2 * (_secondChild + 1);
    }

    //没有右儿子节点，只有左儿子节点
    //这种情况就是底部容器的尾部节点存放的是最值，该尾部节点刚好是右儿子节点
    //由于尾部值要被pop出去,就相当于没有右儿子
    //此时，交换洞值与左儿子值,实际上只是将做儿子值赋值给洞
    if(_secondChild == _len){
        *(_first + _holeIndex) = *(_first + _secondChild - 1);
        _holeIndex = _secondChild -1;
    }

    //似乎没有调用的必要？实验一下
    _push_heap(_first,_topIndex,_holeIndex,_value,_cmp);
}

template<class RandomAccessIterator,class Compare>
inline void
_pop_heap(RandomAccessIterator _first,RandomAccessIterator _last,RandomAccessIterator _result,Compare _cmp){
    
    using _DistanceType = typename iterator_traits<RandomAccessIterator>::_difference_type;
    using _ValueType    = typename iterator_traits<RandomAccessIterator>::_value_type;

    //将尾部的值存于一个变量中
    _ValueType _value = *_result;
    //根节点的值移动至尾部
    *_result = *_first;
    //调整heap使其变得有序(注意：调整的范围并不包括尾部，因为尾部
    //存放的是根节点的值[pop出的值])
    _adjust_heap(_first,_DistanceType(0),_DistanceType(_last - _first),_value,_cmp);
}

//注意，_last为超尾
template<class RandomAccessIterator
    ,class Compare = std::less<typename iterator_traits<RandomAccessIterator>::_value_type> >
inline void
pop_heap(RandomAccessIterator _first, RandomAccessIterator _last,Compare _cmp){
    
    if(_last - _first > 1){
        //因为_last为超尾，所以需要--_last
        --_last;
        _pop_heap(_first,_last,_last,_cmp);
    }   
}

//循环进行pop_heap，就可以形成一个有序序列
template<class RandomAccessIterator
        ,class Compare = std::less<typename iterator_traits<RandomAccessIterator>::_value_type> >
inline void
sort_heap(RandomAccessIterator _first,RandomAccessIterator _last,Compare _cmp){
    //每进行依次pop_heap，就会将一个极值放在尾部，
    //每次pop_heap时将_last向左移动一个距离即可
    while(_last - _first > 1){
        pop_heap(_first,_last--,_cmp);
    }
}

//自底向上调整子树为heap
template<class RandomAccessIterator,class Compare>
inline void
_make_heap(RandomAccessIterator _first, RandomAccessIterator _last,Compare _cmp){

    using _DistanceType = typename iterator_traits<RandomAccessIterator>::_difference_type;
    using _ValueType    = typename iterator_traits<RandomAccessIterator>::_value_type;

    //当长度为1 or 0 时，并不需要排序，直接返回
    if(_last - _first < 2)  return;

    //最尾部节点
    _DistanceType _len = _last - _first;
    //最尾部节点的父节点，也是第一个需要重排的子树的根节点
    _DistanceType _parent = (_len - 1)/2;

    while(true){
        _ValueType _value = *(_first + _parent);
        //重排以parent为首的子树
        _adjust_heap(_first,_parent,_len,_value,_cmp);
        if(_parent == 0) return;
        --_parent;
    }
}

//自底向上调整子树为heap
template<class RandomAccessIterator
        ,class Compare = std::less<typename iterator_traits<RandomAccessIterator>::_value_type> >
inline void
make_heap(RandomAccessIterator _first, RandomAccessIterator _last, Compare _cmp){
    _make_heap(_first,_last,_cmp);
}


#endif  //__MY_STL_HEAP_H__