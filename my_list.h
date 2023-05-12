#ifndef _MY_STL_LIST_H__
#define _MY_STL_LIST_H__ 1

#include "my_memory.h"
#include "my_stl_algorithm.h"

//list节点定义
template <typename T>
struct ListNode{
    //using _ListNode_pointer  = void *;
    using _ListNode_pointer = ListNode<T>*;

    _ListNode_pointer prev ;
    _ListNode_pointer next ;
    T data;
};

//list迭代器定义
template <typename T, typename Ref = T&, typename Ptr = T*>
class list_iterator{
    public:
        using _iterator               = list_iterator<T, T&,       T*>;
        using _const_iterator         = list_iterator<T, const T&, const T*>;
        using _self                   = list_iterator<T, Ref,      Ptr>;

        using _iterator_category = Bidirectional_iterator_tag;
        using _value_type        = T;
        using _difference_type   = ptrdiff_t;
        using _pointer           = Ptr;
        using _reference         = Ref;
        using _size_type         = size_t;

        using _link_type         = ListNode<T>*;
    
    //list迭代器内部指针
        _link_type _node;

    //构造函数
        list_iterator(const list_iterator& _other):_node(_other._node){}
        list_iterator(_link_type _link):_node(_link){}
        list_iterator()=default;

    //功能函数
        bool operator==(const _self& _other){return _node == _other._node;}
        bool operator!=(const _self& _other){return _node != _other._node;}

        _reference operator*()const{return _node->data;}
        _pointer   operator->()const{return &(operator*());}
        _self& operator=(const _self& _other){
            this->_node = _other._node;
            return *this;
        }

        _self& operator++(){
            //_node = (*_node).next;
            _node = (*_node).next;
            return *this;
        }

        _self operator++(int){
            _self _tmp = *this;
            ++(*this);
            return _tmp;
        }

        _self& operator--(){
            //_node = (*_node).prev;
            _node = _node->prev;
            return *this;
        }

        _self operator--(int){
            _self _tmp = *this;
            --(*this);
            return _tmp;
        }

};


//list定义
template <typename T, class _Alloc = alloc>
class list{
    public:
        using _value_type             = T;
        using _node_type              = ListNode<T>;
        using _pointer                = T*;
        using _const_pointer          = const T*;
        using _iterator               = list_iterator<T>;
        using _const_iterator         = const list_iterator<T>;
        using _reference              = T&;
        using _const_reference        = const T&;
        using _size_type              = size_t;
        using _difference_type        = ptrdiff_t;
        using _reverse_iterator       = reverse_iterator<_iterator>;
        using _const_reverse_iterator = reverse_iterator<_const_iterator>;

    protected:
        //链表节点类型
        using list_node = ListNode<T>;

        //空间配置器
        using list_node_allocator = simple_alloc<_node_type,_Alloc>;

    public:
        //链表节点之间的指针
        using _link_type = list_node*;

    protected:
        //只需要一个指针，指向该环形双链表即可
        _link_type _list;

        /*内存相关*/

        //申请一个新节点的空间
        _link_type _get_node(){
            return list_node_allocator::allocate(1);
        }

        //释放一个节点的空间给allocator的freelist
        void _perish_node(_link_type _node){
            list_node_allocator::deallocate(_node);
        }

        //创建一个新节点
        _link_type _create_node(const T& _value){
            _link_type _ret = _get_node();
            construct(&(_ret->data), _value);
            return _ret;
        }

        //销毁一个节点
        void _destroy_node(_link_type _node){
            destroy(&(_node->data));
            _perish_node(_node);
        }


    public:
        /*迭代器*/
        
        _iterator begin() const { return _list->next; }
        _iterator end() const{ return _list; }
        _const_iterator cbegin()const { return _list->next;}
        _const_iterator cend()const { return _list; }

        _reverse_iterator rbegin()noexcept{return reverse_iterator(this->begin());}
        _reverse_iterator rend()noexcept{return reverse_iterator(this->end());}
        _const_reverse_iterator crbegin()noexcept{return reverse_iterator(this->begin());}
        _const_reverse_iterator crend()noexcept{return reverse_iterator(this->end());}

        /*构造函数*/

        //用于产生一个新的空的list
        void _empty_list(){
            _list = _get_node();
            _list->next = _list;
            _list->prev = _list;
        }

        //默认构造函数
        list(){_empty_list();}

        //复制构造函数
        list(const list& _other){
            //申请list的头结点并初始化其指向自己
            _list = _get_node();
            _list->next = _list;
            _list->prev = _list;

            _iterator _first = _other.begin();
            _iterator _last =  _other.end();
            _iterator _p(_list);

            //将_other中的元素依次插入到头结点之后，尾插法
            while(_first != _last){
                _insert(_p, _first._node->data);
                ++_p;
                ++_first;
            }

        }
        //转换构造函数
        list(const _iterator& _other_node){
            _list = _get_node();
            _iterator _tmp = _create_node(_other_node._node->data);
            _list->next = _tmp;
            _list->prev = _tmp;
            _tmp._node->prev = _list;
            _tmp._node->next = _list;
        }

        //转换构造函数
        explicit list(const T& _value){
            //申请list的头结点并初始化其指向自己
            _list = _get_node();
            _list->next = _list;
            _list->prev = _list;

            _iterator _p(_list);
            _insert(_p, _value);
        }

        //移动构造函数
        list(list&& _other){
            _list = _other._list;
            _other._list = nullptr;
        }

        //析构函数
        ~list(){
            if(_list != nullptr){
                _link_type _tmp = _list->next;
                _link_type _p;
                while(_tmp != _list){
                    _p = _tmp->next;
                    _destroy_node(_tmp);
                    _tmp = _p;
                }
                _destroy_node(_list);
            }
        }

        /*功能函数*/

        bool empty()const {return _list->next == _list;}
        _size_type size()const {return distance(begin(),end());}

        /*元素相关*/

        _reference front()const{return *begin();}
        _reference back()const{return *end();}

        /*运算符重载*/

        //赋值运算符
        list& operator=(const list& _other){
            //申请list的头结点并初始化其指向自己
            _list = _get_node();
            _list->next = _list;
            _list->prev = _list;

            _iterator _first = _other.begin();
            _iterator _last = _other.end();
            _iterator _p(_list);

            while(_first != _last){
                _insert(_p, _first._node->data);
                ++_p;
                ++_first;
            }
            return *this;
        }

        //移动赋值运算符
        list& operator=(list&& _other){
            _list = _other._list;
            _other._list = nullptr;
            return *this;
        }

        /*元素修改*/

        //尾插法,内部使用,构建新节点并插入,返回新插入的节点的迭代器
        _iterator _insert(_iterator _pos,const T& _value){
            _iterator _tmp;
            _tmp._node = _create_node(_value);
            _tmp._node->next = _pos._node->next;
            _tmp._node->prev = _pos._node;
            _pos._node->next->prev = _tmp._node;
            _pos._node->next = _tmp._node;           
            return _tmp;
        }
        
        //头插法
        //在_pos所指位置前插入以_value为值所构建的链表节点_node,返回新插入的节点的迭代器
        _iterator insert(_iterator _pos,const T& _value){
            _iterator _tmp;
            _tmp._node = _create_node(_value);
            _tmp._node->next = _pos._node;
            _tmp._node->prev = _pos._node->prev;
            _pos._node->prev->next = _tmp._node;
            _pos._node->prev = _tmp._node;
            return _tmp;
        }
        //在_pos所指位置前插入链表节点_x
       _iterator insert(_iterator _pos,list_node& _x){

            _iterator _tmp;
            _tmp._node = &_x;
            _tmp->next = _pos._node;
            _tmp->prev = _pos._node->prev;
            _pos._node->prev->next = _tmp;
            _pos._node->prev = _tmp;
            return _tmp;
        }

        //在_pos所指位置前插入以_value为值所构建的链表节点_node _size个
        _iterator insert(_iterator _pos,_size_type _size,const T& _value){

        }


        //在_pos所指位置前插入链表节点_node
        _iterator emplace(_iterator _pos,list_node _node){return insert(_pos,_node);}
        _iterator emplace(_iterator _pos,list_node&& _node){return insert(_pos,_node);}
        _iterator emplace(_iterator _pos,const T& _value){return insert(_pos,_value);}

        _iterator emplace_front(list_node _node){return insert(begin(),_node);}
        _iterator emplace_front(const T& _value){return insert(begin(),_value);}
        _iterator emplace_back(list_node _node){return insert(end(),_node);}
        _iterator emplace_back(const T& _value){return insert(end(),_value);}

        _iterator push_back(const T& _value){return insert(end(),_value);}
        _iterator push_back(list_node _node){return insert(end(),_node);}
        _iterator push_back(list_node&& _node){return insert(end(),_node);}

        _iterator erase(_iterator _pos){
            if(_list->next == _list)
                return _list;
            _iterator _ret = _pos._node->next;
            _pos._node->prev->next = _pos._node->next;
            _pos._node->next->prev = _pos._node->prev;
            _destroy_node(_pos._node);

            return _ret;
        }
        _iterator erase(_iterator _begin,_iterator _end){
            _begin._node->prev->next = _end._node;
            _end._node->prev = _begin._node->prev;

            _iterator _p(_begin._node->next);
            while(_begin != _end){
                erase(_begin);
                _begin = _p++;
            }
            return _end;
        }
        _iterator pop_front(){return erase(begin());}
        _iterator pop_back(){return erase(--end());}

        void clear(){
            erase(begin(),end());
            _list->next = _list->prev = _list;
        }

        void resize(){

        }

        /*内部实现算法*/

        //[_begin,_end)中的元素copy到_des之后，并返回最后一个插入元素之后的位置的迭代器
        _iterator _range_copy_after(_iterator _des,_iterator _begin,_iterator _end){
            _iterator _tmp;
            _iterator _next = _des;
            ++_next;
            while(_begin != _end){
                _tmp = _create_node(_begin._node->data);
                _des._node->next = _tmp._node;
                _tmp._node->prev = _des._node;
                ++_des;
                ++_begin;
            }
            //因为是环形双链表，所以要处理最后一个节点
            _tmp._node->next = _next._node;
            _next._node->prev = _tmp._node;
            return _des;
        }
        //[_begin,_end)中的元素copy到_des之前
        _iterator _range_copy_before(_iterator _des,_iterator _begin,_iterator _end){
            _iterator _prev = _des;
            --_prev;
            _iterator _tmp;
            while(_begin != _end){
                _tmp = _create_node(_begin._node->data);
                _prev._node->next = _tmp._node;
                _tmp._node->prev = _prev._node;
                ++_prev;
                ++_begin;
            }
            //因为是环形双链表，所以要处理最后一个节点
            _tmp._node->next = _des._node;
            _des._node->prev = _tmp._node;
            return _des;
        }

        //将[_begin,_end)中的元素移动到_pos的前面（前插法）
        //_begin,_end可能是另一list中的迭代器，注意！
        void _transfer(_iterator _pos,_iterator _begin,_iterator _end){
            if(_pos != _end){
                _end._node->prev->next = _pos._node;
                _begin._node->prev->next = _end._node;
                _pos._node->prev->next = _begin._node;

                _link_type _tmp = _pos._node->prev;
                _pos._node->prev = _end._node->prev;
                _end._node->prev = _begin._node->prev;
                _begin._node->prev = _tmp;
            }
        }


        /*对外算法*/

        //移除所有与_value相等的链表节点
        void remove(const T& _value){
            _iterator _first = begin();
            _iterator _last = end();
            while(_first != _last){
                _iterator _next = _first._node->next;
                if(_first._node->data == _value){
                    erase(_first);
                }
                _first = _next;
            }
        }
        
        //移除所有使_op为true的链表节点
        void remove_if(_iterator _begin,_iterator _end){

        }

        //list自己的unique
        void unique(){
            _iterator _first = begin();
            _iterator _last = end();
            //空链表
            if(_first == _last)
                return;
            while(_first != _last){
                _iterator _next = _first._node->next;
                if(_first._node->data == _next._node->data){
                    erase(_next);
                    _next = _first;
                }else{
                    _first = _next;
                }
            }
        }

        //将整个链表_other插入到当前链表_pos之前,_other不能是*this
        _iterator splice(_iterator _pos,list& _other){
            _transfer(_pos,_other.begin(),_other.end());
            return _pos;
        }

        //将链表_other的_src所指的节点插入到当前链表_pos之前
        _iterator splice(_iterator _pos,_iterator _src,list& _other){
            _iterator _tmp = _src;
            ++_tmp;
            if(_pos == _tmp || _pos == _src)    return _pos;
            _transfer(_pos,_src,_tmp);
            return _pos;
        }

        //将链表_other的[_src_begin , _src_begin)插入到当前链表_pos之前
        //注意，_pos不能位于_src_begin,_src_end之间
        _iterator splice(_iterator _pos,
                _iterator _src_begin,_iterator _src_end,list& _other){
            if(_src_begin != _src_end){
                _transfer(_pos,_src_begin,_src_end);
            }
            return _pos;
        }

        //将_other按需合并到*this上
        //*this和_other必须已排序
        void merge(list& _other){
            _iterator first1 = begin();
            _iterator last1  = end();

            _iterator first2 = _other.begin();
            _iterator last2  = _other.end();

            while(first1 != last1 && first2 != last2){
                //_other中的元素小于*this中的元素时
                if(*first2 < *first1){
                    _iterator _next = first2;
                    _transfer(first1,first2,++_next);
                    first2 = _next;
                }else
                    ++first1; 
            }

            //_other还有剩余，将剩余的链表挂到*this上
            if(first2 != last2)
                _transfer(last1,first2,last2);
        }

        //翻转list
        //采用头插法将节点依次插入到头结点之前
        void reverse(){
            //空list或只有一个节点的情况下，直接返回
            if(_list->next == _list || _list->next->next == _list){
                return;
            }

            _iterator _first = begin();
            ++_first;
            while(_first != end()){
                _iterator _old = _first;
                ++_first;
                _transfer(begin(),_old,_first);
            }
        }

        //list的sort，采用快排，不能使用STL的sort，
        //因为STL的sort只接受随机迭代器
        void sort(){
            //待实现，等学完快排再来填补
        }

};




#endif // _MY_STL_LIST_H__