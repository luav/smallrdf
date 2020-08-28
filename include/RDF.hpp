//! C++ interface of the Small  library
//! \author Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
//!         (c) 2020 Artem Lutov

#ifndef _HPP_
#define _HPP_

#include  <stdexcept>
#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include "RDF.h"

namespace smallrdf {

using std::logic_error;  // Not Implemented exception

// Implementation of C++ interface =============================================
class String {
 public:
  String();
  explicit String(const char* cstr); //!< Acquire the string poiner

  //! \brief Create String from the provided buffer
  //! The string is allocated if the buffer if is not null-terminated, otherwise the buffer pointer is acquired
  //!
  //! \param buf const uint8_t*  - buffer, which may not have the null-terminator
  //! \param length const size_t  - size of the buffer
  String(const uint8_t* buf, const size_t length);
#ifdef ARDUINO
  explicit String(String str, bool copy = false);
#endif // ARDUINO
  virtual ~String();

  const char* c_str() const  { return _data; };
  const uint8_t* data() const  { return reinterpret_cast<const uint8_t*>(_data); };
  //! \deprecated Replaced with standard-compliant data(). Remained only for the compatibility with the original API until the refactoring completion
  const uint8_t* buffer() const  { return reinterpret_cast<const uint8_t*>(_data); };
  const size_t length() const  { return _size ? _size-1 : 0; }  //!< String length without the null-terminator
  bool equals(const String& other) const;
  bool equals(const String* other) const;

 private:
  const char* _data;  //!< String content
  size_t  _size;  //!<  Size in bytes including the null-terminator
  bool _allocated;  //!< The string data were allocated rather than acquierd
};

class Term {
 public:
  const TermType termType;
  const String* value;

  Term(const TermType termType, const String* value);
  virtual ~Term() {}

  virtual bool equals(const Term* other) const;
};

class NamedNode : public Term {
 public:
  explicit NamedNode(const String* value);
};

class Literal : public Term {
 public:
  const String* language;
  const String* datatype;

  Literal(const String* value, const String* language = nullptr,
             const String* datatype = nullptr);

  virtual bool equals(const Term* other) const;
};

class BlankNode : public Term {
 public:
  explicit BlankNode(const String* value);
};

class Quad {
 public:
  const Term* subject;
  const Term* predicate;
  const Term* object;
  const Term* graph;

  Quad(const Term* subject, const Term* predicate,
          const Term* object, const Term* graph = nullptr);

  const bool match(const Term* subject, const Term* predicate = nullptr,
                   const Term* object = nullptr, const Term* graph = nullptr) const;
};

class Dataset {
 public:
  List<const Quad*> quads;

  virtual ~Dataset();

  const Quad* find(const Term* subject, const Term* predicate = nullptr,
                const Term* object = nullptr, const Term* graph = nullptr);
  Dataset* match(const Term* subject, const Term* predicate = nullptr,
                    const Term* object = nullptr, const Term* graph = nullptr);

 protected:
  List<Dataset*> _datasets;
};

class Document : public Dataset {
 public:
  virtual ~Document();

  const String* string(const char* buf);
  const String* string(const uint8_t* buf, const size_t length);
  const NamedNode* namedNode(const String* value);
  const Literal* literal(const String* value, const String* language = nullptr,
                            const String* datatype = nullptr);
  const BlankNode* blankNode(const String* value);
  const Quad* triple(const Term* subject, const Term* predicate,
                      const Term* object, const Term* graph = nullptr);
  Dataset* dataset();

#if defined(ARDUINO)
  const String* string(String str, bool copy = false);
#endif

 protected:
  List<String*> _strings;
  List<Term*> _terms;

  const String* findString(const String* newStr) const;
  const Term* findTerm(const Term* newTerm) const;
};

}  // smallrdf

#endif  // _HPP_
