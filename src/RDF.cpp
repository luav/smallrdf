/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#include <string.h>
#include <assert.h>

#include "RDF.hpp"


RDFString::RDFString()
: RDFString(nullptr) {
}

RDFString::RDFString(const char* buf)
: _data(buf),
  _size(buf ? strlen(buf) + 1 : 0),
  _allocated(false) {
}

RDFString::RDFString(const uint8_t* buf, size_t size)
: _data(reinterpret_cast<const char*>(buf)),
  _size(size),
  _allocated(false) {
  // Ensure that the data is null-terminated
  if (_data && _size && _data[_size-1] != 0) {
    //char** writable_data = const_cast<char**>(&this->_data);
    //writable_data = new char[_size];
    ++_size;
    _data = new const char[_size]{0};
    memcpy(const_cast<char*>(_data), buf, _size-1);
    _allocated = true;
  }
  assert((!_data || _size >= strlen(_data) + 1) && "Invalid size of the data");
}

#ifdef ARDUINO
RDFString::RDFString(String str, bool copy)
: RDFString(str.c_str())
 {
  if (copy) {
    _data = new const char[_size]{0};
    memcpy(const_cast<char*>(_data), str.c_str(), _size-1);
    _allocated = true;
  }
}
#endif  // ARDUINO

RDFString::~RDFString() {
  if (_allocated && _data) {
    _allocated = false;
    delete[] _data;
  }
}

bool RDFString::equals(const RDFString& other) const {
  if (_size != other._size)
    return false;
  if ((!_data || !other._data) && _data != other._data)
    return false;

  return !memcmp(_data, other._data, _size);
}

bool RDFString::equals(const RDFString* other) const {
  if (this == other)
    return true;
  else if (!other)
    return false;
  return equals(*other);
}

RDFTerm::RDFTerm(const RDFTermType termType, const RDFString* value)
: termType(termType),
  value(value) {
}

bool RDFTerm::equals(const RDFTerm* other) const {
  if (this == other) {
    return true;
  }

  return termType == other->termType && value->equals(other->value);
}

RDFNamedNode::RDFNamedNode(const RDFString* value)
: RDFTerm(RDF_NAMED_NODE, value) {
}

RDFLiteral::RDFLiteral(const RDFString* value, const RDFString* language,
                       const RDFString* datatype)
: RDFTerm(RDF_LITERAL, value),
  language(language),
  datatype(datatype) {
}

bool RDFLiteral::equals(const RDFTerm* other) const {
  if (this == other) {
    return true;
  }

  if (other->termType != RDF_LITERAL) {
    return false;
  }

  const RDFLiteral* otherLiteral = reinterpret_cast<const RDFLiteral*>(other);

  return value->equals(otherLiteral->value)
      && language->equals(otherLiteral->language)
      && datatype->equals(otherLiteral->datatype);
}

RDFBlankNode::RDFBlankNode(const RDFString* value)
: RDFTerm(RDF_BLANK_NODE, value) {
}

RDFQuad::RDFQuad(const RDFTerm* subject, const RDFTerm* predicate,
                 const RDFTerm* object, const RDFTerm* graph)
: subject(subject),
  predicate(predicate),
  object(object),
  graph(graph) {
}

const bool RDFQuad::match(const RDFTerm* subject, const RDFTerm* predicate,
                          const RDFTerm* object, const RDFTerm* graph) const {
  if (subject && !this->subject->equals(subject)) {
    return false;
  }

  if (predicate && !this->predicate->equals(predicate)) {
    return false;
  }

  if (object && !this->object->equals(object)) {
    return false;
  }

  if (graph && (!this->graph || !this->graph->equals(graph))) {
    return false;
  }

  return true;
}

RDFDataset::~RDFDataset() {
  for (int i = 0; i < _datasets.length(); ++i)
    delete _datasets.get(i);
}

const RDFQuad* RDFDataset::find(const RDFTerm* subject,
                                const RDFTerm* predicate, const RDFTerm* object,
                                const RDFTerm* graph) {
  for (int i = 0; i < quads.length(); i++) {
    const RDFQuad* quad = quads.get(i);

    if (quad->match(subject, predicate, object, graph))
      return quad;
  }

  return NULL;
}

RDFDataset* RDFDataset::match(const RDFTerm* subject, const RDFTerm* predicate,
                              const RDFTerm* object, const RDFTerm* graph) {
  RDFDataset* matches = _datasets.add(new RDFDataset());

  for (int i = 0; i < quads.length(); i++) {
    const RDFQuad* quad = quads.get(i);

    if (quad->match(subject, predicate, object, graph))
      matches->quads.add(quad);
  }

  return matches;
}

RDFDocument::~RDFDocument() {
  for (int i = 0; i < quads.length(); i++)
    delete quads.get(i);

  for (int i = 0; i < _terms.length(); i++)
    delete _terms.get(i);

  for (int i = 0; i < _strings.length(); i++)
    delete _strings.get(i);
}

const RDFString* RDFDocument::string(const char* buf) {
  const RDFString cur(buf);
  const RDFString* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<RDFString*>(_strings.add(new RDFString(buf)));
}

const RDFString* RDFDocument::string(const uint8_t* buf, const size_t length) {
  const RDFString cur(buf, length);
  const RDFString* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<RDFString*>(_strings.add(new RDFString(buf, length)));
}

const RDFNamedNode* RDFDocument::namedNode(const RDFString* value) {
  const RDFNamedNode cur(value);
  const RDFTerm* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const RDFNamedNode*>(found);
  }

  return reinterpret_cast<const RDFNamedNode*>(_terms.add(
      new RDFNamedNode(value)));
}

const RDFLiteral* RDFDocument::literal(const RDFString* value,
                                       const RDFString* language,
                                       const RDFString* datatype) {
  const RDFLiteral cur(value, language, datatype);
  const RDFTerm* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const RDFLiteral*>(found);
  }

  return reinterpret_cast<const RDFLiteral*>(_terms.add(
      new RDFLiteral(value, language, datatype)));
}

const RDFBlankNode* RDFDocument::blankNode(const RDFString* value) {
  const RDFBlankNode cur(value);
  const RDFTerm* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const RDFBlankNode*>(found);
  }

  return reinterpret_cast<const RDFBlankNode*>(_terms.add(
      new RDFBlankNode(value)));
}

const RDFQuad* RDFDocument::triple(const RDFTerm* subject,
                                   const RDFTerm* predicate,
                                   const RDFTerm* object,
                                   const RDFTerm* graph) {
  return quads.add(new RDFQuad(subject, predicate, object, graph));
}

RDFDataset* RDFDocument::dataset() {
  return _datasets.add(new RDFDataset());
}

const RDFString* RDFDocument::findString(const RDFString* newStr) const {
  for (int i = 0; i < _strings.length(); i++) {
    RDFString* cur = _strings.get(i);

    if (cur->equals(newStr))
      return cur;
  }

  return NULL;
}

const RDFTerm* RDFDocument::findTerm(const RDFTerm* newTerm) const {
  for (int i = 0; i < _terms.length(); i++) {
    RDFTerm* cur = _terms.get(i);

    if (cur->equals(newTerm))
      return cur;
  }

  return NULL;
}

#ifdef ARDUINO
const RDFString* RDFDocument::string(String str, bool copy) {
  const RDFString cur(str, copy);
  const RDFString* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<RDFString*>(_strings.add(new RDFString(str, copy)));
}
#endif // ARDUINO

// Implementation of C interface ===============================================
// RDFString -------------------------------------------------------------------
RDFString* RDFString_create(const uint8_t* data, const size_t size) {
  throw logic_error("Not implemented");
  return nullptr;
}

RDFString* RDFString_create_str(const char* str) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFString_release(RDFString* self) {
  throw logic_error("Not implemented");
}

bool RDFString_equals(const RDFString* self, const RDFString* other) {
  throw logic_error("Not implemented");
  return false;
}

// RDFTerm ---------------------------------------------------------------------
RDFTerm* RDFTerm_create(const RDFTermType termType, const RDFString* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFTerm_release(RDFTerm* self) {
  throw logic_error("Not implemented");
}

bool RDFTerm_equals(const RDFTerm* self, const RDFTerm* other) {
  throw logic_error("Not implemented");
  return false;
}

// RDFNamedNode ----------------------------------------------------------------
RDFNamedNode* RDFNamedNode_create(const RDFString* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFNamedNode_release(RDFNamedNode* self) {
  throw logic_error("Not implemented");
}

// RDFLiteral ------------------------------------------------------------------
RDFLiteral* RDFLiteral_create_simple(const RDFString* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

RDFLiteral* RDFLiteral_create(const RDFString* value, const RDFString* language,
                              const RDFString* datatype) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFLiteral_release(RDFLiteral* self) {
  throw logic_error("Not implemented");
}

bool RDFLiteral_equals(const RDFTerm* self, const RDFTerm* other) {
  throw logic_error("Not implemented");
  return false;
}

// RDFBlankNode ----------------------------------------------------------------
RDFBlankNode* RDFBlankNode_create(const RDFString* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFBlankNode_release(RDFBlankNode* self) {
  throw logic_error("Not implemented");
}

// RDFQuad ---------------------------------------------------------------------
RDFQuad* RDFQuad_create(const RDFTerm* subject, const RDFTerm* predicate,
          const RDFTerm* object, const RDFTerm* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFQuad_release(RDFQuad* self) {
  throw logic_error("Not implemented");
}

bool RDFQuad_match_subject(const RDFQuad* self, const RDFTerm* subject) {
  throw logic_error("Not implemented");
  return false;
}

bool RDFQuad_match(const RDFQuad* self, const RDFTerm* subject, const RDFTerm* predicate,
                   const RDFTerm* object, const RDFTerm* graph) {
  throw logic_error("Not implemented");
  return false;
}

// RDFDataset ------------------------------------------------------------------
RDFDataset* RDFDataset_create() {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFDataset_release(RDFDataset* self) {
  throw logic_error("Not implemented");
}

const RDFQuad* RDFDataset_find_subject(const RDFDataset* self, const RDFTerm* subject) {
  throw logic_error("Not implemented");
  return nullptr;
}

const RDFQuad* RDFDataset_find(const RDFDataset* self, const RDFTerm* subject,
                               const RDFTerm* predicate, const RDFTerm* object, const RDFTerm* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

RDFDataset* RDFDataset_match_subject(const RDFDataset* self, const RDFTerm* subject) {
  throw logic_error("Not implemented");
  return nullptr;
}

RDFDataset* RDFDataset_match(const RDFDataset* self, const RDFTerm* subject, const RDFTerm* predicate,
                             const RDFTerm* object, const RDFTerm* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

// RDFDocument -----------------------------------------------------------------
RDFDocument* RDFDocument_create() {
  throw logic_error("Not implemented");
  return nullptr;
}

void RDFDocument_release(RDFDocument* self) {
  throw logic_error("Not implemented");
}

const RDFString* RDFDocument_rfindString(const RDFDocument* self, const RDFString* newStr) {
  throw logic_error("Not implemented");
  return nullptr;
}

const RDFTerm* RDFDocument_rfindTerm(const RDFDocument* self, const RDFTerm* newTerm) {
  throw logic_error("Not implemented");
  return nullptr;
}
