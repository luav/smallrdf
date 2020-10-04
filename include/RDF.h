//! \brief C interface of the SmallRDF library
//! \author (c) 2020 Aretem Lutov
//! \license Apache License 2

#ifndef RDF_H_
#define RDF_H_

#include <stddef.h>
#include <stdint.h>

typedef enum {
	RTK_NAMED_NODE,
	RTK_BLANK_NODE,
	RTK_LITERAL,
	RTK_VARIABLE
} rdf_termkind_t;

#ifndef __cplusplus

// Accessory types for C interface =============================================
typedef enum {
	false,
	true
} bool;

// Core Types ==================================================================
typedef struct {
	char* data;  //!< String content
	size_t size;  //!<  Size in bytes including the null-terminator
	bool allocated;  //!< The string data were allocated rather than acquierd
} rdf_string_t;

typedef struct {
	const rdf_termkind_t termtype;
	const rdf_string_t* value;
} rdf_term_t;

typedef rdf_term_t rdf_namednode_t;

typedef struct {
	rdf_term_t;
	const rdf_string_t* language;
	const rdf_string_t* datatype;
} rdf_literal_t;

typedef rdf_term_t rdf_blanknode_t;

typedef struct {
	const rdf_term_t* subject;
	const rdf_term_t* predicate;
	const rdf_term_t* object;
	const rdf_term_t* graph;
} rdf_quad_t;

typedef struct rdf_dataset_t {
//	Stack<const rdf_quad_t*> quads;
//	Stack<struct rdf_dataset_t*> datasets;
} rdf_dataset_t;

typedef struct {
	rdf_dataset_t;
//	Stack<rdf_string_t*> strings;
//	Stack<rdf_term_t*> terms;
} rdf_document_t;

#else // __cplusplus

#if __cplusplus < 201103L && !defined(nullptr)
	#ifdef NULL
		#define nullptr  NULL
	#else
		#define nullptr  __null
	#endif // NULL
#endif // < __cplusplus 11+

// Omit restrict C keyword
#define restrict

namespace smallrdf {

typedef rdf_termkind_t  TermKind;

class String;
class Term;
class NamedNode;
class Literal;
class BlankNode;
class Quad;
class Dataset;
class Document;

}  // smallrdf

typedef smallrdf::String  rdf_string_t;
typedef smallrdf::Term  rdf_term_t;
typedef smallrdf::NamedNode  rdf_namednode_t;
typedef smallrdf::Literal  rdf_literal_t;
typedef smallrdf::BlankNode  rdf_blanknode_t;
typedef smallrdf::Quad  rdf_quad_t;
typedef smallrdf::Dataset  rdf_dataset_t;
typedef smallrdf::Document  rdf_document_t;

extern "C" {

#endif // __cplusplus

// rdf_string_t -------------------------------------------------------------------
rdf_string_t* rdf_string_create(const uint8_t* restrict data, size_t size);
rdf_string_t* rdf_string_create_cstr(const char* str);
void rdf_string_release(rdf_string_t* self);
bool rdf_string_equals(const rdf_string_t* self, const rdf_string_t* other);

// rdf_term_t ---------------------------------------------------------------------
rdf_term_t* rdf_term_create(const rdf_termkind_t termType, const rdf_string_t* value);
void rdf_term_release(rdf_term_t* self);
bool rdf_term_equals(const rdf_term_t* self, const rdf_term_t* other);

// rdf_namednode_t ----------------------------------------------------------------
rdf_namednode_t* rdf_namednode_create(const rdf_string_t* value);
void rdf_namednode_release(rdf_namednode_t* self);

// rdf_literal_t ------------------------------------------------------------------
rdf_literal_t* rdf_literal_create_simple(const rdf_string_t* value);
rdf_literal_t* rdf_literal_create(const rdf_string_t* value, const rdf_string_t* language,
	const rdf_string_t* datatype);
void rdf_literal_release(rdf_literal_t* self);
bool rdf_literal_equals(const rdf_term_t* self, const rdf_term_t* other);

// rdf_blanknode_t ----------------------------------------------------------------
rdf_blanknode_t* rdf_blanknode_create(const rdf_string_t* value);
void rdf_blanknode_release(rdf_blanknode_t* self);

// rdf_quad_t ---------------------------------------------------------------------
rdf_quad_t* rdf_quad_create(const rdf_term_t* subject, const rdf_term_t* predicate,
	const rdf_term_t* object, const rdf_term_t* graph);
void rdf_quad_release(rdf_quad_t* self);
bool rdf_quad_match_subject(const rdf_quad_t* self, const rdf_term_t* subject);
bool rdf_quad_match(const rdf_quad_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph);

// rdf_dataset_t ------------------------------------------------------------------
rdf_dataset_t* rdf_dataset_create();
void rdf_dataset_release(rdf_dataset_t* self);
const rdf_quad_t* rdf_dataset_find_subject(const rdf_dataset_t* self, const rdf_term_t* subject);
const rdf_quad_t* rdf_dataset_find(const rdf_dataset_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph);
rdf_dataset_t* rdf_dataset_match_subject(const rdf_dataset_t* self, const rdf_term_t* subject);
rdf_dataset_t* rdf_dataset_match(const rdf_dataset_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph);

// rdf_document_t -----------------------------------------------------------------
rdf_document_t* rdf_document_create();
void rdf_document_release(rdf_document_t* self);
const rdf_string_t* rdf_document_rfindString(const rdf_document_t* self, const rdf_string_t* newStr);
const rdf_term_t* rdf_document_rfindTerm(const rdf_document_t* self, const rdf_term_t* newTerm);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif  // RDF_H_
