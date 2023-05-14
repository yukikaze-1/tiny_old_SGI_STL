#ifndef __MY_STL_RBTREE_H__
#define __MY_STL_RBTREE_H__ 1

#include "my_stl_algorithm.h"
#include "my_memory.h"


template<class T> 
struct tb_tree_node_base{

};

template<class T> 
struct tb_tree_node : public tb_tree_node_base<T>{

};

template<class T> 
struct tb_tree_base_iterator{

};

template<class T> 
struct tb_tree_iterator : public tb_tree_base_iterator<T>{

};


//rbtree definition
template <class Value,class Key,class KeyOfValue,class Ref,class Ptr>
class _rb_tree{

};

#endif