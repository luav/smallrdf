//! \brief C interface of the Small  library
//! \author (c) 2020 Aretem Lutov
//! \license Apache License 2

#ifndef _H_
#define _H_

#include <stddef.h>
#include <stdint.h>

#include "Container.hpp"


typedef enum {
  _NAMED_NODE,
  _BLANK_NODE,
  _LITERAL,
  _VARIABLE
} rdf_term_type_t;

#ifndef __cplusplus

// Accessory types for C interface =============================================
typedef enum {
    false,
    true
} bool;

// Core Types ==================================================================
typename struct {
  const char* const data;  //!< String content
  const size_t size;  //!<  Size in bytes including the null-terminator
  bool _allocated;  //!< The string data were allocated rather than acquierd
} rdf_string_t;

typename struct {
  const rdf_term_type_t termType;
  const rdf_string_t* value;
} rdf_term_t;

typedef rdf_term_t rdf_namednode_t;

typename struct {
  rdf_term_t;
  const rdf_string_t* language;
  const rdf_string_t* datatype;
} rdf_literal_t;

typedef rdf_term_t rdf_blanknode_t;

typename struct {
  const rdf_term_t* subject;
  const rdf_term_t* predicate;
  const rdf_term_t* object;
  const rdf_term_t* graph;
} rdf_quad_t;

typename struct rdf_dataset_t {
  List<const rdf_quad_t*> quads;
  List<struct rdf_dataset_t*> datasets;
} rdf_dataset_t;

typename struct {
  rdf_dataset_t;
  List<rdf_string_t*> strings;
  List<rdf_term_t*> terms;
} rdf_document_t;

#else // __cplusplus
namespace smallrdf {

using TermType = rdf_term_type_t;

class String;
class Term;
class NamedNode;
class Literal;
class BlankNode;
class Quad;
class Dataset;
class Document;

}  // smallrdf

using rdf_string_t = smallrdf::String;
using rdf_term_t = smallrdf::Term;
using rdf_namednode_t = smallrdf::NamedNode;
using rdf_literal_t = smallrdf::Literal;
using rdf_blanknode_t = smallrdf::BlankNode;
using rdf_quad_t = smallrdf::Quad;
using rdf_dataset_t = smallrdf::Dataset;
using rdf_document_t = smallrdf::Document;

extern "C" {
#endif // __cplusplus

// rdf_string_t -------------------------------------------------------------------
rdf_string_t* rdf_string_create(const uint8_t* data, const size_t size);
rdf_string_t* rdf_string_create_cstr(const char* str);
void rdf_string_release(rdf_string_t* self);
bool rdf_string_equals(const rdf_string_t* self, const rdf_string_t* other);

// rdf_term_t ---------------------------------------------------------------------
rdf_term_t* rdf_term_create(const rdf_term_type_t termType, const rdf_string_t* value);
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
bool rdf_quad_match(const rdf_quad_t* self, const rdf_term_t* subject, const rdf_term_t* predicate,
                   const rdf_term_t* object, const rdf_term_t* graph);

// rdf_dataset_t ------------------------------------------------------------------
rdf_dataset_t* rdf_dataset_create();
void rdf_dataset_release(rdf_dataset_t* self);
const rdf_quad_t* rdf_dataset_find_subject(const rdf_dataset_t* self, const rdf_term_t* subject);
const rdf_quad_t* rdf_dataset_find(const rdf_dataset_t* self, const rdf_term_t* subject,
                               const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph);
rdf_dataset_t* rdf_dataset_match_subject(const rdf_dataset_t* self, const rdf_term_t* subject);
rdf_dataset_t* rdf_dataset_match(const rdf_dataset_t* self, const rdf_term_t* subject, const rdf_term_t* predicate,
                             const rdf_term_t* object, const rdf_term_t* graph);

// rdf_document_t -----------------------------------------------------------------
rdf_document_t* rdf_document_create();
void Rrdf_document_release(rdf_document_t* self);
const rdf_string_t* rdf_document_rfindString(const rdf_document_t* self, const rdf_string_t* newStr);
const rdf_term_t* rdf_document_rfindTerm(const rdf_document_t* self, const rdf_term_t* newTerm);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif  // _H_
