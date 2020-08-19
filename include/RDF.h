//! \brief C interface of the Small RDF library
//! \author (c) 2020 Aretem Lutov
//! \license Apache License 2

#ifndef RDF_H_
#define RDF_H_

#include <stddef.h>
#include <stdint.h>

#include "RDFContainer.hpp"


typedef enum {
  RDF_NAMED_NODE,
  RDF_BLANK_NODE,
  RDF_LITERAL,
  RDF_VARIABLE
} RDFTermType;

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
} RDFString;

typename struct {
  const RDFTermType termType;
  const RDFString* value;
} RDFTerm;

typedef RDFTerm RDFNamedNode;

typename struct {
  RDFTerm;
  const RDFString* language;
  const RDFString* datatype;
} RDFLiteral;

typedef RDFTerm RDFBlankNode;

typename struct {
  const RDFTerm* subject;
  const RDFTerm* predicate;
  const RDFTerm* object;
  const RDFTerm* graph;
} RDFQuad;

typename struct RDFDataset {
  RDFList<const RDFQuad*> quads;
  RDFList<struct RDFDataset*> datasets;
} RDFDataset;

typename struct {
  RDFDataset;
  RDFList<RDFString*> strings;
  RDFList<RDFTerm*> terms;
} RDFDocument;

#else // __cplusplus
class RDFString;
class RDFTerm;
class RDFNamedNode;
class RDFLiteral;
class RDFBlankNode;
class RDFQuad;
class RDFDataset;
class RDFDocument;

extern "C"
{
#endif // __cplusplus

// RDFString -------------------------------------------------------------------
RDFString* RDFString_create(const uint8_t* data, const size_t size);
RDFString* RDFString_create_str(const char* str);
void RDFString_release(RDFString* self);
bool RDFString_equals(const RDFString* self, const RDFString* other);

// RDFTerm ---------------------------------------------------------------------
RDFTerm* RDFTerm_create(const RDFTermType termType, const RDFString* value);
void RDFTerm_release(RDFTerm* self);
bool RDFTerm_equals(const RDFTerm* self, const RDFTerm* other);

// RDFNamedNode ----------------------------------------------------------------
RDFNamedNode* RDFNamedNode_create(const RDFString* value);
void RDFNamedNode_release(RDFNamedNode* self);

// RDFLiteral ------------------------------------------------------------------
RDFLiteral* RDFLiteral_create_simple(const RDFString* value);
RDFLiteral* RDFLiteral_create(const RDFString* value, const RDFString* language,
                              const RDFString* datatype);
void RDFLiteral_release(RDFLiteral* self);
bool RDFLiteral_equals(const RDFTerm* self, const RDFTerm* other);

// RDFBlankNode ----------------------------------------------------------------
RDFBlankNode* RDFBlankNode_create(const RDFString* value);
void RDFBlankNode_release(RDFBlankNode* self);

// RDFQuad ---------------------------------------------------------------------
RDFQuad* RDFQuad_create(const RDFTerm* subject, const RDFTerm* predicate,
          const RDFTerm* object, const RDFTerm* graph);
void RDFQuad_release(RDFQuad* self);
bool RDFQuad_match_subject(const RDFQuad* self, const RDFTerm* subject);
bool RDFQuad_match(const RDFQuad* self, const RDFTerm* subject, const RDFTerm* predicate,
                   const RDFTerm* object, const RDFTerm* graph);

// RDFDataset ------------------------------------------------------------------
RDFDataset* RDFDataset_create();
void RDFDataset_release(RDFDataset* self);
const RDFQuad* RDFDataset_find_subject(const RDFDataset* self, const RDFTerm* subject);
const RDFQuad* RDFDataset_find(const RDFDataset* self, const RDFTerm* subject,
                               const RDFTerm* predicate, const RDFTerm* object, const RDFTerm* graph);
RDFDataset* RDFDataset_match_subject(const RDFDataset* self, const RDFTerm* subject);
RDFDataset* RDFDataset_match(const RDFDataset* self, const RDFTerm* subject, const RDFTerm* predicate,
                             const RDFTerm* object, const RDFTerm* graph);

// RDFDocument -----------------------------------------------------------------
RDFDocument* RDFDocument_create();
void RRDFDocument_release(RDFDocument* self);
const RDFString* RDFDocument_rfindString(const RDFDocument* self, const RDFString* newStr);
const RDFTerm* RDFDocument_rfindTerm(const RDFDocument* self, const RDFTerm* newTerm);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

#endif  // RDF_H_
