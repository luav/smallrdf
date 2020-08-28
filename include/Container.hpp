/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov: Container interface and Hashmap added
 */

#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

//#include <stdexcept>

namespace smallrdf {

//using std::out_of_range;

template<typename T>
class Container {
public:
	virtual unsigned length() const noexcept=0;
	virtual T& add(const T& item) noexcept=0;

    //! \brief Fetch node by index
    //!
    //! \param index int  - index, a negative value is treated as index from the end
    //! \return ListNode<T>*  - resulting node
	virtual T& get(int index)=0;
	virtual const T& get(int index) const=0;
	virtual T& operator[](int index) final  { return get(index); }
	virtual const T& operator[](int index) const final  { return get(index); }
};

// List --------------------------------------------------------------------
template<typename T>
struct ListNode {
	ListNode(const T& val): item(val), next(nullptr)  {}

	T item;
	ListNode<T>* next;
};

template<typename T>
class List: Container<T> {
public:
	unsigned length() const noexcept override  { return _length; }

	List() noexcept;
	~List();

	T& add(const T& item) noexcept override;
	T& get(int index) override;
	const T& get(int index) const override;
private:
	unsigned _length;
protected:
	ListNode<T>* _root;
	ListNode<T>* _last;

    //! \brief Fetch node by index
    //!
    //! \param index int  - index, a negative value is treated as index from the end
    //! \return ListNode<T>*  - resulting node
	ListNode<T>* node(int index) const noexcept;
};

template<typename T> List<T>::List() noexcept
: _length(0), _root(nullptr), _last(nullptr) {
}

template<typename T> List<T>::~List() {
	while (_root) {
	  ListNode<T>* cur = _root;
	  _root = _root->next;
	  delete cur;
	}
}

template<typename T> ListNode<T>* List<T>::node(int index) const noexcept {
	ListNode<T>* cur = _root;

	if(index < 0)
		index += _length;
	while (index-- && cur)
	  cur = cur->next;
	return cur;
}

template<typename T> T& List<T>::add(const T& item) noexcept {
	ListNode<T>* cur = new ListNode<T>(item);

	if (_root) {
	  _last->next = cur;
	  _last = cur;
	}
	else _root = _last = cur;
	++_length;

	return *cur;
}

template<typename T> T& List<T>::get(int index) {
	ListNode<T>* cur = node(index);

//	// Note: the app is crashed if cur = nullptr exception is not handled,
//	// but this handling affects performance.
//	if (!cur)
//		throw out_of_range("The index is out of range");
	return cur->item;
}

}  // smallrdf

#endif  // CONTAINER_HPP_
