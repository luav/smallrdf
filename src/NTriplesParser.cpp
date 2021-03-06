/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

 #include <ctype.h>  // isspace
#include "NTriplesParser.h"

using namespace smallrdf;


NTriplesParser::NTriplesParser()
	: _doc(new Document()),
	  _buf(nullptr),
	  _cur(nullptr),
	  _end(nullptr)
{
}

#if __cplusplus >= 201103L
NTriplesParser::NTriplesParser(NTriplesParser&& other)
	: _doc(other._doc),
	  _buf(other._buf),
	  _cur(other._cur),
	  _end(other._end)
{
	other._doc = new Document();
	other._buf = other._cur = other._end = nullptr;
}
#endif // __cplusplus 11+

NTriplesParser::NTriplesParser(Document*& doc)
	: _doc(doc ? doc : new Document()),
	  _buf(nullptr),
	  _cur(nullptr),
	  _end(nullptr)
{
	doc = nullptr;  // Invalidate the pointer to ensure self-sufficiency of the internal data
}

NTriplesParser::~NTriplesParser()
{
	if(_doc) {
		delete _doc;
		_doc = nullptr;
	}
	_end = _cur = _buf = nullptr;
}


Document* NTriplesParser::release()
{
	Document *res = _doc;
	_doc = new Document();  // Reset the internal state to insure its self-sufficiency
	_end = _cur = _buf = nullptr;
	return res;
}

Document& NTriplesParser::parse(const String& input)
{
	_buf = input.data();
	_cur = input.data(),
	_end = input.data() + input.length();
	assert(_doc && "Internal data should be initialized");
	while (hasNext())
		parseQuad();

	return *_doc;
}

Document& NTriplesParser::parse(const String& input, Document*& doc)
{
	NTriplesParser parser(doc);
	parser.parse(input);
	return *(doc = parser.release());
}

const Quad* NTriplesParser::parseQuad()
{
	readWhiteSpace();
	const Term* subject = readSubject();
	readWhiteSpace();
	const Term* predicate = readPredicate();
	readWhiteSpace();
	const Term* object = readObject();
	readWhiteSpace();
	if (*_cur == '.')
		++_cur;

	if (subject && predicate && object)
		return _doc->quad(*subject, *predicate, *object);
	return nullptr;
}

bool NTriplesParser::hasNext() const
{
	return _cur < _end;
}

NTriplesParser::data_t NTriplesParser::getNext()
{
	return *_cur++;
}

bool NTriplesParser::readWhiteSpace()
{
	const data_t* beg = _cur;

	// next == ' ' || next == '\t' || next == '\n' || next == '\r'; \f, \v
	while(hasNext() && isspace(*_cur))
		++_cur;

	return beg != _cur;
}

const Term* NTriplesParser::readSubject()
{
	// Note: readIRIRef() returns the nullptr only if memory is insufficient, when it is OK to crash the app
	if (isIRIRef())
		return _doc->namedNode(*readIRIRef());
	else if (isBlankNodeLabel())
		return _doc->blankNode(*readBlankNodeLabel());
	return nullptr;
}

const Term* NTriplesParser::readPredicate()
{
	if (isIRIRef())
		return _doc->namedNode(*readIRIRef());
	return nullptr;
}

const Term* NTriplesParser::readObject()
{
	if (isIRIRef())
		return _doc->namedNode(*readIRIRef());
	else if (isLiteral())
		return readLiteral();
	else if (isBlankNodeLabel())
		return _doc->blankNode(*readBlankNodeLabel());
	return nullptr;
}

bool NTriplesParser::isLiteral() const
{
	return isStringLiteralQuote();
}

const Literal* NTriplesParser::readLiteral()
{
	if(!isLiteral())
		return nullptr;
	const String* value = readStringLiteralQuote();
	const String* language = readLangtag();

	// TODO(@bergos) check for ^^
	if (*_cur == '^')
		++_cur;
	if (*_cur == '^')
		++_cur;

	// Note: readIRIRef() returns the nullptr only if memory is insufficient, when it is OK to crash the app
	const String* dtype = readIRIRef();
	return _doc->literal(*value, language, dtype);
}

const String* NTriplesParser::readLangtag()
{
	if (*_cur != '@')
		return nullptr;

	data_t* buf = ++_cur;
	while (hasNext() && !readWhiteSpace())
		++_cur;

#if __cplusplus >= 201103L
	#define STR_PARAM  String(buf, _cur - buf)
#else
	String  str(buf, _cur - buf);
	#define STR_PARAM  str
#endif // __cplusplus 11+
	return _doc->string(STR_PARAM);
#undef STR_PARAM
}

bool NTriplesParser::isIRIRef() const
{
	return *_cur == '<';
}

const String* NTriplesParser::readIRIRef()
{
	if (!isIRIRef())
		return nullptr;

	data_t* buf = ++_cur;
	while(hasNext() && getNext() != '>');  // Note: the cycle body is intentionally empty

// Note: -1 to consider the increment upon the '>'
#if __cplusplus >= 201103L
	#define STR_PARAM  String(buf, _cur - buf - 1)
#else
	String  str(buf, _cur - buf - 1);
	#define STR_PARAM  str
#endif // __cplusplus 11+
	return _doc->string(STR_PARAM);
#undef STR_PARAM
}

bool NTriplesParser::isStringLiteralQuote() const
{
	return *_cur == '"';
}

const String* NTriplesParser::readStringLiteralQuote()
{
	data_t* buf = ++_cur;
	while (hasNext() && getNext() != '"'); // Note: the cycle body is intentionally empty

// Note: -1 to consider the increment upon the '"'
#if __cplusplus >= 201103L
	#define STR_PARAM  String(buf, _cur - buf - 1)
#else
	String  str(buf, _cur - buf - 1);
	#define STR_PARAM  str
#endif // __cplusplus 11+
	return _doc->string(STR_PARAM);
#undef STR_PARAM
}

bool NTriplesParser::isBlankNodeLabel() const
{
	// TODO(@bergos) check for _:
	return *_cur == '_';
}

const String* NTriplesParser::readBlankNodeLabel()
{
	if(!isBlankNodeLabel())
		return nullptr;
	_cur += 2;

	data_t* buf = _cur;
	while (hasNext() && !readWhiteSpace() && *_cur != '.')
		++_cur;

#if __cplusplus >= 201103L
	#define STR_PARAM  String(buf, _cur - buf)
#else
	String  str(buf, _cur - buf);
	#define STR_PARAM  str
#endif // __cplusplus 11+
	return _doc->string(STR_PARAM);
#undef STR_PARAM
}
