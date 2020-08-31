/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#ifndef NTRIPLESSERIALIZER_H_
#define NTRIPLESSERIALIZER_H_

#include "RDF.hpp"


namespace smallrdf {

class NTriplesSerializer {
	String* _buf;
	uint8_t* _cur;
	uint8_t* _end;
public:
	//! \brief Serialize RDF dataset to the storage
	//!
	//! \param dataset const Dataset&  - input RDF dataset
	//! \param storage String*&  - resulting serialized data, being extended.
	//! 	Receives the ownership of the internal storage
	//! \return const String&  - serialized data
	static String& serialize(const Dataset& dataset, String*& storage);

	NTriplesSerializer();
    //! \brief Construct, initializing the internal storage
    //!
    //! \param storage String&  - storage, being extended on serialization;
    //! 	invalidated after the call
    NTriplesSerializer(NTriplesSerializer&& other);
	NTriplesSerializer(String*& storage);
	~NTriplesSerializer();

    //! \brief Release the storage, transferring the ownership and resetting the internal state
    //!
    //! \return String*  - resulting allocated storage
	String* release();

	String& storage();
	const String& storage() const
		{ return const_cast<NTriplesSerializer*>(this)->storage(); }

	//! \brief Iteratively serialize RDF datasets
	//!
	//! \param dataset const Dataset&  - RDF dataset being serialized
	//! \return String&  - serialized data
	String& serialize(const Dataset& dataset);
protected:
	void write(uint8_t chr);
	void write(const String& str);


	//! \brief Evaluate dataset size
	//!
	//! \param dataset const Dataset& - dataset to be processed
	//! \return size_t  - pure size of the dataset (without the null-terminator used in serlization)
	size_t datasetSize(const Dataset& dataset) const;
	void serializeDataset(const Dataset& dataset);
	size_t quadSize(const Quad& quad) const;
	void serializeQuad(const Quad& quad);
	size_t termSize(const Term* term) const;
	void serializeTerm(const Term* term);
	size_t literalSize(const Literal& literal) const;
	void serializeLiteral(const Literal& literal);
	void serializeBlankNode(const BlankNode& blankNode);
	void serializeIri(const String* iri);
};

}  // smallrdf

#endif  // NTRIPLESSERIALIZER_H_
