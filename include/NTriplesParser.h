/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#ifndef NTRIPLESPARSER_H_
#define NTRIPLESPARSER_H_

#include "RDF.hpp"

using namespace smallrdf;


class NTriplesParser {
 public:
  Document* document;

  explicit NTriplesParser(const String* input, Document* document = 0);

  Document* parse();
  static Document* parse_static(const String* input,
                                   Document* document = 0);

  const Quad* parseQuad();
  static const Quad* parseQuad_static(const String* input,
                                         Document* document);

 protected:
  const uint8_t* _buf;
  const uint8_t* _cur;
  const uint8_t* _end;

  const bool hasNext();
  const uint8_t getNext(const bool keep = false);
  const bool readWhiteSpace();
  const Term* readSubject();
  const Term* readPredicate();
  const Term* readObject();
  const bool isLiteral();
  const Literal* readLiteral();
  const String* readLangtag();
  const bool isIRIRef();
  const String* readIRIRef();
  const bool isStringLiteralQuote();
  const String* readStringLiteralQuote();
  const bool isBlankNodeLabel();
  const String* readBlankNodeLabel();
};

#endif  // NTRIPLESPARSER_H_
