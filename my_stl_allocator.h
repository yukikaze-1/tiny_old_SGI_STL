#ifndef __MY_STL_ALLOCATOR__
#define __MY_STL_ALLOCATOR__ 1

#include "my_stl_alloc.h" 

//stl标准的 配置器接口

    //////////////////////////////////////////////////////////////////////// 
    template<class _T>
    inline  _T*
    _allocate(ptrdiff_t _size){
        return simple_alloc<_T>::allocate(_size);
    }

    template<class _T>
    inline void
    _deallocate(_T* _ptr){
        simple_alloc<_T>::deallocate(_ptr);
    }

    template <class _T,class _T1>
    inline  void
    _construct(_T * _ptr,const _T1& _value){
        construct(_ptr, _value);
    }

    //指针
    template <class _T>
    inline  void
    _destroy(_T * _ptr){
        destroy(_ptr);
    }

    /*
    //迭代器,为什么没有迭代器版本？
    template <class _ForwardIterator>
    inline void
    _destroy(_ForwardIterator _first, _ForwardIterator _last){
        my_destroy(_first, _last);
    }
    */

    ////////////////////////////////////////////////////////////////////////

    template <class _T>
    class allocator{
        public:
        /*配置器属性*/
        using value_type      = _T;
        using pointer         = _T*;
        using reference       = _T&;
        using const_pointer   = const _T*;
        using const_reference = const _T&;
        using size_type       = size_t;
        using difference_type = ptrdiff_t;

        template <class _U>
        struct rebind{
            using other = allocator<_U>;
        };
        //default constructor

        //default destructor

        //copy constructor
        //alloctor(const allocator& _other){}

        //template copy constructor
        //alloctor(const allocator<_T>& _other){}

        //allocate 
        static pointer allocate(size_t _bytes,const void* _hint = nullptr){
            return _allocate((difference_type)_bytes,(pointer)_hint);
        }

        //deallocate
        static void deallocate(pointer _ptr,size_type _size){
            _deallocate(_ptr,_size);
        }

        //construct
        static void construct(pointer _ptr,const _T& _value){
            _construct(_ptr,_value);
        }

        //destroy
        static void destroy(pointer _ptr){
            _destroy(_ptr);
        }

        static pointer address(reference _element){
            return (pointer)&_element;
        }

        static const_pointer address(const_reference _element){
            return (const_pointer)&_element;
        }

         size_type max_size() const {
            return (size_type)(UINT_MAX/sizeof(_T));
        }
    };//end class


#endif // _MY_STL_ALLOCATOR_