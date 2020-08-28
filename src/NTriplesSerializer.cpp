/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#include <string.h>

#include "NTriplesSerializer.h"

using namespace smallrdf;


NTriplesSerializer::NTriplesSerializer()
    : _length(0),
      _buf(0),
      _cur(0),
      _end(0) {
}

const uint8_t* NTriplesSerializer::serialize(Dataset* dataset) {
  _length = datasetSize(dataset);
  _buf = _cur = new uint8_t[_length];
  _end = _cur + _length;

  serializeDataset(dataset);

  return _buf;
}

const uint8_t* NTriplesSerializer::serialize_static(Dataset* dataset) {
  NTriplesSerializer serializer;

  return serializer.serialize(dataset);
}

void NTriplesSerializer::write(const char chr) {
  _cur[0] = chr;
  _cur++;
}

void NTriplesSerializer::write(const String* str) {
  memcpy(_cur, str->c_str(), str->length());
  _cur += str->length();
}

size_t NTriplesSerializer::datasetSize(const Dataset* dataset) {
  size_t size = 1; // Reserve for the terminating NULL

  for (int i = 0; i < dataset->quads.length(); i++)
    size += quadSize(dataset->quads.get(i));

  return size;
}

void NTriplesSerializer::serializeDataset(const Dataset* dataset) {
  for (int i = 0; i < dataset->quads.length(); i++)
    serializeQuad(dataset->quads.get(i));

  write(static_cast<char>(0));
}

size_t NTriplesSerializer::quadSize(const Quad* quad) {
  return termSize(quad->subject) + termSize(quad->predicate)
      + termSize(quad->object) + termSize(quad->graph) + 5;
}

void NTriplesSerializer::serializeQuad(const Quad* quad) {
  serializeTerm(quad->subject);
  write(' ');
  serializeTerm(quad->predicate);
  write(' ');
  serializeTerm(quad->object);
  write(' ');

  if (quad->graph) {
    serializeTerm(quad->graph);
    write(' ');
  }

  write('.');
  write('\n');
}

size_t NTriplesSerializer::termSize(const Term* term) {
  if (term == 0)
    return 0;

  switch (term->termType) {
    case RTT_NAMED_NODE:
      return term->value->length() + 2;
    case RTT_LITERAL:
      return literalSize(reinterpret_cast<const Literal*>(term));
    case RTT_BLANK_NODE:
      return term->value->length() + 2;
    default:
      return 0;
  }
}

void NTriplesSerializer::serializeTerm(const Term* term) {
  switch (term->termType) {
    case RTT_NAMED_NODE:
      serializeIri(term->value);
      return;
    case RTT_LITERAL:
      serializeLiteral(reinterpret_cast<const Literal*>(term));
      return;
    case RTT_BLANK_NODE:
      serializeBlankNode(reinterpret_cast<const BlankNode*>(term));
      return;
    default:
      return;
  }
}

size_t NTriplesSerializer::literalSize(const Literal* literal) {
  size_t size = literal->value->length() + 2;

  if (literal->language) {
    size += literal->language->length() + 1;
  } else if (literal->datatype) {
    size += literal->datatype->length() + 2;
  }

  return size;
}

void NTriplesSerializer::serializeLiteral(const Literal* literal) {
  write('"');
  write(literal->value);
  write('"');

  if (literal->language) {
    write('@');
    write(literal->language);
  } else if (literal->datatype) {
    write('^');
    write('^');
    serializeIri(literal->datatype);
  }
}

void NTriplesSerializer::serializeBlankNode(const BlankNode* blankNode) {
  write('_');
  write(':');
  write(blankNode->value);
}

void NTriplesSerializer::serializeIri(const String* iri) {
  write('<');
  write(iri);
  write('>');
}
