/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#ifndef NTRIPLESPARSER_H_
#define NTRIPLESPARSER_H_

#include "RDF.hpp"


namespace smallrdf {

class NTriplesParser {
public:
	//! \brief Parse input, extending provided RDF document
	//!
	//! \param input const String&  - input to be parsed
    //! \param doc Document*&  - original RDF document to be extended.
	//! 	Receives the ownership of the internal storage
	//! \return Document&  - extended RDF document
	static Document& parse(const String& input, Document*& doc);

	NTriplesParser();
#if __cplusplus >= 201103L
	NTriplesParser(NTriplesParser&& other);
#endif // __cplusplus 11+
    //! \brief Construct, initializing the internal RDF document
    //!
    //! \param doc Document*&  - original RDF document to be extended;
    //! 	invalidated after the call
	NTriplesParser(Document*& doc);

	~NTriplesParser();

    //! \brief Release the document, transferring the ownership and resetting the internal state
    //!
    //! \return Document*  - resulting allocated RDF document
	Document* release();
	Document& parse(const String& input);
protected:
	typedef const uint8_t  data_t;  //!< Data type

	const Quad* parseQuad();
	bool hasNext() const;
	data_t getNext();
	bool readWhiteSpace();
	const Term* readSubject();
	const Term* readPredicate();
	const Term* readObject();
	bool isLiteral() const;
	const Literal* readLiteral();
	const String* readLangtag();
	bool isIRIRef() const;
	const String* readIRIRef();
	bool isStringLiteralQuote() const;
	const String* readStringLiteralQuote();
	bool isBlankNodeLabel() const;
	const String* readBlankNodeLabel();
private:
	Document* _doc;
	data_t* _buf;
	data_t* _cur;
	data_t* _end;
};

}  // smallrdf

#endif  // NTRIPLESPARSER_H_
