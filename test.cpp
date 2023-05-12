#include <iostream>
#include "my_list.h"
#include "my_vector.h"
#include "my_stl_deque.h"
#include "my_stl_stack.h"


using std::cout;
using std::endl;
struct __in_type{};
struct __out_type{};



template<typename _T,typename _U>
void show(const vector<std::pair<_T,_U>>& _vector){
    if(_vector.empty())
        cout<<"Empty vector"<<endl; 
    for(auto e: _vector){
       cout<<e.first<<','<<e.second<<endl;
    } 
    cout<<endl;
}


template<typename _T>
void show(const vector<_T>& _vector){
    
   if(_vector.empty())
        cout<<"Empty vector"<<endl; 
   else     
        for(auto& e: _vector)
            cout<<e<<',';
         
   
    cout<<endl;
}

template <class T>
void show(const list<T>& x){
    for(auto p = x.begin(); p != x.end();++p){
        cout<<*p<<',';
    }
    cout<<endl;
}

template <class T>
void show(const deque<T>& _deque){
    if(_deque.empty()){
        cout<<"Empty deque"<<endl; 
    }else{
        for(auto& e: _deque){
            cout<<e<<',';
        }
        cout<<endl;
    }
}

int main(){
    
   stack<int> s;

    cout<<"all ended"<<endl;  
   return 0;
}

