/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov: Container interface and Hashmap added
 */

#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

namespace smallrdf {

template<typename T>
class Container {
 public:
  virtual int length() const=0;
  virtual T add(const T& item)=0;
  virtual T get(int index) const=0;
  virtual T operator[](int index) const final  { return get(index); }
};

// List --------------------------------------------------------------------
template<typename T>
class ListNode {
public:
  T item;
  ListNode<T>* next;
};

template<typename T>
class List: Container<T> {
 public:
  int length() const override  { return _length; }

  List();
  ~List();

  T add(const T& item) override;
  T get(int index) const override;

private:
  int _length;
 protected:
  ListNode<T>* _root;
  ListNode<T>* _last;

  ListNode<T>* node(int index) const;
};

template<typename T> List<T>::List()
    : _length(0),
      _root(0),
      _last(0) {
}

template<typename T> List<T>::~List() {
  while (_root) {
    ListNode<T>* cur = _root;
    _root = _root->next;
    delete cur;
  }
}

template<typename T> ListNode<T>* List<T>::node(int index) const {
  int pos = 0;
  ListNode<T>* cur = _root;

  while (pos < index && cur) {
    cur = cur->next;
    pos++;
  }

  if (pos == index) {
    return cur;
  }

  return 0;
}

template<typename T> T List<T>::add(const T& item) {
  ListNode<T>* cur = new ListNode<T>();

  cur->item = item;
  cur->next = 0;

  if (_root) {
    _last->next = cur;
    _last = cur;
  } else {
    _root = _last = cur;
  }

  ++_length;

  return item;
}

template<typename T> T List<T>::get(int index) const {
  ListNode<T>* cur = node(index);

  if (cur) {
    return cur->item;
  } else {
    return 0;
  }
}

}  // smallrdf

#endif  // CONTAINER_HPP_
