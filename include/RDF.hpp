//! C++ interface of the Small  library
//! \author Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
//!		 (c) 2020 Artem Lutov

#ifndef RDF_HPP_
#define RDF_HPP_

#ifdef ARDUINO
namespace arddefs {
#include <Arduino.h>
// Note: Arduino.h may define min and max macroses that break C++ stdlib compilation
#if defined(_LIBCPP_VERSION) || defined(__GLIBCXX__) || defined(__cpp_lib_result_of_sfinae)
#undef max
#undef min
#endif // C++ stdlib
typedef String  AString;  // Arduino String
}  // arddefs
#endif  // ARDUINO

#include "Container.hpp"
#include "RDF.h"


namespace smallrdf {

#ifdef ARDUINO
typedef arddefs::AString  AString;  // Arduino String
#endif  // ARDUINO

// Implementation of C++ interface =============================================
class String {
public:
    //! \brief Become a view for the cstr or allocate a new String from it
    //!
    //! \param cstr const char*  - original string that holds the data ownership
    //! \param copy=false bool  - whether to copy cstr or become its view
	explicit String(const char* cstr, bool copy=false);

    //! \brief Create an empty string of the specified size (including the null-terminator)
    //!
	//! \param size size_t  - size of the buffer
	String(size_t size=0);
	//! \brief Create String from the provided buffer
	//! The string is allocated if the buffer if is not null-terminated,
	//! otherwise the buffer pointer is used
	//! \note The buffer pointer should not be acquired because its allocation type
	//! (new vs memalloc) is unknown
	//!
	//! \param buf const uint8_t*  - buffer, which may not have the null-terminator
	//! \param size size_t  - size of the buffer
	String(const uint8_t* buf, size_t size);
#ifdef ARDUINO
    //! \brief Become a view for the Arduino String or allocate a new String from it
	explicit String(const AString& str, bool copy = false);
	//! \brief Implicit conversion to Arduino String
	operator AString() const  { return c_str(); }
#endif // ARDUINO

#if __cplusplus >= 201103L
    //! \brief Acquire the string
    //!
    //! \param other String&&  - original string being acquired
	String(String&& other)=default;
#endif // __cplusplus 11+
    //! \brief Acquire ownership of data if other's content, making other a view
    //!
    //! \param other const String&  - original string, which could hold an ownership and becomes a view
	String(String& other);

#if __cplusplus >= 201103L
    //! \brief Acquire the string
    //!
    //! \param other String&&  - original string being acquired
    //! \return String&  - resulting string
	String& operator=(String&& other);
#endif // __cplusplus 11+
    //! \brief Become a view for the string
    //! \attention The former content is released, which may cause dangling references to released memory
    //!
    //! \param other const String&  - original string that holds the data ownership
    //! \return String&  - resulting string view
	String& operator=(const String& other);

	~String();
	void clear();  //!< Clear string content, deallocating data when necessary
    //! \brief Swap the string content, including the data ownership
    //! \attention Not tread-safe
    //!
    //! \param other String&  - swapoing string
    //! \return void
	void swap(String& other);

    //! \brief Resize the string
    //! \note A string view is always transformed to the string
    //!
    //! \param length size_t  - required length (excluding the null-terminator)
    //! \return bool  - whether resized successfully, otherwise the original content is retained
	bool resize(size_t length);
    //! \brief Acquire ownership of the content, transforming view to the string
    //! \note Does nothing if the content is already owned
    //!
    //! \return bool  - whether the ownership is acquired or there is insufficient memory for that
	bool acquire()
		{ return _allocated ? true : resize(length()); }
    //! \brief Release the string, transferring the ownership,
    //! 	and becoming a view to the resulting string
    //! \note A string view is always transformed to the string
    //!
    //! \return String*  - resulting allocated string; nullptr if the allocation is not possible
	String* release();

    //! \brief Extend the string with another one
    //! \note A string view is transformed to the string
    //!
    //! \param other const String&  - another string to be appended
    //! \return String&  - resulting string
	String& operator+=(const String& other);

	const char* c_str() const
		{ return reinterpret_cast<const char*>(_data); };
	char* c_str()
		{ return reinterpret_cast<char*>(_data); };

	const uint8_t* data() const
		{ return _data; };
	uint8_t* data()
		{ return _data; };

	//! \brief String length without the null-terminator
	size_t length() const
		{ return _size ? _size-1 : 0; }
	bool operator==(const String& other) const;
	bool operator!=(const String& other) const
		{ return !operator==(other); }

	bool allocated() const
		{ return _allocated; }
private:
	typedef uint8_t  data_t;

	data_t* _data;  //!< String content
	size_t  _size;  //!<  Size in bytes including the null-terminator
	bool _allocated;  //!< The string data were allocated rather than acquierd
};

class Term {
public:
	const TermKind kind;
	const String* value;  // Note: Always dereferencable value

	// Note: pass a value by reference to ensure that it is not a nullptr
	Term(TermKind tkind, const String& tval);
#if __cplusplus >= 201103L
	Term(Term&&)=default;
#endif // __cplusplus 11+
	//! \brief Copy constructor
	//! \note Is used because the object does not hold the ownership of its members,
	//! 	allowing to copy their pointers
	Term(const Term&)=default;

#if __cplusplus >= 201103L
	Term& operator=(Term&&)=default;
#endif // __cplusplus 11+
	Term& operator=(const Term&)=default;
	virtual ~Term() {}

	virtual bool operator==(const Term& other) const
		{ return kind == other.kind && *value == *other.value; }
	virtual bool operator!=(const Term& other) const final
		{ return !operator==(other); }
};

class NamedNode: public Term {
public:
	explicit NamedNode(const String& value);
#if __cplusplus >= 201103L
	NamedNode(NamedNode&&)=default;
#endif // __cplusplus 11+
	//! \brief Copy constructor
	//! \note Is used because the object does not hold the ownership of its members,
	//! 	allowing to copy their pointers
	NamedNode(const NamedNode&)=default;

#if __cplusplus >= 201103L
	NamedNode& operator=(NamedNode&&)=default;
#endif // __cplusplus 11+
	NamedNode& operator=(const NamedNode&)=default;
};

class Literal: public Term {
public:
	const String* lang;
	const String* dtype;

	Literal(const String& value, const String* lang=nullptr,
			const String* dtype=nullptr);
#if __cplusplus >= 201103L
	Literal(Literal&&)=default;
#endif // __cplusplus 11+
	//! \brief Copy constructor
	//! \note Is used because the object does not hold the ownership of its members,
	//! 	allowing to copy their pointers
	Literal(const Literal&)=default;

#if __cplusplus >= 201103L
	Literal& operator=(Literal&&)=default;
#endif // __cplusplus 11+
	Literal& operator=(const Literal&)=default;

	bool operator==(const Term& other) const override;
};

class BlankNode: public Term {
public:
	explicit BlankNode(const String& value);
#if __cplusplus >= 201103L
	BlankNode(BlankNode&&)=default;
#endif // __cplusplus 11+
	//! \brief Copy constructor
	//! \note Is used because the object does not hold the ownership of its members,
	//! 	allowing to copy their pointers
	BlankNode(const BlankNode&)=default;

#if __cplusplus >= 201103L
	BlankNode& operator=(BlankNode&&)=default;
#endif // __cplusplus 11+
	BlankNode& operator=(const BlankNode&)=default;
};

//! \brief A quad, which does not owns its members
class Quad {
public:
	const Term* subject;
	const Term* predicate;
	const Term* object;
	const Term* graph;

	// Note: pass some parameters by reference to ensure that each Quad is a valid triple
	Quad(const Term& subject, const Term& predicate,
		const Term& object, const Term* graph = nullptr);
	//! Explicit constructor to produce incomplete quads for partial search
	//! \attention Should not be used to store quads in the dataset
	explicit Quad(const Term* subject = nullptr, const Term* predicate = nullptr,
		const Term* object = nullptr, const Term* graph = nullptr);

#if __cplusplus >= 201103L
	Quad(Quad&&)=default;
#endif // __cplusplus 11+
	//! \brief Copy constructor
	//! \note Is used because the object does not hold the ownership of its members,
	//! 	allowing to copy their pointers
	Quad(const Quad&)=default;

#if __cplusplus >= 201103L
	Quad& operator=(Quad&&)=default;
#endif // __cplusplus 11+
	Quad& operator=(const Quad&)=default;

	bool operator==(const Quad& other) const;
	bool operator!=(const Quad& other) const
		{ return !operator==(other); }
	bool match(const Term* subject = nullptr, const Term* predicate = nullptr,
				const Term* object = nullptr, const Term* graph = nullptr) const;
};

//! \brief Main interface for the Quad/Triplesotre
class Dataset {
public:
	typedef Stack<Quad>  Quads;
	Quads quads;  //!< Actual Quad/Triplestore

	virtual ~Dataset()  {}

	Quad* find(const Quad& quad);
	Quads match(const Term* subject = nullptr, const Term* predicate = nullptr,
				const Term* object = nullptr, const Term* graph = nullptr);
};

//! \brief RDF document, which owns all the stored objects, becoming a session memory manager
class Document: public Dataset {
	typedef Stack<String>  Strings;
	Strings _strings;

	typedef Stack<Term>  Terms;
	Terms _terms;
public:
    //! \brief Transfer ownership of the str to the document
    //!
    //! \param str String*  - original string/view, becoming a view by transferring
    //! 	the ownership to the document
    //! \return const String*  - stored owned sting
	const String* string(String& str);
#if __cplusplus >= 201103L
	const String* string(String&& str)
		{ return string(str); }  // Calls string(String& str);
#endif // __cplusplus 11+
	const NamedNode* namedNode(const String& value);
	const Literal* literal(const String& value, const String* lang=nullptr,
						   const String* dtyoe=nullptr);
	const BlankNode* blankNode(const String& value);
	const Quad* quad(const Term& subject, const Term& predicate,
					   const Term& object, const Term* graph = nullptr);
protected:
	const String* findString(const String& newStr) const;
	const Term* findTerm(const Term& newTerm) const;
};

}  // smallrdf

#endif  // RDF_HPP_
