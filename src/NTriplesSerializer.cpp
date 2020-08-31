/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#include <string.h>

#include "NTriplesSerializer.h"

using namespace smallrdf;


NTriplesSerializer::NTriplesSerializer()
	: _buf(new String()),
	  _cur(_buf ? _buf->data() : nullptr),
	  _end(_buf ? _buf->data() : nullptr)
{
}

NTriplesSerializer::NTriplesSerializer(NTriplesSerializer&& other)
	: _buf(other._buf),
	  _cur(other._cur),
	  _end(other._end)
{
	other._buf = new String();
	other._cur = other._end = nullptr;
}

NTriplesSerializer::NTriplesSerializer(String*& storage)
	: _buf(storage ? storage : new String()),
	  _cur(_buf->data() + _buf->length()),
	  _end(_buf->data() + _buf->length())
{
	storage = nullptr;  // Invalidate the pointer to insure self-sufficiency of the internal data
}

NTriplesSerializer::~NTriplesSerializer()
{
	_end = _cur = nullptr;
	if(_buf)
		delete _buf;
	_buf = nullptr;
}

String* NTriplesSerializer::release()
{
	String *res = _buf;
	_buf = new String();  // Reset the internal state to insure its self-sufficiency
	_end = _cur = nullptr;
	return res;
}

String& NTriplesSerializer::storage()
{
	assert(_buf && "Internal buffer should be initialized");
	return *_buf;
}

String& NTriplesSerializer::serialize(const Dataset& dataset)
{
	assert(_buf && "Internal buffer should be initialized");
	size_t dsize = datasetSize(dataset);
	size_t offs = 0;
	if(_cur) {
		assert(_cur >= _buf->data() && "Serialization position is invalid");
		offs = _cur - _buf->data();
		dsize += offs;
	}
	if(_buf->length() < dsize)
		_buf->resize(dsize);
	_cur = _buf->data() + offs;
	_end = _buf->data() + _buf->length();

	serializeDataset(dataset);

	return *_buf;
}

String& NTriplesSerializer::serialize(const Dataset& dataset, String*& storage)
{
	NTriplesSerializer serializer(storage);
	serializer.serialize(dataset);
	return *(storage = serializer.release());
}

void NTriplesSerializer::write(uint8_t chr)
{
	_cur[0] = chr;
	_cur++;
}

void NTriplesSerializer::write(const String& str)
{
	memcpy(_cur, str.c_str(), str.length());
	_cur += str.length();
}

size_t NTriplesSerializer::datasetSize(const Dataset& dataset) const
{
	size_t size = 0;
	for(auto piq = dataset.quads.begin(); piq != dataset.quads.end(); piq = piq->next())
		size += quadSize(**piq);
	return size;
}

void NTriplesSerializer::serializeDataset(const Dataset& dataset)
{
	for(auto piq = dataset.quads.begin(); piq != dataset.quads.end(); piq = piq->next())
		serializeQuad(**piq);
	write(0);
}

size_t NTriplesSerializer::quadSize(const Quad& quad) const
{
	return termSize(quad.subject) + termSize(quad.predicate)
	       + termSize(quad.object) + termSize(quad.graph) + 5;
}

void NTriplesSerializer::serializeQuad(const Quad& quad)
{
	serializeTerm(quad.subject);
	write(' ');
	serializeTerm(quad.predicate);
	write(' ');
	serializeTerm(quad.object);
	write(' ');

	if (quad.graph) {
		serializeTerm(quad.graph);
		write(' ');
	}

	write('.');
	write('\n');
}

size_t NTriplesSerializer::termSize(const Term* term) const
{
	if(!term)
		return 0;
	switch (term->kind) {
	case RTK_NAMED_NODE:
		return term->value->length() + 2;
	case RTK_LITERAL:
		return literalSize(reinterpret_cast<const Literal&>(*term));
	case RTK_BLANK_NODE:
		return term->value->length() + 2;
	case RTK_VARIABLE:
		// TODO: implement
		assert(0 && "RTK_VARIABLE term size handling is not implemented");
	default:
		return 0;
	}
}

void NTriplesSerializer::serializeTerm(const Term* term)
{
	if(!term)
		return;
	switch (term->kind) {
	case RTK_NAMED_NODE:
		serializeIri(term->value);
		return;
	case RTK_LITERAL:
		serializeLiteral(reinterpret_cast<const Literal&>(*term));
		return;
	case RTK_BLANK_NODE:
		serializeBlankNode(reinterpret_cast<const BlankNode&>(*term));
		return;
	case RTK_VARIABLE:
		// TODO: implement
		assert(0 && "RTK_VARIABLE term size handling is not implemented");
	default:
		return;
	}
}

size_t NTriplesSerializer::literalSize(const Literal& literal) const
{
	size_t size = literal.value->length() + 2;

	if (literal.lang) {
		size += literal.lang->length() + 1;
	} else if (literal.dtype) {
		size += literal.dtype->length() + 2;
	}

	return size;
}

void NTriplesSerializer::serializeLiteral(const Literal& literal)
{
	write('"');
	write(*literal.value);
	write('"');

	if (literal.lang) {
		write('@');
		write(*literal.lang);
	} else if (literal.dtype) {
		write('^');
		write('^');
		serializeIri(literal.dtype);
	}
}

void NTriplesSerializer::serializeBlankNode(const BlankNode& blankNode)
{
	write('_');
	write(':');
	write(*blankNode.value);
}

void NTriplesSerializer::serializeIri(const String* iri)
{
	assert(iri && "Null pointer to string");
	write('<');
	write(*iri);
	write('>');
}
