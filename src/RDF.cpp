/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#include <string.h>
#include <assert.h>
#include "RDF.hpp"

using namespace smallrdf;


String::String()
: String(nullptr) {
}

String::String(const char* buf)
: _data(buf),
  _size(buf ? strlen(buf) + 1 : 0),
  _allocated(false) {
}

String::String(const uint8_t* buf, size_t size)
: _data(reinterpret_cast<const char*>(buf)),
  _size(size),
  _allocated(false) {
  // Ensure that the data is null-terminated
  if (_data && _size && _data[_size-1] != 0) {
    //char** writable_data = const_cast<char**>(&this->_data);
    //writable_data = new char[_size];
    _data = new const char[++_size]{0};
    _allocated = true;
    memcpy(const_cast<char*>(_data), buf, _size-1);
  }
  assert((!_data || _size >= strlen(_data) + 1) && "Invalid size of the data");
}

#ifdef ARDUINO
String::String(String str, bool copy)
: String(str.c_str())
 {
  if (copy) {
    _data = new const char[_size]{0};
    memcpy(const_cast<char*>(_data), str.c_str(), _size-1);
    _allocated = true;
  }
}
#endif  // ARDUINO

String::~String() {
  if (_allocated && _data) {
    _allocated = false;
    delete[] _data;
  }
}

bool String::equals(const String& other) const {
  if (_size != other._size || !_data ^ !other._data)
    return false;
  return !memcmp(_data, other._data, _size);
}

bool String::equals(const String* other) const {
  if (this == other)
    return true;
  else if (!other)
    return false;
  return equals(*other);
}

Term::Term(const TermType termType, const String* value)
: termType(termType),
  value(value) {
}

bool Term::equals(const Term* other) const {
  if (this == other) {
    return true;
  }

  return termType == other->termType && value->equals(other->value);
}

NamedNode::NamedNode(const String* value)
: Term(RTT_NAMED_NODE, value) {
}

Literal::Literal(const String* value, const String* language,
                       const String* datatype)
: Term(RTT_LITERAL, value),
  language(language),
  datatype(datatype) {
}

bool Literal::equals(const Term* other) const {
  if (this == other) {
    return true;
  }

  if (other->termType != RTT_LITERAL) {
    return false;
  }

  const Literal* otherLiteral = reinterpret_cast<const Literal*>(other);

  return value->equals(otherLiteral->value)
      && language->equals(otherLiteral->language)
      && datatype->equals(otherLiteral->datatype);
}

BlankNode::BlankNode(const String* value)
: Term(RTT_BLANK_NODE, value) {
}

Quad::Quad(const Term* subject, const Term* predicate,
                 const Term* object, const Term* graph)
: subject(subject),
  predicate(predicate),
  object(object),
  graph(graph) {
}

const bool Quad::match(const Term* subject, const Term* predicate,
                          const Term* object, const Term* graph) const {
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

Dataset::~Dataset() {
  for (int i = 0; i < _datasets.length(); ++i)
    delete _datasets.get(i);
}

const Quad* Dataset::find(const Term* subject,
                                const Term* predicate, const Term* object,
                                const Term* graph) {
  for (int i = 0; i < quads.length(); i++) {
    const Quad* quad = quads.get(i);

    if (quad->match(subject, predicate, object, graph))
      return quad;
  }

  return NULL;
}

Dataset* Dataset::match(const Term* subject, const Term* predicate,
                              const Term* object, const Term* graph) {
  Dataset* matches = _datasets.add(new Dataset());

  for (int i = 0; i < quads.length(); i++) {
    const Quad* quad = quads.get(i);

    if (quad->match(subject, predicate, object, graph))
      matches->quads.add(quad);
  }

  return matches;
}

Document::~Document() {
  for (int i = 0; i < quads.length(); i++)
    delete quads.get(i);

  for (int i = 0; i < _terms.length(); i++)
    delete _terms.get(i);

  for (int i = 0; i < _strings.length(); i++)
    delete _strings.get(i);
}

const String* Document::string(const char* buf) {
  const String cur(buf);
  const String* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<String*>(_strings.add(new String(buf)));
}

const String* Document::string(const uint8_t* buf, const size_t length) {
  const String cur(buf, length);
  const String* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<String*>(_strings.add(new String(buf, length)));
}

const NamedNode* Document::namedNode(const String* value) {
  const NamedNode cur(value);
  const Term* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const NamedNode*>(found);
  }

  return reinterpret_cast<const NamedNode*>(_terms.add(
      new NamedNode(value)));
}

const Literal* Document::literal(const String* value,
                                       const String* language,
                                       const String* datatype) {
  const Literal cur(value, language, datatype);
  const Term* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const Literal*>(found);
  }

  return reinterpret_cast<const Literal*>(_terms.add(
      new Literal(value, language, datatype)));
}

const BlankNode* Document::blankNode(const String* value) {
  const BlankNode cur(value);
  const Term* found = findTerm(&cur);

  if (found != 0) {
    return reinterpret_cast<const BlankNode*>(found);
  }

  return reinterpret_cast<const BlankNode*>(_terms.add(
      new BlankNode(value)));
}

const Quad* Document::triple(const Term* subject,
                                   const Term* predicate,
                                   const Term* object,
                                   const Term* graph) {
  return quads.add(new Quad(subject, predicate, object, graph));
}

Dataset* Document::dataset() {
  return _datasets.add(new Dataset());
}

const String* Document::findString(const String* newStr) const {
  for (int i = 0; i < _strings.length(); i++) {
    String* cur = _strings.get(i);

    if (cur->equals(newStr))
      return cur;
  }

  return NULL;
}

const Term* Document::findTerm(const Term* newTerm) const {
  for (int i = 0; i < _terms.length(); i++) {
    Term* cur = _terms.get(i);

    if (cur->equals(newTerm))
      return cur;
  }

  return NULL;
}

#ifdef ARDUINO
const String* Document::string(String str, bool copy) {
  const String cur(str, copy);
  const String* found = findString(&cur);

  if (found != 0) {
    return found;
  }

  return reinterpret_cast<String*>(_strings.add(new String(str, copy)));
}
#endif // ARDUINO

// Implementation of C interface ===============================================
// String -------------------------------------------------------------------
String* rdf_string_create(const uint8_t* data, const size_t size) {
  throw logic_error("Not implemented");
  return nullptr;
}

String* rdf_string_create_cstr(const char* str) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_string_release(String* self) {
  throw logic_error("Not implemented");
}

bool rdf_string_equals(const String* self, const String* other) {
  throw logic_error("Not implemented");
  return false;
}

// Term ---------------------------------------------------------------------
Term* rdf_term_create(const TermType termType, const String* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_term_release(Term* self) {
  throw logic_error("Not implemented");
}

bool rdf_term_equals(const Term* self, const Term* other) {
  throw logic_error("Not implemented");
  return false;
}

// NamedNode ----------------------------------------------------------------
NamedNode* rdf_namednode_create(const String* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_namednode_release(NamedNode* self) {
  throw logic_error("Not implemented");
}

// Literal ------------------------------------------------------------------
Literal* rdf_literal_create_simple(const String* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

Literal* rdf_literal_create(const String* value, const String* language,
                              const String* datatype) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_literal_release(Literal* self) {
  throw logic_error("Not implemented");
}

bool rdf_literal_equals(const Term* self, const Term* other) {
  throw logic_error("Not implemented");
  return false;
}

// BlankNode ----------------------------------------------------------------
BlankNode* rdf_blanknode_create(const String* value) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_blanknode_release(BlankNode* self) {
  throw logic_error("Not implemented");
}

// Quad ---------------------------------------------------------------------
Quad* rdf_quad_create(const Term* subject, const Term* predicate,
          const Term* object, const Term* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_quad_release(Quad* self) {
  throw logic_error("Not implemented");
}

bool rdf_quad_match_subject(const Quad* self, const Term* subject) {
  throw logic_error("Not implemented");
  return false;
}

bool rdf_quad_match(const Quad* self, const Term* subject, const Term* predicate,
                   const Term* object, const Term* graph) {
  throw logic_error("Not implemented");
  return false;
}

// Dataset ------------------------------------------------------------------
Dataset* rdf_dataset_create() {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_dataset_release(Dataset* self) {
  throw logic_error("Not implemented");
}

const Quad* rdf_dataset_find_subject(const Dataset* self, const Term* subject) {
  throw logic_error("Not implemented");
  return nullptr;
}

const Quad* rdf_dataset_find(const Dataset* self, const Term* subject,
                               const Term* predicate, const Term* object, const Term* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

Dataset* rdf_dataset_match_subject(const Dataset* self, const Term* subject) {
  throw logic_error("Not implemented");
  return nullptr;
}

Dataset* rdf_dataset_match(const Dataset* self, const Term* subject, const Term* predicate,
                             const Term* object, const Term* graph) {
  throw logic_error("Not implemented");
  return nullptr;
}

// Document -----------------------------------------------------------------
Document* rdf_document_create() {
  throw logic_error("Not implemented");
  return nullptr;
}

void rdf_document_release(Document* self) {
  throw logic_error("Not implemented");
}

const String* rdf_document_rfindString(const Document* self, const String* newStr) {
  throw logic_error("Not implemented");
  return nullptr;
}

const Term* rdf_document_rfindTerm(const Document* self, const Term* newTerm) {
  throw logic_error("Not implemented");
  return nullptr;
}
