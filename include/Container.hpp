/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov: Container interface and Hashmap added
 */

#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include <stdint.h>
#include <assert.h>

namespace smallrdf {

#if __cplusplus < 201103L && !defined(nullptr)
	#ifdef NULL
		#define nullptr  NULL
	#else
		#define nullptr  __null
	#endif // NULL
#endif // < __cplusplus 11+

// Interface ===================================================================
//template<typename T>
//class Managed {
//	T* _val;
//	uint16_t  _count;
//	//Hash _hash;
//public:
////	Managed(T& val)(noexcept(T()));  // Required when const T's copy constructor is not defined
////	Managed(const T& val)(noexcept(T()));
////		{ return add(const_cast<T&>(val)); }
//	Managed(T*& val);
//
//	~Managed();
//};

template<typename T>
struct Iterator {
	typedef T  value_type;

	virtual ~Iterator()  {}

	virtual Iterator* next() const=0;
	//virtual Iterator& operator++()=0;  //!< Preincrement
	//virtual const Iterator& operator++(int)=0 const;  //!< Postincrement

	virtual T& operator*()=0;
	virtual const T& operator*() const final
		{ return **const_cast<Iterator*>(this); }
};

template<typename T>
class Container {
public:
	typedef T  value_type;
	typedef Iterator<T>  Iter;

	virtual ~Container()  {}
	virtual unsigned length() const=0;

	virtual T* add(T& val)=0;  // Required when const T's copy constructor is not defined
	virtual T* add(const T& val) final
		{ return add(const_cast<T&>(val)); }

	virtual Iter* find(const T& val)=0;
	virtual const Iter* find(const T& val) const final
		{ return const_cast<Container*>(this)->find(val); }
	virtual T& operator[](const T& val) final
		{ return **find(val); }
	virtual const T& operator[](const T& val) const final
		{ return **find(val); }

	// Note: const variants should not be made final to allow specialized return types
	virtual const Iter* begin() const //final
		{ return const_cast<Container*>(this)->begin(); }
	virtual Iter* begin()=0;

	virtual const Iter* end() const //final
		{ return const_cast<Container*>(this)->end(); }
	virtual Iter* end()=0;
};

// Stack -----------------------------------------------------------------------
template<typename T>
class StackNode: public Iterator<T> {
//	union {
//		T  _val;
//		uint8_t  _empty[sizeof(T)];
//	};
	T  _val;
	StackNode* _next;

//	StackNode()
//		: _empty{0}, _next(nullptr) {}
public:
	typedef T  value_type;
	typedef Iterator<T>  Iter;
	using Iter::operator*;

	// Note: initialization by non-const reference is required when default
	// copy constructor does not exist
	StackNode(T& val, StackNode* next=nullptr)
		: _val(val), _next(next)  {}
	StackNode(const T& val, StackNode* next=nullptr)
		: _val(val), _next(next)  {}
#if __cplusplus >= 201103L
	StackNode(StackNode&&)=default;
#endif // __cplusplus 11+
	StackNode(const StackNode&)=default;
//	StackNode(StackNode& other)
//		// Note: T(other._val)) for T=String acquires the ownership
//		: _val(T(other._val)), _next(other._next)  {}

#if __cplusplus >= 201103L
	StackNode& operator=(StackNode&&)=default;
#endif // __cplusplus 11+
	StackNode& operator=(const StackNode&)=default;
//	StackNode& operator=(StackNode& other)
//		{
//			// Note: T(other._val)) for T=String acquires the ownership
//			_val = T(other._val);
//			_next = other._next;
//		}

	~StackNode()  {}

	StackNode* next() const override
		{ return _next; }
	// Note: operator++() requires std::move for non-pointer data, so next() is used
	//Iter& operator++() override
	//	{ assert(_next && "Incrementing end iterator"); return *(this = _next); }
	//const Iter& operator++(int) override;

	T& operator*() override
		{ return _val; }

	static StackNode* blank()  //!< Blank (empty) node, which serves as end()
		//{ static StackNode sn; return &sn; }
		{ return reinterpret_cast<StackNode*>(-1); }
};

template<typename T>  // , StackNode<T> END=StackNode<T>()
class Stack: public Container<T> {
public:
	typedef StackNode<T>  Node;
	typedef Iterator<T>  Iter;  //< Interface of the Node
	typedef Container<T>  Cont;
	using Cont::add;
	using Cont::find;
	using Cont::operator[];

	unsigned length() const override  { return _length; }

	Stack()
		: _root(end()), _length(0)  {}
#if __cplusplus >= 201103L
	Stack(Stack&&)=default;
#endif // __cplusplus 11+
	Stack(const Stack&)=default;

#if __cplusplus >= 201103L
	Stack& operator=(Stack&&)=default;
#endif // __cplusplus 11+
	Stack& operator=(const Stack&)=default;
	~Stack();

    //! \brief Add an object to the container
    //! \note The container acquires ownership of the object content
    //!
    //! \param val T&  - an object to be added
    //! \return T*  - acquired object, which holds the ownership of its content
	T* add(T& val) override;

	Node* find(const T& val) override;

	Node* begin() override
		{ return _root; }
	const Node* begin() const override
		{ return _root; }
	Node* end() override
		{ return const_cast<Node*>(_end); }
	const Node* end() const override
		{ return _end; }
private:
	static Node* _end;
	Node* _root;
	unsigned _length;
};

template<typename T>
typename Stack<T>::Node* Stack<T>::_end = Stack<T>::Node::blank();

// Implementation ==============================================================
//// Managed ---------------------------------------------------------------------
//template<typename T>
//Managed<T>::Managed(T& val)(noexcept(T()))
//: T(val), _count(0), _hash(0) {
//
//}
//
//template<typename T>
//Managed<T>::~Managed() {
//	if(--_count)
//		return;
//	// TODO: Delete from the hashmap
//}

// Hashmap ---------------------------------------------------------------------

// Stack -----------------------------------------------------------------------
template<typename T>
Stack<T>::~Stack()
{
	Node* cur = begin();
	while(cur != end()) {
		Node* old = cur;
		cur = cur->next();
		delete old;
	}
}

template<typename T>
T* Stack<T>::add(T& val)
{
	_root = new Node(val, _root);
	if (_root) {
		++_length;
		return &**_root;
	}
	return nullptr;
}

template<typename T>
StackNode<T>* Stack<T>::find(const T& val)
{
	Node* cur = _root;
	while(cur != end() && **cur != val)
		cur = cur->next();
	return cur;
}

}  // smallrdf

#endif  // CONTAINER_HPP_
