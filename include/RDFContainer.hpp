/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov: RDFContainer interface and RDFHashmap added
 */

#ifndef RDFCONTAINER_HPP_
#define RDFCONTAINER_HPP_

template<typename T>
class RDFContainer {
 public:
  virtual int length() const=0;
  virtual T add(const T& item)=0;
  virtual T get(int index) const=0;
  virtual T operator[](int index) const final  { return get(index); }
};

// RDFList --------------------------------------------------------------------
template<typename T>
class RDFListNode {
public:
  T item;
  RDFListNode<T>* next;
};

template<typename T>
class RDFList: RDFContainer<T> {
 public:
  int length() const override  { return _length; }

  RDFList();
  ~RDFList();

  T add(const T& item) override;
  T get(int index) const override;

private:
  int _length;
 protected:
  RDFListNode<T>* _root;
  RDFListNode<T>* _last;

  RDFListNode<T>* node(int index) const;
};

template<typename T> RDFList<T>::RDFList()
    : _length(0),
      _root(0),
      _last(0) {
}

template<typename T> RDFList<T>::~RDFList() {
  while (_root) {
    RDFListNode<T>* cur = _root;
    _root = _root->next;
    delete cur;
  }
}

template<typename T> RDFListNode<T>* RDFList<T>::node(int index) const {
  int pos = 0;
  RDFListNode<T>* cur = _root;

  while (pos < index && cur) {
    cur = cur->next;
    pos++;
  }

  if (pos == index) {
    return cur;
  }

  return 0;
}

template<typename T> T RDFList<T>::add(const T& item) {
  RDFListNode<T>* cur = new RDFListNode<T>();

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

template<typename T> T RDFList<T>::get(int index) const {
  RDFListNode<T>* cur = node(index);

  if (cur) {
    return cur->item;
  } else {
    return 0;
  }
}

#endif  // RDFCONTAINER_HPP_
