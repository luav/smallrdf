/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.
(c) 2020 Artem Lutov
 */

#include <gtest/gtest.h>

#include "NTriplesParser.h"

using namespace smallrdf;


TEST(NTriplesParser, NamedNode) {
  const String input(
      "<http://example.org/subject> <http://example.org/predicate> <http://example.org/object> .");
  NTriplesParser  parser;  // Holds the ownwership of the generating rdf document
  parser.parse(input);
  const Document& doc = parser.parse(input);
  const Quad& quad = **doc.quads.begin();

  ASSERT_TRUE(*quad.subject->value == String("http://example.org/subject"));
  ASSERT_TRUE(*quad.predicate->value == String("http://example.org/predicate"));
  ASSERT_TRUE(*quad.object->value == String("http://example.org/object"));
}

TEST(NTriplesParser, Literal) {
  const String input(
      "<http://example.org/subject> <http://example.org/predicate> \"object\" .");
  Document* doc = nullptr;
  {
	  NTriplesParser  parser;
	  parser.parse(input);
	  doc = parser.release();  // Parser transfers the ownership
  }
  const Quad* quad = &**doc->quads.begin();

  ASSERT_TRUE(*quad->subject->value == String("http://example.org/subject"));
  ASSERT_TRUE(*quad->predicate->value == String("http://example.org/predicate"));
  ASSERT_TRUE(*quad->object->value == String("object"));

  const String input2(
      "<http://example.org/subject2> <http://example.org/predicate2> \"object2\" .");
  NTriplesParser::parse(input2, doc);
  auto qit = *doc->quads.begin();
  quad = &*qit;
  ASSERT_TRUE(*quad->subject->value == String("http://example.org/subject2"));
  quad = &**qit.next();
  ASSERT_TRUE(*quad->subject->value == String("http://example.org/subject"));

  delete doc;  // Release memory from the aquired object
}
