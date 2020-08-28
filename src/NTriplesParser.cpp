/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#include "NTriplesParser.h"

using namespace smallrdf;


NTriplesParser::NTriplesParser(const String* input, Document* document)
    : document(document),
      _buf(input->data()),
      _cur(input->data()),
      _end(input->data() + input->length()) {
  if (document == 0)
    this->document = new Document();
}

Document* NTriplesParser::parse() {
  while (hasNext())
    parseQuad();

  return document;
}

Document* NTriplesParser::parse_static(const String* input,
                                          Document* document) {
  NTriplesParser parser(input, document);

  return parser.parse();
}

const Quad* NTriplesParser::parseQuad() {
  readWhiteSpace();

  const Term* subject = readSubject();

  readWhiteSpace();

  const Term* predicate = readPredicate();

  readWhiteSpace();

  const Term* object = readObject();

  readWhiteSpace();

  if (getNext(true) == '.')
    getNext();

  if (subject && predicate && object)
    return document->triple(subject, predicate, object);
  return NULL;
}

const Quad* NTriplesParser::parseQuad_static(const String* input,
                                                Document* document) {
  NTriplesParser parser(input, document);

  return parser.parseQuad();
}

const bool NTriplesParser::hasNext() {
  return _cur < _end;
}

const uint8_t NTriplesParser::getNext(const bool keep) {
  if (keep)
    return *_cur;
  return *_cur++;
}

const bool NTriplesParser::readWhiteSpace() {
  bool read = false;
  uint8_t next = getNext(true);

  while (hasNext()
      && (next == ' ' || next == '\t' || next == '\n' || next == '\r')) {
    read = true;

    getNext();

    next = getNext(true);
  }

  return read;
}

const Term* NTriplesParser::readSubject() {
  if (isIRIRef())
    return document->namedNode(readIRIRef());
  else if (isBlankNodeLabel())
    return document->blankNode(readBlankNodeLabel());
  return NULL;
}

const Term* NTriplesParser::readPredicate() {
  if (isIRIRef())
    return document->namedNode(readIRIRef());
  return NULL;
}

const Term* NTriplesParser::readObject() {
  if (isIRIRef()) {
    return document->namedNode(readIRIRef());
  } else if (isLiteral()) {
    return readLiteral();
  } else if (isBlankNodeLabel())
    return document->blankNode(readBlankNodeLabel());
  return NULL;
}

const bool NTriplesParser::isLiteral() {
  return isStringLiteralQuote();
}

const Literal* NTriplesParser::readLiteral() {
  const String* value = readStringLiteralQuote();
  const String* language = readLangtag();

  // TODO(@bergos) check for ^^
  if (getNext(true) == '^') {
    getNext();
    getNext();
  }

  const String* datatype = readIRIRef();

  return document->literal(value, language, datatype);
}

const String* NTriplesParser::readLangtag() {
  if (getNext(true) != '@')
    return NULL;

  const uint8_t* buf = ++_cur;
  size_t length = 0;

  while (hasNext() && !readWhiteSpace()) {
    getNext();
    length++;
  }

  return document->string(buf, length);
}

const bool NTriplesParser::isIRIRef() {
  return getNext(true) == '<';
}

const String* NTriplesParser::readIRIRef() {
  if (!isIRIRef())
    return NULL;

  const uint8_t* buf = ++_cur;
  size_t length = 0;

  while (hasNext() && getNext() != '>')
    length++;

  return document->string(buf, length);
}

const bool NTriplesParser::isStringLiteralQuote() {
  return getNext(true) == '"';
}

const String* NTriplesParser::readStringLiteralQuote() {
  const uint8_t* buf = ++_cur;
  size_t length = 0;

  while (hasNext() && getNext() != '"')
    length++;

  return document->string(buf, length);
}

const bool NTriplesParser::isBlankNodeLabel() {
  // TODO(@bergos) check for _:
  return getNext(true) == '_';
}

const String* NTriplesParser::readBlankNodeLabel() {
  _cur += 2;

  const uint8_t* buf = _cur;
  size_t length = 0;

  while (hasNext() && !readWhiteSpace() && getNext(true) != '.') {
    getNext();
    length++;
  }

  return document->string(buf, length);
}
