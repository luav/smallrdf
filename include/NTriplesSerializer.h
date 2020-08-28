/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#ifndef NTRIPLESSERIALIZER_H_
#define NTRIPLESSERIALIZER_H_

#include "RDF.hpp"

namespace smallrdf {

class NTriplesSerializer {
 public:
  NTriplesSerializer();
  const uint8_t* serialize(Dataset* dataset);

  static const uint8_t* serialize_static(Dataset* dataset);

 protected:
  size_t _length;
  uint8_t* _buf;
  uint8_t* _cur;
  uint8_t* _end;

  void write(const char chr);
  void write(const String* str);

  size_t datasetSize(const Dataset* dataset);
  void serializeDataset(const Dataset* dataset);
  size_t quadSize(const Quad* quad);
  void serializeQuad(const Quad* quad);
  size_t termSize(const Term* term);
  void serializeTerm(const Term* term);
  size_t literalSize(const Literal* literal);
  void serializeLiteral(const Literal* literal);
  void serializeBlankNode(const BlankNode* blankNode);
  void serializeIri(const String* iri);
};

}  // smallrdf

#endif  // NTRIPLESSERIALIZER_H_
