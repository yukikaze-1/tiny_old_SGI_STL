#ifndef __MY_STL_ALLOC_H__
#define __MY_STL_ALLOC_H__ 1

#include <malloc.h>

// 1 表示使用第一级配置器，0表示使用第二级配置器
#define __USE_MALLOC 0

//多线程有关的宏
#define __MY_NODE_ALLOCATOR_THREADS 1

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//第一级配置器
#if __USE_MALLOC
    #include <new>
    #define __THROW_BAD_ALLOC  throw bad_alloc
#elif !defined(__THROW_BAD_ALLOC)
    #include <iostream>
    #define __THROW_BAD_ALLOC std::cerr<<"out of memory"<<std::endl;exit(1);
#endif

template<int _inst>
class malloc_alloc_template{

    private:
    //内存不足时的处理函数
    static void* _oom_malloc(size_t _bytes);
    static void* _oom_realloc(void* _ptr,size_t _bytes);

    //一个static指针，指向一个返回值为void，没有参数的函数,指针名字为_my_malloc_alloc_oom_handler
    //可由该类中的_my_set_malloc_handler函数进行设置
    static void (* _malloc_alloc_oom_handler)();
    
    public:
    //allocate memory ,use malloc function
    static void* allocate(size_t _bytes){
        void* _ret = malloc(_bytes);
        if(_ret == NULL){
            _ret = _oom_malloc(_bytes);
        }
        return _ret;   
    }

    //deallocate memory ,use free function
    static void deallocate(void* _ptr,size_t _bytes){
        free(_ptr);
    }

    //realloc memory ,use realloc function
    static void* reallocate(void* _ptr,size_t _old_size,size_t _new_size){
        void* _ret = realloc(_ptr,_new_size);
        if(_ret == NULL){
            _ret =_oom_realloc(_ptr,_new_size);
        }
        return _ret;
    }


    //仿真c++的set_new_handler()，用于设置自己的 out of memory handler，
    //即oom时调用的处理函数
    static void (*set_malloc_handler(void (*_f)()))(){
        //保存旧的处理函数，并返回
        void (*_old)() = _malloc_alloc_oom_handler;
        //设置新的处理函数
        _malloc_alloc_oom_handler = _f;
        return _old;
    }
    /*更易懂的写法
    static auto set_malloc_handler_1(void (*_f)())->void(*)(){
        //保存旧的处理函数，并返回
        void (*_old)() = _malloc_alloc_oom_handler;
        //设置新的处理函数
        _malloc_alloc_oom_handler = _f;
        return _old;
    }*/

};// end of class 

//oom时的处理函数指针，初始设置为nullptr，由用户自己设置
template<int _inst>
void (*malloc_alloc_template<_inst>::_malloc_alloc_oom_handler)() = nullptr;


//_oom_malloc
template<int _inst>
void* 
malloc_alloc_template<_inst>::_oom_malloc(size_t _size){
    void (*_my_malloc_handler)();
    void* _ret;
    while(1){
        //取得oom时的处理函数
        _my_malloc_handler = _malloc_alloc_oom_handler;
        //如果没有设置oom时的处理函数，则抛出异常
        if(nullptr == _my_malloc_handler){
            __THROW_BAD_ALLOC;
        }
        //如果设置了oom时的处理函数，则调用之
        (*_my_malloc_handler)();
        //重新申请内存
        _ret = malloc(_size);
        //如果申请成功，则直接返回，否则进入下一个循环
        if(_ret != nullptr) 
            return _ret;
    }
}


//_oom_realloc
template<int _inst>
void*
malloc_alloc_template<_inst>::_oom_realloc(void* _ptr,size_t _size){
    void (*_my_malloc_handler)();
    void* _ret;
    while(1){
        //取得oom时的处理函数
        _my_malloc_handler = _malloc_alloc_oom_handler;
        //如果没有设置oom时的处理函数，则抛出异常
        if(nullptr == _my_malloc_handler){
            __THROW_BAD_ALLOC;
        }
        //如果设置了oom时的处理函数，则调用之
        (*_my_malloc_handler)();
        //重新申请内存
        _ret = realloc(_ptr,_size);
        //如果申请成功，则直接返回，否则进入下一个循环
        if(_ret != nullptr) 
            return _ret;
    }
}

//用于配置选择配置器，如果选择一级配置器，则可以使用
//using alloc = malloc_alloc_first;即malloc_alloc_first代表第一级配置器
//alloc是simple_alloc选择配置器时的参数
using malloc_alloc_first = malloc_alloc_template<0>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//第二级配置器

//class中的静态变量初值
//freelist中每个链表块大小的差值
enum{__ALIGN = 8};
//freelist中所有链表块大小的上限
enum{__MAX_BYTES = 256};
//freelist中有共多少个链表，此处为256/8 = 32
enum{__NUMS_OF_FREELIST = __MAX_BYTES/__ALIGN};


template<bool _threads,int _inst>
class default_alloc_template{
    private:

        /*内存池的一些参数*/
        //chunk_alloc()使用
        static char* _memory_pool_start;
        static char* _memory_pool_end;
        //用于内存池耗尽时，向heap申请空间时的一个随申请次数递增的附加量
        static size_t _heap_size_use;

        //用于将用户申请的空间大小提升至8的整数倍，从而方便分配
        static size_t ROUND_UP(size_t _bytes){
            return (((_bytes + __ALIGN - 1)) & ~(__ALIGN - 1));
        }

        //union obj 定义
        union _obj{
            union _obj* _freelist_link_next;
            //该指针指向一列同等大小的内存块，用于取下并返回给用户
            char* _client_data[1];
        };

        //freelist定义，为一个静态的数组，元素为union obj*
        static _obj* volatile _freelist[__NUMS_OF_FREELIST];

        //根据区块大小，选择合适的区块大小，并定位到相应的freelist下标
        static size_t FREELIST_INDEX(size_t _bytes){
            return (((_bytes) + __ALIGN -1 )/__ALIGN - 1);
        }

        //refill freelist,返回一个大小为_bytes的区块，
        // 并且可能会将额外的同样大小的区块加入freelist
        static void* refill_freelist(size_t _bytes){

            int _n_objs = 20;
            char* _chunk = chunk_alloc(_bytes,_n_objs);

            //_free_list_ptr指向freelist中的一个节点
            _obj* volatile * _freelist_ptr ;
            _obj* _ret,*_cur_obj,* _next_obj;

            //只申请到一个区块的情况,直接返回申请到的内存块，freelist不做处理
            if(1 == _n_objs){
                return (_chunk);
            }

            //申请到不知一个区块，先将申请到的多余的区块挂到freelist的相应节点上，后返回申请到的内存
            //先定位到freelist上待挂节点的位置
            _freelist_ptr = _freelist + FREELIST_INDEX(_bytes); 

            _ret = (_obj*)_chunk;

            //freelist的节点的指针指向 待挂在freelist上的众多内存块中的第一个内存块
            *_freelist_ptr = _next_obj = (_obj*)(_chunk + _bytes);

            //接下来，将申请到的内存块依次串联起来
            for(int _i = 1;;++_i){
                _cur_obj = _next_obj;
                _next_obj = (_obj*)((char*)_next_obj + _bytes);
                if(_n_objs - 1 == _i){
                    _cur_obj->_freelist_link_next = nullptr;
                    break;
                }else{
                    _cur_obj->_freelist_link_next = _next_obj;
                }
            }
            //因为_ret是_obj*，而函数的返回值为void* ，故需要转换下
            return (_ret);
        }


        //_my_chunk_alloc
        static char* chunk_alloc(size_t _size ,int& _n_objs){
            char* _ret;
            //总共需要申请的字节数
            size_t _total_need_bytes = _size * _n_objs;
            //剩余字节数
            size_t _bytes_left = _memory_pool_end -_memory_pool_start  ;

            //如果内存池剩余的空间能满足需求
            if(_bytes_left >= _total_need_bytes){
                _ret = _memory_pool_start;
                //更新内存池的参数
                _memory_pool_start += _total_need_bytes; 
                return _ret;

            }else if(_bytes_left >= _size){
            //内存池剩余空间不能完全满足用户省申请的空间，但是至少能供应一个供用户使用
            //返回一个区块供用户使用，如果有额外的区块，就将额外的区块挂到freelist相应的节点上(由refill实现)  
                _n_objs = _bytes_left/_size;
                _total_need_bytes = _size * _n_objs;
                _ret = _memory_pool_start;
                _memory_pool_start += _total_need_bytes;
                return _ret;

            }else{
            //内存池不足以提供至少一个区块
                size_t _bytes_to_get = 2 * _total_need_bytes + ROUND_UP(_heap_size_use>>4);

                //内存池还有些残余
                if(_bytes_left >0){
                    //寻找合适的freelist大小的区块，将内存池中剩余的内存块挂到freelist中
                    _obj* volatile * _free_list_ptr;
                    _free_list_ptr = _freelist + FREELIST_INDEX(_bytes_left);
                    ((_obj*)_memory_pool_start)->_freelist_link_next = *_free_list_ptr;
                    *_free_list_ptr = (_obj*)_free_list_ptr;
                }

                //配置heap空间，用于补充内存
                _memory_pool_start = (char*)malloc(_bytes_to_get);

                //如果heap也没空间了,简化一部分
                if(nullptr == _memory_pool_start){
                    int _i;
                    _obj* volatile* _free_list_ptr,*_p;
                    for(_i = _size;_i<__MAX_BYTES;_i+=__ALIGN){
                        _free_list_ptr = _freelist + FREELIST_INDEX(_i);
                        _p = *_free_list_ptr;
                        //如果freelist中还有尚未使用的合适的足够大的区块，调整freelist释放区块
                        if(nullptr != _p){
                            *_free_list_ptr = _p->_freelist_link_next;
                            _memory_pool_start = (char*)_p;
                            _memory_pool_end = _memory_pool_start + _i;
                            //释放了合适的区块，递归调用自己，用于给freelist分配空间
                            return (chunk_alloc(_size,_n_objs));
                        }//注意：任何残余的领头都将被编入适当的freelist中备用
                    }
                    //此时，山穷水尽,调用第一级配置器，看是否能有用，没用的话会抛出异常
                    _memory_pool_end = nullptr;
                    _memory_pool_start = (char*)malloc_alloc_first::allocate(_bytes_to_get);
                }

                //向heap申请内存成功
                _heap_size_use += _bytes_to_get;
                _memory_pool_end = _memory_pool_start + _bytes_to_get;

                //此时，从heap中申请到了足够的空间，递归调用自己，用于给freelist分配空间
                return (chunk_alloc(_size,_n_objs));
            }
        }




    public:
        static void* allocate(size_t _bytes){
            _obj* _ret;
            _obj* volatile * _freelist_ptr; 

            //申请的大小大于freelist最大区块的值，调用一级配置器
            if(_bytes >  (size_t) __MAX_BYTES){
                return malloc_alloc_first::allocate(_bytes);
            }

            //将_free_list指向freelist中合适区块的节点
            _freelist_ptr = _freelist + FREELIST_INDEX(_bytes);

            //取出该节点内的元素，该元素为一个指针，指向一个obj链表
            _ret = *_freelist_ptr;

            //没在freelist中找到合适的区块,调用refill补充freelist的区块
            if(_ret == nullptr){
                void * _r = refill_freelist(ROUND_UP(_bytes));
                return _r; 
            }

            //在freelist中找到了合适的区块，调整freelist中的相应节点的指针
            *_freelist_ptr = _ret->_freelist_link_next;
            
            return (_ret);
        }
        
        static void  deallocate(void* _ptr,size_t _bytes){
            _obj* _p = (_obj*)_ptr;
            _obj* volatile * _freelist_ptr ;

            //如果大于256字节，则直接调用一级配置器的deallocate
            if(_bytes > (size_t) __MAX_BYTES){
                malloc_alloc_first::deallocate(_ptr,_bytes);
                return ;
            }

            //小于256字节
            //找到对应的区块大小的链表的freelist节点，并用_free_list_ptr指向该节点
            _freelist_ptr = _freelist + FREELIST_INDEX(_bytes);  
            //归还的内存块以头插法插入到相应的链表中
            _p->_freelist_link_next = *_freelist_ptr;
            *_freelist_ptr = _p;
        }
        
        static void reallocate(void* _ptr,size_t _old_size,size_t _new_size){
                /*不必实现，因为在allocate()中调用的_my_chunk_alloc()会解决这部分*/    
        }


};

//freelist的初始值
template<bool _threads,int _inst>
typename default_alloc_template<_threads,_inst>::_obj* volatile
default_alloc_template<_threads,_inst>::_freelist[__NUMS_OF_FREELIST]
 = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//内存池初始值定义
template<bool _threads,int _inst>
char*
default_alloc_template<_threads,_inst>::_memory_pool_start = nullptr;

template<bool _threads,int _inst>
char*
default_alloc_template<_threads,_inst>::_memory_pool_end = nullptr;

template<bool _threads,int _inst>
size_t
default_alloc_template<_threads,_inst>::_heap_size_use = 0;

//二级配置器的别名
using malloc_alloc_second = default_alloc_template<__MY_NODE_ALLOCATOR_THREADS,0>;


////////////////////////////////////////////////////////////////////////////////////////////////
//进行配置器的选择
//__USE_MALLOC代表选择使用第一级配置器，否则，选则使用第二级配置器
//alloc代表配置器
#if __USE_MALLOC
    using alloc = malloc_alloc_first;
#else
    using alloc = malloc_alloc_second;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//一、二级配置器的统一接口
//simple_alloc
template<class _T,class _Alloc  = alloc>
class simple_alloc{
    public:
        static _T* allocate(size_t _num){
            return (_num == 0?nullptr:(_T*)_Alloc::allocate(_num* sizeof(_T)));
        }
        static _T* allocate(){
            return (_T*)_Alloc::allocate(sizeof(_T));
        }
        static void deallocate(_T* _ptr,size_t _num){
            if(_ptr != nullptr){
                _Alloc::deallocate(_ptr,_num * sizeof(_T));
            }
        }
        static void deallocate(_T* _ptr){
            _Alloc::deallocate(_ptr,sizeof(_T));
        }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  /* __MY_STL_ALLOC_H__ */