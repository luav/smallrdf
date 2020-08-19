//! C++ interface of the Small RDF library
//! \author Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
//!         (c) 2020 Artem Lutov

#ifndef RDF_HPP_
#define RDF_HPP_

#include  <stdexcept>
#if defined(ARDUINO)
#include <Arduino.h>
#endif

#include "RDF.h"

using std::logic_error;  // Not Implemented exception


// Implementation of C++ interface =============================================
class RDFString {
 public:
  RDFString();
  explicit RDFString(const char* cstr); //!< Acquire the string poiner

  //! \brief Create RDFString from the provided buffer
  //! The string is allocated if the buffer if is not null-terminated, otherwise the buffer pointer is acquired
  //!
  //! \param buf const uint8_t*  - buffer, which may not have the null-terminator
  //! \param length const size_t  - size of the buffer
  RDFString(const uint8_t* buf, const size_t length);
#ifdef ARDUINO
  explicit RDFString(String str, bool copy = false);
#endif // ARDUINO
  virtual ~RDFString();

  const char* c_str() const  { return _data; };
  const uint8_t* data() const  { return reinterpret_cast<const uint8_t*>(_data); };
  //! \deprecated Replaced with standard-compliant data(). Remained only for the compatibility with the original API until the refactoring completion
  const uint8_t* buffer() const  { return reinterpret_cast<const uint8_t*>(_data); };
  const size_t length() const  { return _size ? _size-1 : 0; }  //!< String length without the null-terminator
  bool equals(const RDFString& other) const;
  bool equals(const RDFString* other) const;

 private:
  const char* _data;  //!< String content
  size_t  _size;  //!<  Size in bytes including the null-terminator
  bool _allocated;  //!< The string data were allocated rather than acquierd
};

class RDFTerm {
 public:
  const RDFTermType termType;
  const RDFString* value;

  RDFTerm(const RDFTermType termType, const RDFString* value);
  virtual ~RDFTerm() {}

  virtual bool equals(const RDFTerm* other) const;
};

class RDFNamedNode : public RDFTerm {
 public:
  explicit RDFNamedNode(const RDFString* value);
};

class RDFLiteral : public RDFTerm {
 public:
  const RDFString* language;
  const RDFString* datatype;

  RDFLiteral(const RDFString* value, const RDFString* language = nullptr,
             const RDFString* datatype = nullptr);

  virtual bool equals(const RDFTerm* other) const;
};

class RDFBlankNode : public RDFTerm {
 public:
  explicit RDFBlankNode(const RDFString* value);
};

class RDFQuad {
 public:
  const RDFTerm* subject;
  const RDFTerm* predicate;
  const RDFTerm* object;
  const RDFTerm* graph;

  RDFQuad(const RDFTerm* subject, const RDFTerm* predicate,
          const RDFTerm* object, const RDFTerm* graph = nullptr);

  const bool match(const RDFTerm* subject, const RDFTerm* predicate = nullptr,
                   const RDFTerm* object = nullptr, const RDFTerm* graph = nullptr) const;
};

class RDFDataset {
 public:
  RDFList<const RDFQuad*> quads;

  virtual ~RDFDataset();

  const RDFQuad* find(const RDFTerm* subject, const RDFTerm* predicate = nullptr,
                const RDFTerm* object = nullptr, const RDFTerm* graph = nullptr);
  RDFDataset* match(const RDFTerm* subject, const RDFTerm* predicate = nullptr,
                    const RDFTerm* object = nullptr, const RDFTerm* graph = nullptr);

 protected:
  RDFList<RDFDataset*> _datasets;
};

class RDFDocument : public RDFDataset {
 public:
  virtual ~RDFDocument();

  const RDFString* string(const char* buf);
  const RDFString* string(const uint8_t* buf, const size_t length);
  const RDFNamedNode* namedNode(const RDFString* value);
  const RDFLiteral* literal(const RDFString* value, const RDFString* language = nullptr,
                            const RDFString* datatype = nullptr);
  const RDFBlankNode* blankNode(const RDFString* value);
  const RDFQuad* triple(const RDFTerm* subject, const RDFTerm* predicate,
                      const RDFTerm* object, const RDFTerm* graph = nullptr);
  RDFDataset* dataset();

#if defined(ARDUINO)
  const RDFString* string(String str, bool copy = false);
#endif

 protected:
  RDFList<RDFString*> _strings;
  RDFList<RDFTerm*> _terms;

  const RDFString* findString(const RDFString* newStr) const;
  const RDFTerm* findTerm(const RDFTerm* newTerm) const;
};

#endif  // RDF_HPP_
