/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#include <gtest/gtest.h>

#include "NTriplesSerializer.h"

using namespace smallrdf;


TEST(NTriplesSerializer, NamedNode) {
  Dataset dataset;
  const String subjectStr("http://example.org/subject");
  const String predicateStr("http://example.org/predicate");
  const String objectStr("http://example.org/object");
  NamedNode subject(subjectStr);
  NamedNode predicate(predicateStr);
  NamedNode object(objectStr);
  dataset.quads.add(Quad(subject, predicate, object));

  const char* expected =
      "<http://example.org/subject> <http://example.org/predicate> <http://example.org/object> .\n";
  NTriplesSerializer  ser;
  const String& res = ser.serialize(dataset);
  ASSERT_STREQ(expected, res.c_str());
}

TEST(NTriplesSerializer, Literal) {
  Dataset dataset;
  const String subjectStr("http://example.org/subject");
  const String predicateStr("http://example.org/predicate");
  const String objectStr("object");
  NamedNode subject(subjectStr);
  NamedNode predicate(predicateStr);
  Literal object(objectStr);
  dataset.quads.add(Quad(subject, predicate, object));

  const char* expected =
      "<http://example.org/subject> <http://example.org/predicate> \"object\" .\n";
  String* res = new String();
  NTriplesSerializer::serialize(dataset, res);
  ASSERT_STREQ(expected, res->c_str());

  NTriplesSerializer::serialize(dataset, res);
  String  exp2(expected);
  exp2 += exp2;
  ASSERT_TRUE(exp2 == *res);
  delete res;
}
