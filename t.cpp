#include <iostream>
#include <deque>
#include <vector>
#include <algorithm>
#include <stack>
#include <map>
using namespace std;

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
template<typename _T>
void show(const vector<_T>& _vector){
    
   if(_vector.empty())
        cout<<"Empty vector"<<endl; 
   else     
        for(auto& e: _vector)
            cout<<e<<',';
    cout<<endl;
}

using namespace std;
int main(){
    vector<int> num{0,1,2,3,4,5,6,7,8,9};
    make_heap(num.begin(), num.end());
    show(num);
    cout<<"*************"<<endl;
    num.push_back(99);
    push_heap(num.begin(), num.end());
    show(num);

    //a.push_back(1);
    //show(a);

    return 0;
}