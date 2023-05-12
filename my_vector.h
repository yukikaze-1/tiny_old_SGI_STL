#ifndef __MY_VECTOR_H__
#define __MY_VECTOR_H__ 1

#include "my_memory.h"
#include "my_stl_algorithm.h"

template<class T,class _Alloc = alloc>
class vector{
    public:
        using _value_type                   = T;
        using _pointer                      = T*;
        using _const_pointer                = const T*;
        using _iterator                     = T*;
        using _const_iterator               = const T*;
        using _reference                    = T&;
        using _const_reference              = const T&;
        using _size_type                    = size_t;
        using _difference_type              = ptrdiff_t;
        using _reverse_iterator             = reverse_iterator<_iterator>;
        using _const_reverse_iterator       = reverse_iterator<_const_iterator>;

    protected:
        //vector的空间配置器
        using vector_allocator = simple_alloc<_value_type,_Alloc>;

        //使用空间的三迭代器（指针）
        _iterator _start;
        _iterator _end;
        _iterator _end_of_storage;


        //释放空间
        void vector_deallocate(){
            if(_start != nullptr){
                vector_allocator::deallocate(_start,_end_of_storage - _start);
            }
        }
        
        
        //insert调用的函数，返回指向插入后的元素的迭代器
        _iterator _insert_aux(_iterator _position,const T& _value){
            _iterator _ret = _start;
            auto _count =  distance(_start,_position);

            //还有冗余空间
            if(_end != _end_of_storage){
                //vector末尾构建末尾的元素
                construct(_end,back());
                //移动_end
                ++_end;
                //
                T _copy = _value;
                //从后往前依次移动元素，往后移动一个位置
                my_copy_backward(_position,_end-2,_end-1);
                
                //复制目标元素到目标位置
                *_position = _copy;
                //移动迭代器至目标位置，用于返回迭代器
                advance(_ret,_count);
            }else{
            //无冗余空间，需要reallocate
                const _size_type _old_size = size();
                const _size_type _len = _old_size != 0?2 * _old_size:1;

                _iterator _new_start = vector_allocator::allocate(_len);
                _iterator _new_end = _new_start;
                _iterator _ret = _new_end;

                try{
                    //移动原vector至新vector(第一部分，以position为界限)
                    _new_end =  uninitialized_copy(_start,_position,_new_start); 
                    //_position = _new_start;
                    //插入新元素
                    construct(_new_end,_value);
                    //更新新_end
                    ++_new_end;
                    //移动原vector至新vector(第二部分，以position为界限)
                    _new_end = uninitialized_copy(_position,_end,_new_end);
                }
                //不成功，便全部回滚
                catch(...){
                    //先析构已经copy的元素
                    destroy(_new_start,_new_end);
                    //再deallocate空间
                    vector_deallocate();
                    throw;
                }

                //析构原vector
                destroy(_start,_end);
                //deallocate原vector空间
                vector_deallocate();
                //更新vector三指针
                _start = _new_start;
                _end = _new_end;
                _end_of_storage = _start + _len;

                //更新返回值
                _ret = _start;
                advance(_ret,_count);
            }
            return _ret;
        }

       
        //申请内存并更新vector的三指针
        //调用_my_allocate_and_fill()，其真正进行内存的申请任务
        void _fill_and_initialize(_size_type _n,const _value_type& _value){
            _start = _allocate_and_fill(_n,_value);
            _end  = _start + _n;
            _end_of_storage = _end;
        }
        //调用配置器的allocate函数
        _iterator _allocate_and_fill(_size_type _n,const _value_type& _value){
            //_iterator _ret = _my_vector_allocator::my_allocate(_n);
            _iterator _ret = vector_allocator::allocate(_n);
            uninitialized_fill_n(_ret,_n, _value);
            return _ret;
        }
    public:
        /*迭代器*/
        _iterator begin()const { return _start; }
        _iterator end()const { return _end; }
        _const_iterator cbegin()const { return _start; }
        _const_iterator cend()const { return _end;}  

        // 调用reverse_iterator的构造函数，返回一个指向end()的反向迭代器
        _reverse_iterator rbegin() noexcept {return reverse_iterator(this->end()); }
        _reverse_iterator rend() noexcept {return reverse_iterator(this->begin()); }
        _const_reverse_iterator crbegin() noexcept {return reverse_iterator(this->end()); }
        _const_reverse_iterator crend() noexcept {return reverse_iterator(this->begin()); }
                

        /*容量相关*/

        //返回vector中实际元素个数
        _size_type size()const { return _size_type(end() - begin()); }

        //和系统实现有关,此处不实现
        //_size_type max_size()const { return _size_type(???); }

        _size_type capacity()const {return _size_type(_end_of_storage - begin());}
        bool empty()const {return size() == 0; }

        void shrink_to_fit(){
            vector_allocator::deallocate(_end,_end_of_storage - _end);
            _end_of_storage = _end;
        }

        //该函数会实际改变vector，有可能reallocate vector
        //如果_n小于当前的实际元素个数，则该vector截断前_n个元素
        //如果_n大于当前的实际元素个数，则该vector会扩充至该大小，
        //并将原vector的最后一个元素复制到扩充后的区段，如果有指定元素，
        //则将指定元素复制到扩充后的区段。注意，该函数可能会导致reallocate
        void resize(_size_type _new_size){
            resize(_new_size,T());
        }

        void resize(_size_type _new_size, const T& _value){
            if(_new_size < capacity()){
                erase(begin() + _new_size,end());
            }else{
                //注意，此处若插入时vector已满，会导致reallocate
                insert(end(),_new_size - size(),_value);
            }
        }

        //该函数只影响vector的capacity，即_end_of_storage的位置
        //可能导致reallocate
        void reserve(_size_type _n){
            //只有在新空间大小超过原空间大小时，才进行
            if(_n > capacity()){
                //reallocate vector
                _iterator _new_start = vector_allocator::allocate(_n);
               //复制原vector的元素至新vector中 
                _iterator _new_end = uninitialized_copy(begin(),end(),_new_start);
                
                //destroy原vector
                destroy(_start,_end); 
                //deallocate原vector
                vector_deallocate();

                //更新三指针
                _start = _new_start;
                _end = _new_end;
                _end_of_storage = _start + _n;
            }
        }

        /*构造函数与析构函数*/

        vector():_start(nullptr),_end(nullptr),_end_of_storage(nullptr){};
        vector(_size_type _size,const T& _value){_fill_and_initialize(_size,_value);}
        explicit vector(_size_type _size){_fill_and_initialize(_size,T());}
        ~vector(){destroy(_start,_end);vector_deallocate();}
        
        //复制构造函数
        vector(const vector<T,_Alloc>& _other){
            //先计算申请的空间的大小
            _size_type _total =  _other.size();
            //初始化空间为T类型的默认构造，并更新三指针，
            _fill_and_initialize(_total,T());
            //复制vector
            my_copy(_other.begin(),_other.end(),this->begin());
        }
        //移动构造函数 (右值引用)的复制构造函数 移动复制构造函数
        vector(vector<T,_Alloc>&& _other)noexcept : _start(_other._start),
            _end(_other._end),_end_of_storage(_other._end_of_storage){
            _other._start = _other._end = _other._end_of_storage = nullptr;
        }


        //运算符重载

        //移动赋值运算符(右值引用)的赋值构造函数
        vector<T,_Alloc>& operator=(vector<T,_Alloc>&& _other)noexcept{
            if(_other != *this){
                //释放已有元素
                destroy(_start,_end);
                vector_deallocate();
                //从_other接管元素
                _start = _other._start;
                _end = _other._end;
                _end_of_storage = _other._end_of_storage;
                //将_other置于可析构状态
                _other._start = _other._end=_other._end_of_storage = nullptr;
            }
            return *this;
        }

        //赋值构造函数
        vector<T,_Alloc>& operator=(const vector<T,_Alloc>& _other){
            if(_other == *this){
                return *this;
            }else{
                //如过赋值的vector小于等于原vector,直接调用copy
                if(this->size() >= _other.size()){
                    _iterator _r =  my_copy(_other.begin(),_other.end(),this->begin());
                    erase(_r,this->end());
                }else{
                    //先销毁原vector并deallocate其空间
                    destroy(_start,_end);
                    vector_deallocate();

                    //申请新空间并更新三指针
                    _iterator _ret = vector_allocator::allocate(_other.size() * sizeof(_value_type));
                    _iterator _tmp =  uninitialized_copy(_other.begin(),_other.end(),_ret);
                    _start = _ret;
                    _end  = _tmp;
                    _end_of_storage = _end;
                }
                
            }
            return *this;
        }
        //operator=也可以这么写（若果定义了自己的swap成员函数）
        /*因为此处传入的是一个副本，swap的是临时对象和当前对象，
        *交换后临时对象会自动析构
        *vector& operator=(vector _other){
        *    swap(*this,_other);
        *    return *this;  
        *}
        */


        vector<T,_Alloc>& operator+(const vector<T,_Alloc>& _other){
                //超出剩余空间大小，需要reallocate
            if(_other.size() > _end_of_storage - _end){
                _size_type _total_size = (_end - _start + _other._end - _other._start) * sizeof(_value_type);
                reserve(_total_size);
                _end =  my_copy(_other.begin(),_other.end(),_end);
                 
            }else{
                //剩余空间还足够,直接copy
                _end =  my_copy(_other.begin(),_other.end(),end());
            } 
            return *this;
        }

        bool operator!=(const vector<T,_Alloc>& _other)const{return !(*this == _other);}


        bool operator==(const vector<T,_Alloc>& _other)const{
            if(this->size() != _other.size())
                return false;
            else{
                for(size_t i = 0;i<this->size();++i){
                    if((*this)[i] != _other[i])
                        return false;
                }
            }    
            return true;
        }
        _reference operator[](_size_type _index) const{return *(begin() + _index);}


        /*元素访问相关*/

        _reference at(_size_type _index) const{ return *(begin() + _index);}
        _reference front()const{return (_reference)(*_start);}
        _reference back()const{return (_reference)(*(_end -1 ));}
        _value_type* data()const{return _end_of_storage;}

        /*修改容器*/

        _iterator erase(_iterator _first, _iterator _last){
            //将后面的元素往前复制
            _iterator _ret =  my_copy(_last,end(),_first);
            //destroy多余的元素
            destroy(_ret,_end);
            _end = _ret;
            return _ret;
        }
        _iterator erase(_iterator _position){
            if(_position + 1 != end()){
                my_copy(_position + 1, _end, _position);
            }
            --_end;
            destroy(_end);
            return _position;
        }
        void push_back(const T& _value){
                //vector还没满
            if(_end != _end_of_storage){
                construct(_end,_value);
                ++_end;
            }else{
                //vector已满
                _insert_aux(end(), _value);
            }

        }
        _iterator emplace(_iterator _position,const T& _value){
            if( (size_t)(_position - begin()) == size()){
                emplace_back(_value);
                return begin();
            }else{
                return insert(_position,_value);
            }
            
        }
        void emplace_back(const T& _value){
            return push_back(_value);
        }
        _iterator insert(const _iterator _position,const T& _value){
            return _insert_aux(_position,_value);
        }

        //返回指向插入的前的第一个元素的迭代器
        _iterator insert(const _iterator _position,_size_type _n,const T& _value){
            //超出剩余空间大小，需要reallocate
            _size_type _total_size = _n * sizeof(_value);
            if(_total_size > capacity()){
                reserve(_total_size);
            }
            //先移动元素，腾出位置
            my_copy(_position,_position + _n,_position + _n);
            //调用uninitialized_fill_n
            uninitialized_fill_n(_position,_n,_value);
            return _position;
        }
        //调用erase，erase负责析构元素和deallocate空间
        void clear(){erase(begin(),end());};

        //不deallocate空间
        void pop_back(){--_end;destroy(_end);};

        //成员函数，交换两个vector,只交换vector的三指针，其余的不交换
        //交换后迭代器不失效
        void swap(vector& _other){
            _iterator _tmp = _other._start;
            _other._start = this->_start;
            this->_start = _tmp;

            _tmp = _other._end;
            _other._end = this->_end;
            this->_end = _tmp;

            _tmp = _other._end_of_storage;
            _other._end_of_storage = this->_end_of_storage;
            this->_end_of_storage = _tmp;
        }

        
        /*配置器相关*/
        //返回该vector所使用的配置器
        //_Alloc get_allocator()const {return alloc();}

        
};//end class

#endif //   __MY_VECTOR_H__