/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#include <string.h>  // memcpy
#include <stdlib.h>  // malloc
#include <assert.h>
#include "RDF.hpp"

using namespace smallrdf;


String::String(const char* cstr, bool copy)
	: _data(reinterpret_cast<decltype(_data)>(const_cast<char*>(cstr))),
	  _size(_data ? strlen(cstr) + 1 : 0),
	  _allocated(false)
{
	if (copy) {
		_data = static_cast<decltype(_data)>(malloc(_size));
		if(_data) {
			_allocated = true;
			memcpy(_data, cstr, _size);
		} else _size = 0;
	}
}

String::String(size_t size)
	: _data(size ? static_cast<decltype(_data)>(malloc(size)) : nullptr),
	  _size(_data ? size : 0),
	  _allocated(_data)
{
	if(_size) {
		_data[0] = 0;
		_data[_size-1] = 0;
	}
}

String::String(const uint8_t* buf, size_t size)
	: _data(const_cast<decltype(_data)>(buf)),
	  _size(size),
	  _allocated(false)
{
	// Ensure that the data is null-terminated
	if (_data && _size && _data[_size-1] != 0) {
		//char** writable_data = const_cast<char**>(&this->_data);
		//writable_data = new char[_size]{0};
		_data = static_cast<decltype(_data)>(malloc(++_size));
		if(_data) {
			_allocated = true;
			_data[_size-1] = 0;
			memcpy(_data, buf, _size-1);
		} else _size = 0;
	}
	assert((!_data || _size >= strlen(c_str()) + 1) && "Invalid size of the data");
}

#ifdef ARDUINO
String::String(const AString& str, bool copy)
	: String(str.c_str(), copy)
{
}
#endif  // ARDUINO

String::String(String& other)
	: _data(other._data),
	  _size(other._size),
	  _allocated(other._allocated)
{
	other._allocated = false;
}

String& String::operator=(String&& other)
{
	if(_allocated)
		clear();
	_data = other._data;
	_size = other._size;
	_allocated = other._allocated;
	other._allocated = false;
	return *this;
}

String& String::operator=(const String& other)
{
	if(_allocated)
		clear();
	_data = other._data;
	_size = other._size;
	return *this;
}

String::~String()
{
	clear();
}

void String::clear()
{
	if (_allocated && _data)
		free(_data);
	_data = nullptr;
	_size = 0;
	_allocated = false;
}

void String::swap(String& other)
{
	auto data = _data;
	_data = other._data;
	other._data = data;

	auto size = _size;
	_size = other._size;
	other._size = size;

	auto allocated = _allocated;
	_allocated = other._allocated;
	other._allocated = allocated;
}

bool String::resize(size_t length)
{
	void *ndt;
	if(!_allocated) {
		ndt = malloc(length + 1);
		if(ndt && _size)
			memcpy(ndt, _data, length < _size ? length : _size);
	} else ndt = realloc(_data, length + 1);
	if(ndt) {
		_data = static_cast<decltype(_data)>(ndt);
		_data[length] = 0;
		_size = length + 1;
		_allocated = true;
	}
	return ndt;
}

String* String::release()
{
	if(!_allocated && !resize(length()))
		return nullptr;
	_allocated = true;
	String* res = new String(*this);
	if(res) {
		delete res;
		return nullptr;
	}
	res->_allocated = _allocated;
	_allocated = false;
	return res;
}

String& String::operator+=(const String& other)
{
	size_t offs = length();
	if(resize(length() + other.length()))
		memcpy(_data + offs, other._data, other.length() + 1);  // Copy also the null-terminator
	return *this;
}

bool String::operator==(const String& other) const
{
	if (_size != other._size || !_data ^ !other._data)
		return false;
	return !memcmp(_data, other._data, _size);
}

Term::Term(TermKind tkind, const String& tval)
	: kind(tkind), value(&tval)
{
	//assert(val.allocated() && "Non-allocated values may cause memory leaks");
}

NamedNode::NamedNode(const String& val)
	: Term(RTK_NAMED_NODE, val)
{
}

Literal::Literal(const String& val, const String* lang, const String* dtype)
	: Term(RTK_LITERAL, val),
	  lang(lang),
	  dtype(dtype)
{
}

bool Literal::operator==(const Term& other) const
{
	if(!Term::operator==(other))
		return false;
	const Literal& olit = reinterpret_cast<const Literal&>(other);
	return (lang == olit.lang || (lang && olit.lang && *lang == *olit.lang))
		&& (dtype == olit.dtype || (dtype && olit.dtype && *dtype == *olit.dtype));
}

BlankNode::BlankNode(const String& val)
	: Term(RTK_BLANK_NODE, val)
{
}

Quad::Quad(const Term& subject, const Term& predicate,
           const Term& object, const Term* graph)
	: subject(&subject),
	  predicate(&predicate),
	  object(&object),
	  graph(graph)
{
}

bool Quad::operator==(const Quad& other) const
{
	return (subject == other.subject || *subject == *other.subject)
		&& (predicate == other.predicate || *predicate == *other.predicate)
		&& (object == other.object || *object == *other.object)
		// Note: only the graph might be not defined in a quad
		&& (graph == other.graph || (graph && other.graph && *graph == *other.graph));
}

bool Quad::match(const Term* subj, const Term* pred, const Term* obj, const Term* gr) const
{
	return (!subj || subject == subj || *subject == *subj)
		&& (!pred || predicate == pred || *predicate == *pred)
		&& (!obj || object == obj || *object == *obj)
		// Note: only the graph might be not defined in a quad
		&& (!gr || graph == gr || (graph && *graph == *gr));
}

Quad* Dataset::find(const Quad& quad)
{
	for(auto pit = quads.begin(); pit != quads.end(); pit = pit->next())
		if ((**pit).match(quad.subject, quad.predicate, quad.object, quad.graph))
			return &**pit;
	return nullptr;
}

Dataset::Quads Dataset::match(const Term* subject, const Term* predicate,
                        const Term* object, const Term* graph)
{
	Quads matches;
	for(auto pit = quads.begin(); pit != quads.end(); pit = pit->next())
		if((**pit).match(subject, predicate, object, graph))
			matches.add(**pit);

	return matches;  // Note: Return value optimization is used here
}

const String* Document::string(String& str)
{
	const String* found = findString(str);
	if (found) {
		str = *found;
		return found;
	}
	// Note: acquire() fails only if memory is insufficient
	return str.acquire() ? _strings.add(str) : nullptr;
}

const NamedNode* Document::namedNode(const String& value)
{
	const NamedNode cur(value);
	const Term* found = findTerm(cur);

	if (found)
		return reinterpret_cast<const NamedNode*>(found);
	return reinterpret_cast<const NamedNode*>(_terms.add(cur));
}

const Literal* Document::literal(const String& value,
                                 const String* language,
                                 const String* datatype)
{
	const Literal cur(value, language, datatype);
	const Term* found = findTerm(cur);

	if (found)
		return reinterpret_cast<const Literal*>(found);
	return reinterpret_cast<const Literal*>(_terms.add(cur));
}

const BlankNode* Document::blankNode(const String& value)
{
	const BlankNode cur(value);
	const Term* found = findTerm(cur);

	if (found)
		return reinterpret_cast<const BlankNode*>(found);
	return reinterpret_cast<const BlankNode*>(_terms.add(cur));
}

const Quad* Document::quad(const Term& subject,
                             const Term& predicate,
                             const Term& object,
                             const Term* graph)
{
	return quads.add(Quad(subject, predicate, object, graph));
}

const String* Document::findString(const String& newStr) const
{
	for(auto pit = _strings.begin(); pit != _strings.end(); pit = pit->next())
		if (**pit == newStr)
			return &**pit;

	return nullptr;
}

const Term* Document::findTerm(const Term& newTerm) const
{
	for(auto pit = _terms.begin(); pit != _terms.end(); pit = pit->next())
		if (**pit == newTerm)
			return &**pit;

	return nullptr;
}

// Implementation of C interface ===============================================
// String -------------------------------------------------------------------
String* rdf_string_create(const uint8_t* data, size_t size)
{
	assert(0 && "Not implemented");
	return nullptr;
}

String* rdf_string_create_cstr(const char* str)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_string_release(String* self)
{
	assert(0 && "Not implemented");
}

bool rdf_string_equals(const String* self, const String* other)
{
	assert(0 && "Not implemented");
	return false;
}

// Term ---------------------------------------------------------------------
Term* rdf_term_create(const TermKind termType, const String* value)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_term_release(Term* self)
{
	assert(0 && "Not implemented");
}

bool rdf_term_equals(const Term* self, const Term* other)
{
	assert(0 && "Not implemented");
	return false;
}

// NamedNode ----------------------------------------------------------------
NamedNode* rdf_namednode_create(const String* value)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_namednode_release(NamedNode* self)
{
	assert(0 && "Not implemented");
}

// Literal ------------------------------------------------------------------
Literal* rdf_literal_create_simple(const String* value)
{
	assert(0 && "Not implemented");
	return nullptr;
}

Literal* rdf_literal_create(const String* value, const String* language,
                            const String* datatype)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_literal_release(Literal* self)
{
	assert(0 && "Not implemented");
}

bool rdf_literal_equals(const Term* self, const Term* other)
{
	assert(0 && "Not implemented");
	return false;
}

// BlankNode ----------------------------------------------------------------
BlankNode* rdf_blanknode_create(const String* value)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_blanknode_release(BlankNode* self)
{
	assert(0 && "Not implemented");
}

// Quad ---------------------------------------------------------------------
Quad* rdf_quad_create(const Term* subject, const Term* predicate,
                      const Term* object, const Term* graph)
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_quad_release(Quad* self)
{
	assert(0 && "Not implemented");
}

bool rdf_quad_match_subject(const Quad* self, const Term* subject)
{
	assert(0 && "Not implemented");
	return false;
}

bool rdf_quad_match(const Quad* self, const Term* subject, const Term* predicate,
                    const Term* object, const Term* graph)
{
	assert(0 && "Not implemented");
	return false;
}

// Dataset ------------------------------------------------------------------
Dataset* rdf_dataset_create()
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_dataset_release(Dataset* self)
{
	assert(0 && "Not implemented");
}

const Quad* rdf_dataset_find_subject(const Dataset* self, const Term* subject)
{
	assert(0 && "Not implemented");
	return nullptr;
}

const Quad* rdf_dataset_find(const Dataset* self, const Term* subject,
                             const Term* predicate, const Term* object, const Term* graph)
{
	assert(0 && "Not implemented");
	return nullptr;
}

Dataset* rdf_dataset_match_subject(const Dataset* self, const Term* subject)
{
	assert(0 && "Not implemented");
	return nullptr;
}

Dataset* rdf_dataset_match(const Dataset* self, const Term* subject, const Term* predicate,
                           const Term* object, const Term* graph)
{
	assert(0 && "Not implemented");
	return nullptr;
}

// Document -----------------------------------------------------------------
Document* rdf_document_create()
{
	assert(0 && "Not implemented");
	return nullptr;
}

void rdf_document_release(Document* self)
{
	assert(0 && "Not implemented");
}

const String* rdf_document_rfindString(const Document* self, const String* newStr)
{
	assert(0 && "Not implemented");
	return nullptr;
}

const Term* rdf_document_rfindTerm(const Document* self, const Term* newTerm)
{
	assert(0 && "Not implemented");
	return nullptr;
}
