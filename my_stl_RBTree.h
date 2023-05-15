#ifndef __MY_STL_RBTREE_H__
#define __MY_STL_RBTREE_H__ 1

#include "my_stl_algorithm.h"
#include "my_memory.h"

enum _rb_tree_color {_rb_tree_color_red = false, _rb_tree_color_black  = true};

 
struct rb_tree_node_base{
    using _rb_base_ptr = rb_tree_node_base*;

    _rb_tree_color color;
    _rb_base_ptr parent;
    _rb_base_ptr left;
    _rb_base_ptr right;

    //求RBtree中的最小节点
    static _rb_base_ptr _minium(_rb_base_ptr p){
        while(p->left != nullptr) {
            p = p->left;
        }
        return p;
    }
    //求RBtree中的最大节点
    static _rb_base_ptr _maximum(_rb_base_ptr p){
        while(p->right != nullptr){
            p = p->right;
        }
        return p;
    }
};

struct rb_tree_base_iterator{
    //类型定义
    using _rb_base_ptr       = rb_tree_node_base::_rb_base_ptr;
    using _iterator_category = Bidirectional_iterator_tag;
    using _difference_type   = ptrdiff_t;

    //内部指针
    _rb_base_ptr _node;

    //迭代器+1
    void _increment(){
            //当右子树不空
        if(_node->right != nullptr){
            //进入右子树
            _node = _node->right;
            //向左走，找到该子树中的最小节点
            while(_node != nullptr){
                _node = _node->left;
            }
        }else{
            //当右子树空
            //找到父节点
            auto p = _node->parent;
            //若现节点还是右节点，一直向上
            while(_node == p->right){
                _node = p;
                p = p->parent;
            }
            /*
            *由于RBTree的实现为root的parent为一个header
            *当root无右儿子时，其右儿子实际上是指向header的
            *搜索一需要加这么一个判断
            */
            if(_node->right != p){
                _node = p;
            }
        }
    }

    //迭代器-1
    void _decrement(){
        if(_node->left != nullptr){
            _node = _node->left;

            while(_node != nullptr){
                _node = _node->right;
            }
        }else{
            auto p = _node->parent;

            while(_node == p->left){
                _node = p;
                p = p->parent; 
            }

            if(_node->left != p){
                _node = p;
            }
        }
    }
};


template<class T> 
struct rb_tree_node : public rb_tree_node_base{
    using  _link_type = rb_tree_node<T>*;
    T _value_field;
};


template<class Value,class Ref,class Ptr> 
struct rb_tree_iterator : public rb_tree_base_iterator{
    //类型定义
    using _value_type      = Value;
    using _reference       = Ref;
    using _pointer         = Ptr;
    using _const_reference = const Ref;
    using _const_pointer   = const Ptr;

    using _iterator               = rb_tree_iterator<Value,       Ref,       Ptr>;
    using _const_iterator         = rb_tree_iterator<const Value, const Ref, const Ptr>;
    using _reverse_iterator       = reverse_iterator<_iterator>;
    using _const_reverse_iterator = reverse_iterator<_const_iterator>;
    using _link_type              = rb_tree_node<Value>*;
    using _self                   = rb_tree_iterator<Value,       Ref,       Ptr>;
    
    //构造函数
    rb_tree_iterator()=default;
    rb_tree_iterator(_link_type _x){_node=_x;}
    rb_tree_iterator(const _self& _other){_node = _other._node;}

    //功能实现
    _reference operator*() const {return _link_type(_node)->_value_field;}
    _pointer operator->() const {return &(operator*());}

    _self& operator++() {_increment(); return *this;}
    _self& operator--() {_decrement(); return *this;}
    _self& operator++(int){_self _ret = *this; _increment(); return _ret;}
    _self& operator--(int){_self _ret = *this; _decrement(); return _ret;}
};


//rbtree definition
template <class Value,class Key,class KeyOfValue,class Compare,class _Alloc = alloc>
class _rb_tree{
    protected:
    //类型定义
        using void_pointer           = void*;
        using base_ptr               = rb_tree_node_base*;
        using rb_tree_node           = rb_tree_node<Value>;
        using rb_tree_node_allocator = simple_allocator<rb_tree_node, _Alloc>;

    public:
        using _key_type        = Key;
        using _value_type      = Value;
        using _pointer         = _value_type*;
        using _const_pointer   = const _value_type*;
        using _reference       = _value_type&;
        using _const_reference = const _value_type&;
        using _link_type       = rb_tree_node*;
        using _size_type       = size_t;
        using _difference_type = ptrdiff_t;

    protected:
        _link_type _get_node(){return rb_tree_node_allocator::allocate();}
        void _perish_node(_link_type p){rb_tree_node_allocator::deallocate(p);}

        _link_type _create_node(const _value_type& _value){ 
            _link_type _ret = _get_node();
            try{construct(&_ret->_value_field,_value);}
            catch(...) {_perish_node(_ret);}
            return _ret;
        }

        void _destroy_node(_link_type& p){
            destroy(&p->_value_field);
            _perish_node(p);
        }
        _link_type _clone_node(_link_type _other){
            _link_type _ret = _create_node(_other->_value_field);
            _ret->color = _other->color;
            _ret->left = nullptr;
            _ret->right = nullptr.
            return _ret;
        }

    protected:
        _size_type _node_count;
        _link_type _header;
        Compare key_cmp;

        _link_type root() const { return (_link_type&)(_header->parent);}
        _link_type leftmost() const { return (_link_type&)(_header->left);}
        _link_type rightmost() const { return (_link_type&)(_header->right);}

        //p219

    private:

};

#endif