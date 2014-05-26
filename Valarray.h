/**

  Valarray.
  Author: Gurbinder Gill
  email: gill@cs.utexas.edu

**/
#ifndef _VALARRAY_H
#define _VALARRAY_H 1

#include <vector>
#include <complex>
#include <cstdint>
#include <cassert>
#include <typeinfo>
#include <iostream>
#include <functional>
#include <numeric>
#include <cmath>
#include <climits>
#include <type_traits>

#include "EPL_traits.h"

using std::vector;
using namespace std;

template <typename T>
struct Sqrt{
    //public:
    typename IF_THEN<(EPL_traits<T>::CRank), std::complex<double>, double>::type 
    operator()(T x) const {
	return std::sqrt(x); 
    }
};


/**************************************************************/
/* INDIRECTION						      */
/**************************************************************/

template <typename T1, typename R1, typename f>
class applyProxy;

template <typename T1, typename R1>
class sqrtProxy;

template <typename T , typename V = std::vector<T>>
class Valarray : public std::vector<T> {
private:
    V _valarr;

public:

    using value_type = T;
    using iterator = typename V::iterator;
    //using const_iterator = typename V::const_iterator;


    Valarray() = default;
    explicit Valarray(uint64_t size) : _valarr(size) { }
    Valarray(uint64_t size, T value) : _valarr(size, value) { }
    
    
    //template<typename T2, typename R2>
    Valarray(const V& valarr) : _valarr(valarr) { /*cout << "must be calling me size = " << valarr.size()<< "\n"; */ }
    
    template<typename T2, typename R2>
    Valarray(const Valarray<T2,R2>& valarr){  //_valarr(valarr) {cout << "must be calling me\n";  }{
     	//cout << "now here \n";
	auto vec = valarr.valarr();
	for(uint64_t i = 0; i < valarr.size(); ++i){
		_valarr.push_back(static_cast<T>(vec[i]));
	    }

    }
    
    Valarray(std::initializer_list<T> il) : _valarr(il) { }

    template <typename T1>
    typename std::enable_if<!EPL_traits<T1>::VRank, Valarray& >::type
    operator=(T1 value) {
	  //  cout << "last constructor\n";
	    uint64_t _size = size();
	    for(uint64_t i = 0; (i < _size); ++i) {
		_valarr[i] = value;

	    } 
	
	return *this;
    } 


    template<typename T2, typename R2>
    Valarray& operator = ( const R2& vec2) {
	using base_type = typename selectType<T, T2>::type;

	if(size() == 0) {
	    for(uint64_t i = 0; i < vec2.size(); ++i)
		(*this).push_back(static_cast<base_type>(vec2[i]));
	}
	else {
 	assert(vec2.size() != 0);
	    uint64_t _size = size();
	    //cout << "size 1 = " << size() << " size 2 = "<< vec2.size() <<endl;
	    for(uint64_t i = 0; (i < _size) && (i < vec2.size()); ++i) {
		_valarr[i] = static_cast<base_type>(vec2[i]);

	    } 
	}
	return *this;
    
    }

    uint64_t size() const { 
	return _valarr.size();
    } 
    
    T operator [] (uint64_t i) const {
	return _valarr[i];
    }
    
    auto operator [] (uint64_t i) -> decltype(_valarr[i]) {
	return _valarr[i];
    }

    const  V& valarr() const {
	return _valarr;
    }
    
    void print(std::ostream& out) const {
	const char* prefix="";
	for(uint64_t i = 0; i < _valarr.size(); ++i) {
	    out <<prefix<< _valarr[i];
	    prefix = " , ";
	}

    }

     /* Iterators */
    iterator begin() {
	return this->begin();
    }

    iterator end() {
	return this->end();
    }

    

    /* push and pop */
    void push_back(const T& val) {
	_valarr.push_back(val);
    }
    
    void pop_back() {
	_valarr.pop_back();
    }
    
/***************************************************************/
/* Unary operation                                             */
/***************************************************************/

 /* Apply function */
    template <typename F> 
    Valarray<typename selectType<T, typename std::result_of<F(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<F(T)>::type >::type, V, F>> 
    apply(F f) {
	   // cout << "step 1 f = " << f(_valarr[0]) <<endl;
	   return apply_proxy(*this , f);
 	}
    
    template <typename F>
    Valarray& apply_base(F f) {
	for(auto& x : _valarr) f(x);
	return *this;
    }
   
    
/* accumulate */
    template <typename F, typename T1>
    T1 accumulate(F f, T1 initial_val){
	T1 result = initial_val;
	for(auto& x : _valarr) 
	   result =  f(result,x);
	return result;

    }
 /* sum */
    T sum() {
	T result = T();
	for(auto& x : _valarr) 
	   result += x;
	return result;

	//return accumulate(std::plus<T>(),0);
    }   
 
/* operator += */

   // template<typename F> 
    Valarray& operator+=(const T& value) {
	apply_base([&](T& a) { a+= value; });
	return *this;
    }


/* operator -= */
    Valarray& operator-=(const T& value) {
	 apply_base([&](T& a) { a-= value; } );
	return *this;
    }

/* operator *= */
    Valarray& operator*=(const T& value) {
	apply_base([&](T& a) { a*= value; } );
	return *this;
    }

    /* function Square root */
     Valarray<typename selectType<T, typename std::result_of<Sqrt<T>(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<Sqrt<T>(T)>::type>::type, V, Sqrt<T>>> 
     sqrt() {
	Sqrt<T> s;
	return apply(s);
    } 
    
    /* negate */
     Valarray<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type>::type, V, std::negate<T>>> 
     operator-(){
	//Sqrt<T> s;
	return apply(std::negate<T>());
    } 

	
}; 




// scaler wrapper
template <typename T>
class Scalar {
public:
    
//    using value_type = correct_type;
      using iterator = typename std::vector<T>::iterator;
      using const_iterator = typename std::vector<T>::const_iterator;
     
   // using value_type = T
    Scalar(T _value) : value(_value) { /*cout << "scalar const V = " << _value <<"\n";*/ }
   
    T operator[](uint64_t i) const {
	    //cout << "V = " << value <<endl;
	    return value;
    }

    uint64_t size() const {
	return INT_MAX;
    }
    

private:
    T value;
};




//template <typename T1, typename R1, typename T2, typename R2>
template <typename T, typename R1, typename R2>
class AddProxy {
public:
    
    AddProxy( const R1& _left,   const R2& _right) : left(_left), right(_right) { /*cout << "const AddProxy size  = " << left.size()<<"\n";*/ }
    
   
    T operator[](uint64_t i) const {
	return (static_cast<T>(left[i]) + static_cast<T>(right[i]));
    }

    uint64_t size() const {
//	cout << "here sizeleft " << left.size()<<" sizeright" << right.size()<<endl;
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    
    
    class iterator {
    public:
	AddProxy* proxy_ptr;
	uint64_t position { 0 };
	
	iterator(AddProxy* _proxy_ptr, uint64_t _position = 0) : proxy_ptr(_proxy_ptr), position(_position) { }
	
	T operator*() {  
	    return proxy_ptr->operator[](position);
	} 
	
	bool operator==(const iterator& rhs) const {
	    return ((this->proxy_ptr == rhs.proxy_ptr)&&(this->position == rhs.position));
	}
	bool operator !=(const iterator& rhs) const {
	    return !((this->proxy_ptr == rhs.proxy_ptr)&&(this->position == rhs.position));
	}
	
	bool operator<(const iterator& rhs) const {
	    if(this->proxy_ptr != rhs.proxy_ptr)
		return false;
	    else 
		return (this->position < rhs.position);
	}
	uint64_t operator-(const iterator& rhs) {
	    if(this->proxy_ptr == rhs.proxy_ptr)
		return (this->position - rhs.position);
	    return false;
	}
	 
	iterator& operator++(void) {
		this->position += 1;
		return *this;
	}
	
	iterator operator++(int){
		iterator t{*this};
		this->operator++();
		return t;
	}
	
	iterator operator--(void) {
		this->position -=1;
		return *this;
	}
	iterator operator--(int){
		iterator t{*this};  
		this->operator--();
		return t;
	}
	
	iterator& operator+=(uint64_t n) {
		this->position += n;
		return *this;
	}
	
	iterator& operator-=(uint64_t n) {
		this->position -=n;
		return *this;
	}
    
	iterator& operator+(uint64_t n) {
		this->position += n;
		return *this;
	}
	
	iterator& operator-(uint64_t n) {
		this->position -=n;
		return *this;
	}
	
	
    };

  
    iterator begin() {
	return iterator {this };
    }
    iterator end(){
	return iterator { this, this->size() };
    }
private:
      const R1& left;
      const R2& right;
    
};


//template <typename T1, typename R1, typename T2, typename R2>
template <typename T, typename R1>
class AddProxy<T, R1, Scalar<T>> {
public:
    
      
    AddProxy(const R1& _left, T _right) : left(_left), right(Scalar<T>(_right)){ };//right(Valarray<T, Scalar<T>>(Scalar<T>(_right))) { cout << "const scalar AddProxy \n";}


     T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) + static_cast<T>(right[i]));
    }

    uint64_t size() const {
	//cout << "SIZE =" <<((left.size() < right.size()) ? left.size() : right.size()) << endl;;
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    

     class iterator {
    public:
	AddProxy* proxy_ptr;
	uint64_t position { 0 };
	
	iterator(AddProxy* _proxy_ptr, uint64_t _position = 0) : proxy_ptr(_proxy_ptr), position(_position) { }
	
	T operator*() {  
	    return proxy_ptr->operator[](position);
	} 
	
	bool operator==(const iterator& rhs) const {
	    return ((this->proxy_ptr == rhs.proxy_ptr)&&(this->position == rhs.position));
	}
	bool operator !=(const iterator& rhs) const {
	    return !((this->proxy_ptr == rhs.proxy_ptr)&&(this->position == rhs.position));
	}
	
	bool operator<(const iterator& rhs) const {
	    if(this->proxy_ptr != rhs.proxy_ptr)
		return false;
	    else 
		return (this->position < rhs.position);
	}
	uint64_t operator-(const iterator& rhs) {
	    if(this->proxy_ptr == rhs.proxy_ptr)
		return (this->position - rhs.position);
	    return false;
	}
	 
	iterator& operator++(void) {
		this->position += 1;
		return *this;
	}
	
	iterator operator++(int){
		iterator t{*this};
		this->operator++();
		return t;
	}
	
	iterator operator--(void) {
		this->position -=1;
		return *this;
	}
	iterator operator--(int){
		iterator t{*this};  
		this->operator--();
		return t;
	}
	
	iterator& operator+=(uint64_t n) {
		this->position += n;
		return *this;
	}
	
	iterator& operator-=(uint64_t n) {
		this->position -=n;
		return *this;
	}
    
	iterator& operator+(uint64_t n) {
		this->position += n;
		return *this;
	}
	
	iterator& operator-(uint64_t n) {
		this->position -=n;
		return *this;
	}
	
	
    };

    
    iterator begin() {
	return iterator {this };
    }
    iterator end(){
	return iterator { this, this->size() };
    }

private:
    const R1& left;
    Scalar<T> right;
    
};



template <typename T, typename R1, typename R2>
class MulProxy {
public:
    
    
    using iterator = typename R1::iterator;
    //using const_iterator = typename R1::const_iterator;

    MulProxy( const R1& _left,   const R2& _right) : left(_left), right(_right) { }
    
   
    T operator[](uint64_t i) const {
	return (static_cast<T>(left[i]) * static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
    const R1& left;
    const R2& right;
    
};



template <typename T, typename R1>
class MulProxy<T, R1, Scalar<T>> {
public:
    
    
    using iterator = typename R1::iterator;
    //using const_iterator = typename R1::const_iterator;

    
    MulProxy( const R1& _left,   T _right) : left(_left), right(Scalar<T>(_right)) { }


    T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) * static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
      const R1& left;
       Scalar<T> right;
    
};



template <typename T, typename R1, typename R2>
class DivProxy {
public:
    
    
    using iterator = typename R1::iterator;
    //using const_iterator = typename R1::const_iterator;

    DivProxy( const R1& _left,   const R2& _right) : left(_left), right(_right) { }
    
   
    T operator[](uint64_t i) const {
	return (static_cast<T>(left[i]) / static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
    const R1& left;
    const R2& right;
    
};





template <typename T, typename R1>
class DivProxy<T, R1, Scalar<T>> {
public:
    
    
    using iterator = typename R1::iterator;
    //using const_iterator = typename R1::const_iterator;

    
    DivProxy( const R1& _left,   T _right) : left(_left), right(Scalar<T>(_right)) { }


    T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) / static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
      const R1& left;
       Scalar<T> right;
    
};



//////////////////////////////////////////////////////////////////////

template <typename T, typename R1>
class DivProxy<Scalar<T>, T, R1> {
public:
    
    
    using iterator = typename R1::iterator;
   // using const_iterator = typename R1::const_iterator;

    
    DivProxy(  T _left , const R1& _right) : right(_right), left(Scalar<T>(_left)) { }


    T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) / static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
      const R1& right;
       Scalar<T> left;
    
};
/////////////////////////////////////////////////////////////////////////

template <typename T, typename R1, typename R2>
class SubProxy {
public:
    
    
    using iterator = typename R1::iterator;
   // using const_iterator = typename R1::const_iterator;

    SubProxy( const R1& _left,   const R2& _right) : left(_left), right(_right) { }
    
   
    T operator[](uint64_t i) const {
	return (static_cast<T>(left[i]) - static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
    const R1& left;
    const R2& right;
    
};


/************************************************
* Experimenting : since substraction is not commutative we need 2 overloads.
************************************************/
template <typename T, typename R1>
class SubProxy<T, R1, Scalar<T>> {
public:
    
    
    using iterator = typename R1::iterator;
   // using const_iterator = typename R1::const_iterator;

    
    SubProxy( const R1& _left,   T _right) : left(_left), right(Scalar<T>(_right)) { }


    T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) - static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
      const R1& left;
       Scalar<T> right;
    
};
//////////////////////////////////////////////////////////////////////

template <typename T, typename R1>
class SubProxy<Scalar<T>, T, R1> {
public:
    
    
    using iterator = typename R1::iterator;
   // using const_iterator = typename R1::const_iterator;

    
    SubProxy(  T _left , const R1& _right) : right(_right), left(Scalar<T>(_left)) { }


    T operator[](uint64_t i) const {
	    return (static_cast<T>(left[i]) - static_cast<T>(right[i]));
    }

    uint64_t size() const {
	return (left.size() < right.size()) ? left.size() : right.size();
    }
    
    iterator begin() {
	return left.begin();
    }
    iterator end(){
	return left.end();
    }
private:
      const R1& right;
       Scalar<T> left;
    
};
/////////////////////////////////////////////////////////////////////////







template <typename T, typename R, typename F>
class applyProxy {
public:
    
    using iterator = typename R::iterator;
  //  using const_iterator = typename R::const_iterator;

     applyProxy(const R& _left, F _function) : left(_left), function(_function) {    /* cout <<"inside apply_proxy constructor f =  "<< function(3) <<endl;*/}
   
       
    T operator[](uint64_t i) const {
	    //TODO: check this static_cast	
	    //return function(static_cast<T>(left[i]));
	    return function(left[i]);
    }

    uint64_t size() const {
	
	return left.size();
    }
private:
    const R& left;
    F function;


};


//Add

template <typename T1, typename R1, typename T2, typename R2>
/*typename std::enable_if<(EPL_traits<R1>::VRank | EPL_traits<R2>::VRank), Valarray<typename selectType<T1, T2>::type , AddProxy<T1, R1, T2, R2>>>::type*/
Valarray<typename selectType<T1, T2>::type , AddProxy<typename selectType<T1, T2>::type , R1, R2>>
operator+(const Valarray<T1, R1>& x, const Valarray<T2, R2>& y) {
    return Valarray<typename selectType<T1, T2>::type, AddProxy<typename selectType<T1, T2>::type , R1, R2>>(AddProxy<typename selectType<T1, T2>::type, R1, R2>(x.valarr() , y.valarr()));
}


template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , AddProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator+(const Valarray<T1, R1>& x, T2 y) {
    return Valarray<typename selectType<T1, T2>::type, AddProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(AddProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}

template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , AddProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator+( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, AddProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(AddProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}


//Multiply

template <typename T1, typename R1, typename T2, typename R2>
/*typename std::enable_if<(EPL_traits<R1>::VRank | EPL_traits<R2>::VRank), Valarray<typename selectType<T1, T2>::type , AddProxy<T1, R1, T2, R2>>>::type*/
Valarray<typename selectType<T1, T2>::type , MulProxy<typename selectType<T1, T2>::type , R1, R2>>
operator*(const Valarray<T1, R1>& x, const Valarray<T2, R2>& y) {
    return Valarray<typename selectType<T1, T2>::type, MulProxy<typename selectType<T1, T2>::type , R1, R2>>(MulProxy<typename selectType<T1, T2>::type, R1, R2>(x.valarr() , y.valarr()));
}


template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , MulProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator*(const Valarray<T1, R1>& x, T2 y) {
    return Valarray<typename selectType<T1, T2>::type, MulProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(MulProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}

template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , MulProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator*( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, MulProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(MulProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}




//Divide

template <typename T1, typename R1, typename T2, typename R2>
/*typename std::enable_if<(EPL_traits<R1>::VRank | EPL_traits<R2>::VRank), Valarray<typename selectType<T1, T2>::type , AddProxy<T1, R1, T2, R2>>>::type*/
Valarray<typename selectType<T1, T2>::type , DivProxy<typename selectType<T1, T2>::type , R1, R2>>
operator/(const Valarray<T1, R1>& x, const Valarray<T2, R2>& y) {
    return Valarray<typename selectType<T1, T2>::type, DivProxy<typename selectType<T1, T2>::type , R1, R2>>(DivProxy<typename selectType<T1, T2>::type, R1, R2>(x.valarr() , y.valarr()));
}


template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , DivProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator/(const Valarray<T1, R1>& x, T2 y) {
    return Valarray<typename selectType<T1, T2>::type, DivProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(DivProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}

/*
template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , DivProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator/( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, DivProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(DivProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}
*/

template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , DivProxy<Scalar<typename selectType<T1, T2>::type> , typename selectType<T1, T2>::type,  R1>> 
operator/( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, DivProxy< Scalar<typename selectType<T1, T2>::type>, typename selectType<T1, T2>::type, R1>>(DivProxy<Scalar<typename selectType<T1, T2>::type>, typename selectType<T1, T2>::type, R1>(y, x.valarr()));
}



//Subtract

template <typename T1, typename R1, typename T2, typename R2>
/*typename std::enable_if<(EPL_traits<R1>::VRank | EPL_traits<R2>::VRank), Valarray<typename selectType<T1, T2>::type , AddProxy<T1, R1, T2, R2>>>::type*/
Valarray<typename selectType<T1, T2>::type , SubProxy<typename selectType<T1, T2>::type , R1, R2>>
operator-(const Valarray<T1, R1>& x, const Valarray<T2, R2>& y) {
    return Valarray<typename selectType<T1, T2>::type, SubProxy<typename selectType<T1, T2>::type , R1, R2>>(SubProxy<typename selectType<T1, T2>::type, R1, R2>(x.valarr() , y.valarr()));
}


template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , SubProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator-(const Valarray<T1, R1>& x, T2 y) {
    return Valarray<typename selectType<T1, T2>::type, SubProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(SubProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}

/*
template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , SubProxy<typename selectType<T1, T2>::type,  R1, Scalar<typename selectType<T1, T2>::type>>> 
operator-( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, SubProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>>(SubProxy<typename selectType<T1, T2>::type, R1, Scalar<typename selectType<T1, T2>::type>>(x.valarr(), y));
}
*/

template <typename T1, typename R1, typename T2>
Valarray<typename selectType<T1, T2>::type , SubProxy<Scalar<typename selectType<T1, T2>::type> , typename selectType<T1, T2>::type,  R1>> 
operator-( T2 y, const Valarray<T1, R1>& x) {
    return Valarray<typename selectType<T1, T2>::type, SubProxy< Scalar<typename selectType<T1, T2>::type>, typename selectType<T1, T2>::type, R1>>(SubProxy<Scalar<typename selectType<T1, T2>::type>, typename selectType<T1, T2>::type, R1>(y, x.valarr()));
}
/* NEGATION */
/*
template <typename T, typename R>
Valarray<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type>::type, R, std::negate<T>()> >
operator-(const Valarray<T, R>& x) {
    return Valarray<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type>::type, R, std::negate<T>()>>(
    applyProxy<typename selectType<T, typename std::result_of<std::negate<T>(T)>::type >::type, R,std::negate<T>()>(x.valarray(), std::negate<T>()));
} 
*/


template <typename T, typename R, typename F>
Valarray<typename selectType<T, typename std::result_of<F(T)>::type >::type , applyProxy<typename selectType<T, typename std::result_of<F(T)>::type >::type, R, F>> 
apply_proxy(const Valarray<T, R>& x, F f) {
    return Valarray<typename selectType<T, typename std::result_of<F(T)>::type >::type, applyProxy<typename selectType<T, typename std::result_of<F(T)>::type >::type, R, F>>(applyProxy<typename selectType<T, typename std::result_of<F(T)>::type >::type, R,F>(x.valarr(), f));
}




template <typename T, typename R>
std::ostream& operator<<(std::ostream& out, const Valarray<T, R>& valarr) {
	valarr.print(out);
	return out;
}

#endif /* _VALARRAY_H */
