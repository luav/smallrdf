//! \brief Implementation of the C interface of the SmallRDF library
//! \author (c) 2020 Aretem Lutov
//! \license Apache License 2

#include <assert.h>
#include "RDF.h"

// rdf_string_t ----------------------------------------------------------------
rdf_string_t* rdf_string_create(const uint8_t* restrict data, size_t size)
{
	rdf_string_t *rstr = malloc(sizeof(rdf_string_t));
	rstr->data = data;
	rstr->size = size;
	rstr->allocated = false;
	// Ensure that the data is null-terminated
	if (data && size && data[size-1] != 0) {
		rstr->data = malloc(++rstr->size);
		rstr->allocated = true;
		rstr->data[size] = 0;
		memcpy(rstr->data, data, size);
	}
	assert((!rstr->data || rstr->size >= strlen(rstr->c_str()) + 1) && "Invalid size of the data");
	return rstr;
}

rdf_string_t* rdf_string_create_cstr(const char* str)
{
	rdf_string_t *rstr = malloc(sizeof(rdf_string_t));
	rstr->data = str;
	rstr->size = str ? strlen(str) + 1 : 0;
	rstr->allocated = false;
	return rstr;
}

void rdf_string_release(rdf_string_t* self)
{
	if(!self)
		return;
	if(self->allocated)
		free(self->data);
	free(self);
}

bool rdf_string_equals(const rdf_string_t* self, const rdf_string_t* other)
{
	if(self == other)
		return true;
	else if(!self || !other || self->size != other->size || !self->data ^ !other->data)
		return false;

	return !memcmp(self->data, other->data, self->size);
}

// rdf_term_t ------------------------------------------------------------------
rdf_term_t* rdf_term_create(const rdf_termkind_t termType, const rdf_string_t* value)
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_term_release(rdf_term_t* self)
{
	assert(0 && "Not implemented");
}

bool rdf_term_equals(const rdf_term_t* self, const rdf_term_t* other)
{
	assert(0 && "Not implemented");
	return false;
}

// rdf_namednode_t -------------------------------------------------------------
rdf_namednode_t* rdf_namednode_create(const rdf_string_t* value)
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_namednode_release(rdf_namednode_t* self)
{
	assert(0 && "Not implemented");
}

// rdf_literal_t ---------------------------------------------------------------
rdf_literal_t* rdf_literal_create_simple(const rdf_string_t* value)
{
	assert(0 && "Not implemented");
	return NULL;
}

rdf_literal_t* rdf_literal_create(const rdf_string_t* value, const rdf_string_t* language,
	const rdf_string_t* datatype)
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_literal_release(rdf_literal_t* self)
{
	assert(0 && "Not implemented");
}

bool rdf_literal_equals(const rdf_term_t* self, const rdf_term_t* other)
{
	assert(0 && "Not implemented");
	return false;
}

// rdf_blanknode_t -------------------------------------------------------------
rdf_blanknode_t* rdf_blanknode_create(const rdf_string_t* value)
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_blanknode_release(rdf_blanknode_t* self)
{
	assert(0 && "Not implemented");
}

// rdf_quad_t ------------------------------------------------------------------
rdf_quad_t* rdf_quad_create(const rdf_term_t* subject, const rdf_term_t* predicate,
	const rdf_term_t* object, const rdf_term_t* graph)
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_quad_release(rdf_quad_t* self)
{
	assert(0 && "Not implemented");
}

bool rdf_quad_match_subject(const rdf_quad_t* self, const rdf_term_t* subject)
{
	assert(0 && "Not implemented");
	return false;
}

bool rdf_quad_match(const rdf_quad_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph)
{
	assert(0 && "Not implemented");
	return false;
}

// rdf_dataset_t ---------------------------------------------------------------
rdf_dataset_t* rdf_dataset_create()
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_dataset_release(rdf_dataset_t* self)
{
	assert(0 && "Not implemented");
}

const rdf_quad_t* rdf_dataset_find_subject(const rdf_dataset_t* self, const rdf_term_t* subject)
{
	assert(0 && "Not implemented");
	return NULL;
}

const rdf_quad_t* rdf_dataset_find(const rdf_dataset_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph)
{
	assert(0 && "Not implemented");
	return NULL;
}

rdf_dataset_t* rdf_dataset_match_subject(const rdf_dataset_t* self, const rdf_term_t* subject)
{
	assert(0 && "Not implemented");
	return NULL;
}

rdf_dataset_t* rdf_dataset_match(const rdf_dataset_t* self, const rdf_term_t* subject,
	const rdf_term_t* predicate, const rdf_term_t* object, const rdf_term_t* graph)
{
	assert(0 && "Not implemented");
	return NULL;
}

// rdf_document_t --------------------------------------------------------------
rdf_document_t* rdf_document_create()
{
	assert(0 && "Not implemented");
	return NULL;
}

void rdf_document_release(rdf_document_t* self)
{
	assert(0 && "Not implemented");
}

const rdf_string_t* rdf_document_rfindString(const rdf_document_t* self, const rdf_string_t* newStr)
{
	assert(0 && "Not implemented");
	return NULL;
}

const rdf_term_t* rdf_document_rfindTerm(const rdf_document_t* self, const rdf_term_t* newTerm)
{
	assert(0 && "Not implemented");
	return NULL;
}
