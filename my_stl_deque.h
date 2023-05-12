#ifndef __MY_STL_DEQUE_H__
#define __MY_STL_DEQUE_H__ 1

#include "my_memory.h"
#include "my_stl_algorithm.h"

#define DEQUE_BUFFER_SIZE 512

inline size_t _deque_buf_size(size_t n,size_t size){
    return n != 0 ? n : (n < DEQUE_BUFFER_SIZE ? size_t(DEQUE_BUFFER_SIZE/size):size_t(1));
}


template <class T,class Ref = T& ,class Ptr = T*,size_t BufSize = 0>
struct _deque_iterator{
    //类型定义
    using _iterator_category      = Bidirectional_iterator_tag;
    using _iterator               = _deque_iterator<T, T&,       T*,       BufSize>;
    using _const_iterator         = _deque_iterator<T, const T&, const T*, BufSize>;
    using _reverse_iterator       = _deque_iterator<T, T&,       T*,       BufSize>;
    using _const_reverse_iterator = _deque_iterator<T, const T&, const T*, BufSize>;

    using _value_type      = T;
    using _pointer         = Ptr;
    using _const_pointer   = const Ptr;
    using _reference       = Ref;
    using _const_reference = const Ref;
    using _difference_type = ptrdiff_t;

    using _map_pointer = T**;
    using _self        = _deque_iterator;

    //内部指针
    //指向缓冲区中的一块
    T* _first;
    //注意，last为超尾
    T* _last;   
    //注意，_start._cur不是超尾
    //_finish._cur是超尾
    T* _cur;

    _map_pointer _node;//指向map中的一个node，而该node指向一块缓冲区

    //构造函数与析构函数
    _deque_iterator()=default;
    ~_deque_iterator()=default;
    
    //内部使用函数
    static size_t _buffer_size(){return _deque_buf_size(BufSize,sizeof(T));}
    
    //重新设置迭代器中指向map的node指针
    //注意，此处没有修改_cur指针
    void _set_node(_map_pointer _new_node){
        _node = _new_node;
        _first = *_new_node;
        _last = _first + _difference_type(_buffer_size());
    }

    //运算符重载
    _reference operator*()const{return *_cur;}
    _pointer operator->()const {return &(operator*());}
    _self& operator=(const _self&) = default;

        //将迭代器前进n个距离
    _self& operator+=(_difference_type n){

        //_offset代表移动后的迭代器与移动前的迭代器之间的距离
        _difference_type _offset = n + (_cur - _first);

        if(_offset > 0 && _offset < (_difference_type)(_buffer_size())){
            //目标位置在同一个缓冲区内
            _cur += n;

        }else{
            //目标位置不在同一个缓冲区内
                //计算map中节点的偏移量
            _difference_type _node_offset = 
                _offset > 0 ? _offset / (_difference_type)(_buffer_size())
                            : -(_difference_type)((-_offset - 1) / _buffer_size()) - 1;

                //将_node指向正确的map节点，_node,_first,_last都进行了设置，但是_cur的不正确
            _set_node(_node + _node_offset);

                //将_cur指向正确的元素
            _cur =  _first + (_offset - _node_offset * (_difference_type)(_buffer_size()));
        }
        return *this;
    }

    _self operator-=(_difference_type n){
        return *this += -n;
    }

    _self operator+(_difference_type n){
        _self _ret = *this;
        return _ret += n;
    }

    _difference_type operator-(const _self& _other)const{
        return (_difference_type)(_buffer_size()) * (_node - _other._node -1) 
                + (_cur - _first) + (_other._last - _other._cur);
    }

    _self operator-(_difference_type n){
        _self _ret = *this;
        return _ret -=n;
    }

    _self& operator++(){
        ++_cur;
        //last为超尾
        if(_cur == _last){
            _set_node(_node + 1);
            _cur = _first;
        }
        return *this;
    }

    _self operator++(int){
        _self _ret = *this;
        ++*this;
        return _ret;
    }

    _self& operator--(){
        if(_cur == _first){
            _set_node(_node - 1);
            _cur = _last;
        }
        --_cur;
        return *this;
    } 

    _self operator--(int){
        _self _ret = *this;
        --*this;
        return _ret;
    } 

    _reference operator[](_difference_type n){return *(*this + n);}
    
    bool operator==(const _self& _other)const{return this->_cur == _other._cur;}
    bool operator!=(const _self& _other)const{return !(*this == _other);}

    bool operator<=(const _self& _other)const{
        return this->_node == _other._node || this->_node < _other._node;
    }
    bool operator<(const _self& _other)const{return this->_node < _other._node;}
    bool operator>=(const _self& _other)const{
        return this->_node == _other._node || this->_node > _other._node;
    }
    bool operator>(const _self& _other)const{return this->_node > _other._node;}
};


template <class T,class _Alloc = alloc,size_t BufSize = 0>
class deque{
    public:
        //类型定义
        using _value_type      = T;
        using _pointer         = T*;
        using _const_pointer   = const T*;
        using _reference       = T&;
        using _const_reference = const T&;
        using _size_type       = size_t;
        using _difference_type = ptrdiff_t;
        using _self            = deque;
        using _index_type      = ptrdiff_t;

        using deque_data_allocator = simple_alloc<_value_type, _Alloc>;
        using deque_map_allocator  = simple_alloc<_pointer,    _Alloc>;

        using _iterator               = _deque_iterator<T, T&,       T*,       BufSize>;
        using _const_iterator         = _deque_iterator<T, const T&, const T*, BufSize>;
        using _reverse_iterator       = reverse_iterator<_iterator>;
        using _const_reverse_iterator = reverse_iterator<_const_iterator>;

    protected:
        //指向map的指针
        using _map_pointer = _pointer*;

        _iterator _start;
        _iterator _finish;

        //指向map的指针，该map数组内的元素为指针，而该指针指向一个缓冲区
        _map_pointer _map;   

        //map内含多少指针，即缓冲区的个数,map的capacity
        _size_type _map_size;

    public:
        //内存相关
        static size_t _buffer_size(){return _deque_buf_size(BufSize,sizeof(T));}

        _size_type _initial_map_size(){
            return _size_type(8);
        }

        //申请一个缓冲区的空间(即元素数组)，申请是按元素来申请的
        //该缓冲区中含有_buffer_size()个元素的空间
        _pointer _allocate_node(){
            return deque_data_allocator::allocate(_buffer_size());
        }

        //销毁一个缓冲区的空间
        //因为申请的是一个元素数组，故释放时需要加上长度
        void _deallocate_node(_pointer p){
            deque_data_allocator::deallocate(p,_buffer_size());
        }

        //负责产生并安排好deque的结构0
        void _create_map_and_node(_size_type _num_elements){

            //需要节点数，即缓冲区的个数，注意，会多配制一个缓冲区，即map的节点
            //+1是因为：A.如果刚好整除(eg: 10/5 = 2 => 3)，会多分配一个
            //缓冲区，这样如果要插入元素就不会引起缓冲区的申请
            //B.如果不是刚好整除(eg: 11/5 = 2...1 => 3)，也需要一个缓冲区
            _size_type _num_nodes = _num_elements / _buffer_size() + 1;

            //map中实际所申请的节点数，即缓冲区的数量，最少8个，最多是{所需节点+2}个 
            //多加两个是为了方便头尾扩充
            //_map_size相当于该map的capacity,_num_nodes相当于map的size
            _map_size = std::max(_initial_map_size(),_num_nodes + 2);

            //申请map的空间
            _map = deque_map_allocator::allocate(_map_size);    

            //_nstart和_nfinish分别指向map中已使用的节点的头和尾
            //_nfinish不是超尾！
            _map_pointer _nstart  = _map + (_map_size - _num_nodes)/2;
            _map_pointer _nfinish = _nstart + _num_nodes - 1;
            _map_pointer _cur;

            //为map中的节点申请缓冲区
            try{
                //注意：_nfinish不是超尾
                for(_cur = _nstart;_cur <= _nfinish; ++_cur){
                    *_cur = _allocate_node();
                }
            }catch(...){
                //失败，回滚,即释放申请到的缓冲区(node)
                for(_cur = _nstart;_cur <= _nfinish; ++_cur){
                    _deallocate_node(*_cur);
                }
            }

            //申请空间完毕，更新deque的_start和_finish迭代器
            _start._set_node(_nstart);
            _finish._set_node(_nfinish);
            _start._cur  = _start._first;
            _finish._cur = _finish._first + _num_elements % _buffer_size();

        }


        //负责初始化deque
        //注意：该函数会更新deque的_start和_finish迭代器
        //以及_map指针和_map_size
        void _fill_initialize(_size_type n,const _value_type& _value){
            //申请map和缓冲区的空间
            _create_map_and_node(n);

            _map_pointer _cur;
            
            //为每个map节点的缓冲区设置初始值
            try{
                for(_cur = _start._node;_cur < _finish._node;++_cur){
                    uninitialized_fill(*_cur,*_cur + _buffer_size(),_value);
                }
                //最后一个缓冲区单独处理，因为可能有不初始化的空间
                uninitialized_fill(_finish._first,_finish._cur,_value);

            }catch(...){
                //失败就回滚,即析构元素,并释放空间
                for(_cur = _start._node;_cur <= _finish._node;++_cur){
                    destroy(*_cur,*_cur + n);
                    _deallocate_node(*_cur);
                }
            }
        }

        //_is_add_at_front为true(false)时，代表是由于原map前端(后端)空间不足导致的reallocate map
        void _reallocate_map(_size_type _nodes_to_add,bool _is_add_at_front){

            _size_type _old_num_nodes = _finish._node  - _start._node + 1;
            _size_type _new_num_nodes = _old_num_nodes + _nodes_to_add;

            _map_pointer _new_start;

            if(_map_size > 2 * _new_num_nodes){
                //map足够大，只是前后端扩张不均匀导致的空间不足时,只需要调整原map即可
                _new_start = _map + (_map_size -  _new_num_nodes)/2 
                             + (_is_add_at_front ? _nodes_to_add : 0);

                
                if(_new_start < _start._node){
                    //新起点在原起点右边时,从前往后依次复制
                    //注意_finish._node并不是超尾，此处是因为copy算法要求超尾(即end())
                    //故需要 + 1
                    my_copy(_start._node, _finish._node + 1,_new_start);
                }else{
                    //新起点在原起点左边时,从后向前依次复制
                    //+1的原因同上面的my_copy()
                    my_copy_backward(_start._node,_finish._node + 1,_new_start + _old_num_nodes);
                }
            }else{
                //map不够大，重新申请空间，并将原map复制过去

                //新map的大小，直接扩充到2倍+2大小
                _size_type _new_map_size = _map_size + std::max(_map_size,_nodes_to_add) + 2;

                //申请新map的空间
                _map_pointer _new_map = deque_map_allocator::allocate(_new_map_size);
                
                //定位_new_start
                _new_start = _new_map + (_new_map_size - _new_num_nodes)/2
                            + (_is_add_at_front ? _nodes_to_add : 0);
                
                my_copy(_start._node,_finish._node + 1,_new_start);
    
                deque_map_allocator::deallocate(_map,_map_size);

                //设置新map的起始地址与大小
                _map      = _new_map;
                _map_size = _new_map_size;
            }

            //更改deque的_start和_finish迭代器
            //注意，因为只是移动map，所以迭代器中的_cur指针并不受影响
            //其实迭代器中的_first和_last指针也不瘦影响，此处_set_node()
            //看似更改了_first和_last，实际上改完还是原样
            _start._set_node(_new_start);
            _finish._set_node(_new_start + _old_num_nodes + 1);
        }

        //当map前端的空间不够用时，调用此函数
        void _reserve_map_at_front(_size_type _nodes_to_add = 1){
            
            if(_nodes_to_add > (_difference_type)(_start._node - _map)){
                _reallocate_map(_nodes_to_add,true);
            }
        }

        //当map尾端的空间不够用时，调用此函数
        //_nodes_to_add表示需要添加的node个数，即缓冲区个数
        void _reserve_map_at_back(_size_type _nodes_to_add = 1){
            //+1是因为？
            if(_nodes_to_add + 1 > _map_size - (_finish._node - _map)){
                _reallocate_map(_nodes_to_add,false);
            }
        }

        //迭代器
        _iterator                begin()    const     {return _start;}
        _iterator                end()      const     {return _finish;}
        _const_iterator          cbegin()   const     {return _start;}
        _const_iterator          cend()     const     {return _finish;}
        _reverse_iterator        rbegin()   noexcept  {return reverse_iterator(this->begin());}
        _reverse_iterator        rend()     noexcept  {return reverse_iterator(this->end());}
        _const_reverse_iterator  crbegin()  noexcept  {return reverse_iterator(this->cbegin());}
        _const_reverse_iterator  crend()    noexcept  {return reverse_iterator(this->cend());}

        

        //构造、析构函数

        //默认构造函数

        deque():_start(),_finish(),_map(nullptr),_map_size(0){_fill_initialize(0,T());}
        
        deque(int n, const _value_type& _value):_start(),_finish(),_map(nullptr),_map_size(0){
            _fill_initialize(n,_value);
        }

        //复制构造函数
        deque(const _self& _other):_start(),_finish(),_map(nullptr),_map_size(0){
            //先申请并初始化和_other一样大的空间
            _fill_initialize(_other.size(),_value_type());
            //再将_other的元素依次copy过来
            my_copy(_other.begin(),_other.end(),this->begin());
        }

        //移动构造函数
        deque(const _self&& _other):_start(_other._start),_finish(_other._finish),
                                    _map(_other._map),_map_size(_other._map_size){
            _other._start._cur  = _other._start._first  = _other._start._last  = nullptr;
            _other._finish._cur = _other._finish._first = _other._finish._last = nullptr;
            _other._map = nullptr;
            _other._map_size = 0;
        }

        //析构函数
        ~deque(){
            //清除所有数据
            clear();
            //由于clear()会留下一个缓冲区，故需要释放该缓冲区
            deque_data_allocator::deallocate(_start._first,_buffer_size());
            //释放map
            deque_map_allocator::deallocate(_map);
        }

        
        //功能函数
        _size_type size()const {return _finish - _start;}
        bool empty()const{return _finish == _start;}
        void resize(){}
        void shrink_to_fit(){}
        
        //元素访问
        _reference front()const{return *_start;}
        _reference back()const{
            _iterator _tmp = _finish;
            --_tmp;
            return *_tmp;
        }

        //有下界检查
        _reference at(_index_type _index)const{
            if(_index >= 0 && _index < size()){
                _iterator _tmp = _start;
                _tmp += _index;
                return *_tmp;
            }else{
                throw("invalid range!");
            }
        }

        //无下界检查
        _reference operator[](_difference_type _index){
            return  _start[_index];
        }

        //复制运算符
        _self& operator=(const _self& _other){
            if(_other == *this) return *this;

            //方法是：先将原deque销毁，再重新申请空间和初始化
            clear();
            //由于clear()会留下一个缓冲区，故需要释放该缓冲区
            deque_data_allocator::deallocate(_start._first,_buffer_size());
            //释放map
            deque_map_allocator::deallocate(_map);

            //先申请并初始化和_other一样大的空间
            _fill_initialize(_other.size(),_value_type());
            //再将_other的元素依次copy过来
            my_copy(_other.begin(),_other.end(),begin());
        } 

        //元素插入
        _iterator insert_aux(_iterator _pos,const _value_type& _value){
            _value_type _value_copy = _value;
            //插入点之前的元素个数
            _difference_type _index = _pos - _start;

            if(_index < size()/2){
                //插入点之前的元素个数较少
                //在最前端插入与第一个元素相等的元素
                push_front(front());

                _iterator _front1 = _start;
                ++_front1;
                _iterator _front2 = _front1;
                ++_front2;
                //为什么要重定位_pos? 因为push操作可能
                //导致map重申请空间导致_pos迭代器失效
                _pos = _start + _index;
                _iterator _pos1 = _pos;
                ++_pos1;

                my_copy(_front2,_pos1,_front1);
            }else{
                //插入点之后的元素个数较少
                //在最尾端插入与最后一个元素相等的元素
                push_back(back());

                _iterator _back1 = _finish;
                --_back1;
                _iterator _back2 = _back1;
                --_back2;

                //重定位_pos原因如上
                _pos = _start + _index;
                
                my_copy_backward(_pos,_back2,_back1);
            }

            *_pos = _value_copy;
            return _pos;
        }

        //在_pos之前插入_value构建的节点
        _iterator insert(_iterator _pos,const _value_type& _value){
            if(_pos._cur == _finish._cur){
                push_back(_value);
                _iterator _tmp = _finish;
                --_tmp;
                return _tmp;
            }else if(_pos._cur == _start._cur){
                push_front(_value);
                return _start;
            }else{
                return insert_aux(_pos,_value);
            }
        }

        _iterator insert(_iterator _pos, _size_type _n,const _value_type& _value){
                _difference_type _index = _pos - _start;
                _iterator _ret = _pos;

                //偷懒了
                if(_index < size()/2){
                    //插入点之前的元素个数较少
                    for(auto i = _n ; i > 0; --i) {
                        push_front(_value);
                    }
                }else{
                    //插入点之后的元素个数较少
                    for(auto i = _n ; i > 0; --i) {
                        push_back(_value);
                    }
                }

            return _ret;
        }

        //待修改
        _iterator emplace(_iterator _pos,const _value_type& _value){
            return insert(_pos,_value);
        }

        //待修改
        _iterator emplace_back(const _value_type& _value){
            return insert(end(),_value);
        }

        //待修改
        _iterator emplace_front(const _value_type& _value){
            return insert(begin(),_value);
        }

        void push_back_aux(const _value_type& _value){
            _value_type _value_copy = _value;
            //检查是否有必要扩充map
            _reserve_map_at_back();

            //申请新的缓冲区
            *(_finish._node + 1) = _allocate_node();

            try{
                construct(_finish._cur,_value_copy);
                //更新_finish的_node和_cur
                _finish._set_node(_finish._node + 1);
                _finish._cur = _finish._first;

            }catch(...){
                //失败，回滚，即释放申请到的空间
                _finish._set_node(_finish._node - 1);
                _finish._cur = _finish._last;
                _deallocate_node(*(_finish._node + 1));
                throw;
            }

        }

        void push_back(const _value_type& _value){
            if(_finish._cur != _finish._last - 1){
                //当缓冲区还有两个或两个以上的空位时
                //直接在上面构建元素
                construct(_finish._cur,_value);
                ++_finish._cur;

            }else{
                //缓冲区只有一个空位时
                push_back_aux(_value);
            }
        }
        
        void push_front_aux(const _value_type& _value){
            _value_type _value_copy = _value;
            //检查是否有必要扩充map
            _reserve_map_at_front();
            //申请新的缓冲区
            *(_start._node - 1) = _allocate_node();

            try{
                //更新_start的_node和_cur
                _start._set_node(_start._node - 1);
                _start._cur = _start._last - 1;
                construct(_start._cur,_value_copy);
                
            }catch(...){
                //失败，回滚，即释放申请到的空间
                _start._set_node(_start._node + 1);
                _start._cur = _start._first;
                _deallocate_node(*(_start._node - 1));
                throw;
            }

        }

        void push_front(const _value_type& _value){
            if(_start._cur != _start._first ){
                construct(_start._cur - 1,_value);
                --_start._cur;
            }else{
                //缓冲区无空位时
                push_front_aux(_value);
            }
        }
           
        //元素删除

        //只有当_start._cur == _start._last 时才会被调用
        void _pop_front_aux(){
            destroy(_start._cur);
            _deallocate_node(_start._first);
            _start._set_node(_start._node + 1);
            _start._cur = _start._first;
        }

        void pop_front(){
            if(_start._cur != _start._last -1 ){
                //如果最前面的缓冲区还有两个(或更多)元素
                destroy(_start._cur);
                ++_start._cur;
            }else{
                //最前面的缓冲区只有一个元素
                _pop_front_aux();
            }
        }

        //只有当_finish._cur == _finish._first时才会被调用
        void _pop_back_aux(){
            destroy(_finish._cur);
            _deallocate_node(_finish._first);
            _finish._set_node(_finish._node - 1);
            _finish._cur = _finish._last - 1;
        }

        void pop_back(){
            if(_finish._cur != _finish._first){
                //如果最后一个缓冲区有有元素
                --_finish._cur;
                destroy(_finish._cur);
            }else{
                //最后面的缓冲区没有元素
                _pop_back_aux();
            }
        }

        //返回被删除目标的下一个位置的迭代器
        _iterator erase(_iterator _pos){
            _iterator _ret = _pos;
            ++_ret;
            _difference_type _index = _pos - _start;

            if(_index < size()/2){
                //pos之前的元素比较少   
                //移动前段元素
                my_copy_backward(_start,_pos,_ret);
                pop_front();

            }else{
                //pos之后s的元素比较少
                //移动后段元素
                my_copy(_ret,_finish,_pos);
                pop_back();
            }
            return _ret;
        }

        //返回清除区间的下一个位置的迭代器
        _iterator erase(_iterator _pos1,_iterator _pos2){
            //如果是清除整个区间
            if(_pos1 == _start && _pos2 == _finish){
                clear();
                return _finish;
            }else{
                //清除区间的长度
                _difference_type _len = _pos2 - _pos1;
                //清除区间前方的元素个数
                _difference_type _elems_before = _pos1 - _start;

                if(_elems_before < (size() - _len)/2){
                    //前方区间元素较少
                    //向后移动前方区间元素
                    my_copy_backward(_start, _pos1,_pos2);

                    _iterator _new_start = _start + _len;
                    destroy(_start,_new_start);
                    
                    //释放缓冲区
                    for(_map_pointer _cur = _start._node; _cur < _new_start._node; ++_cur){
                        deque_data_allocator::deallocate(*_cur);
                    }
                    _start =  _new_start;
                }else{
                    //后方区间元素较少
                    //向前移动后方元素
                    my_copy(_pos2,_finish,_pos1);

                    _iterator _new_finish = _finish - _len;
                    destroy(_new_finish,_finish);

                    //释放缓冲区
                    for(_map_pointer _cur = _new_finish._node; _cur < _finish._node;++_cur){
                        deque_data_allocator::deallocate(*_cur);
                    }
                    _finish = _new_finish;
                }
                return _start + _elems_before;
            }
        }

        //clear会保留一个缓冲区，以防止下次使用时再申请空间
        void clear(){
            //清除除头尾以外的缓冲区
            for(_map_pointer _cur = _start._node + 1; _cur < _finish._node;++_cur){
                destroy(*_cur , *_cur + _buffer_size());
                deque_data_allocator::deallocate(*_cur,_buffer_size());
            }

            if(_start._node != _finish._node){
                //头尾至少有两个缓冲区
                //先析构头尾缓冲区的元素
                destroy(_start._cur,_start._last);
                destroy(_finish._first,_finish._cur);
                
                //保留头缓冲区，释放尾缓冲区
                deque_data_allocator::deallocate(_finish._first,_buffer_size());

            }else{
                //只剩下一个缓冲区
                //只析构缓冲区元素(注意，此时剩下的一个缓冲区肯定是头缓冲区)
                destroy(_start._cur,_start._last);
            }
            _finish = _start;
        }

        //其他
        void swap(){

        }
        void assign(){

        }
        
};

#endif // __MY_STL_DEQUE_H__