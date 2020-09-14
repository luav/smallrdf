/* Copyright 2016 Thomas Bergwinkl. All Rights Reserved.

 */

#include <gtest/gtest.h>

#include "RDF.hpp"

using namespace smallrdf;


TEST(String, length) {
  ASSERT_EQ(0, String().length());
  ASSERT_EQ(4, String("test").length());
  ASSERT_EQ(3, String((const uint8_t*)"test", 3).length());
}

TEST(String, data) {
  const char* test = "test";

  ASSERT_EQ(nullptr, String().data());
  ASSERT_EQ((const uint8_t*)test, String(test).data());
  ASSERT_EQ(true,
            String((const uint8_t*)test, 4) == String(test));
//  // Note: the test below is ill-formed because it compares pointers, which are different
//  // because the String had to relocate a buffer lacking the null terminator in the end (4th character)
//  ASSERT_EQ((const uint8_t*)test,
//            String((const uint8_t*)test, 4).data());
}

TEST(String, equals) {
  String test("test");
  String tset("tset");
  String testDifferentLength("testDifferentLength");
  String empty;
  String equals("test");
  String equalsEmpty;

  ASSERT_EQ(false, test == tset);
  ASSERT_EQ(false, test == testDifferentLength);
  ASSERT_EQ(false, test == empty);
  ASSERT_EQ(true, test == equals);
  ASSERT_EQ(true, empty == equalsEmpty);
}

TEST(String, c_str) {
  String test("test");
  String tes((const uint8_t*) "test", 3);

  ASSERT_STREQ("test", test.c_str());
  ASSERT_STREQ("tes", tes.c_str());
}

TEST(Term, kind) {
  String value;

  ASSERT_EQ(RTK_NAMED_NODE, Term(RTK_NAMED_NODE, value).kind);
}

TEST(Term, value) {
  String str("test");

  ASSERT_EQ(&str, Term(RTK_NAMED_NODE, str).value);
}

TEST(Term, equals) {
  String test("test");
  String tset("tset");

  Term termTest(RTK_NAMED_NODE, test);
  Term termTset(RTK_NAMED_NODE, tset);

  ASSERT_TRUE(termTest == termTest);
  ASSERT_FALSE(termTest == termTset);
}

TEST(NamedNode, kind) {
  String value;

  ASSERT_EQ(RTK_NAMED_NODE, NamedNode(value).kind);
}

TEST(NamedNode, value) {
  String str("http://example.org/");

  ASSERT_EQ(&str, NamedNode(str).value);
}

TEST(Literal, kind) {
  String value;

  ASSERT_EQ(RTK_LITERAL, Literal(value).kind);
}

TEST(Literal, value) {
  String value("test");

  ASSERT_EQ(&value, Literal(value).value);
}

TEST(Literal, language) {
  String value;
  String language("en");

  ASSERT_EQ(&language, Literal(value, &language).lang);
}

TEST(Literal, datatype) {
  String value;
  String datatype("http://example.org/");

  ASSERT_EQ(&datatype, Literal(value, nullptr, &datatype).dtype);
}

TEST(BlankNode, kind) {
  String value;

  ASSERT_EQ(RTK_BLANK_NODE, BlankNode(value).kind);
}

TEST(BlankNode, value) {
  String value("test");

  ASSERT_EQ(&value, BlankNode(value).value);
}

TEST(Quad, subject) {
  String iri("http://example.org/");
  NamedNode term(iri);

  ASSERT_EQ(&term, Quad(term, term, term, nullptr).subject);
}

TEST(Quad, predicate) {
  String iri("http://example.org/");
  NamedNode term(iri);

  ASSERT_EQ(&term, Quad(term, term, term, nullptr).predicate);
}

TEST(Quad, object) {
  String iri("http://example.org/");
  NamedNode term(iri);

  ASSERT_EQ(&term, Quad(term, term, term, nullptr).object);
}

TEST(Quad, graph) {
  String iri("http://example.org/");
  NamedNode term(iri);

  ASSERT_EQ(&term, Quad(term, term, term, &term).graph);
}

TEST(Dataset, find) {
  Dataset dataset;
  String subjectStr1("http://example.org/subject1");
  String subjectStr2("http://example.org/subject2");
  String predicateStr("http://example.org/predicate");
  String objectStr("http://example.org/object");
  NamedNode subject1(subjectStr1);
  NamedNode subject2(subjectStr2);
  NamedNode predicate(predicateStr);
  NamedNode object(objectStr);
  const Quad  quad1(subject1, predicate, object);
  dataset.quads.add(quad1);
  dataset.quads.add(Quad(subject2, predicate, object));

  ASSERT_TRUE(dataset.quads.length() == 2);
  ASSERT_TRUE(dataset.find(quad1));
  ASSERT_TRUE(dataset.match(&subject2).length());
  ASSERT_FALSE(dataset.match(&object).length());
}

TEST(Document, string) {
  Document doc;

  const String* str1 = doc.string(String("test"));
  const String* str2 = doc.string(String("test"));
  const String* str3 = doc.string(String((const uint8_t*) "test", 4));

  ASSERT_STREQ(str1->c_str(), "test");
  ASSERT_EQ(str1, str2);
  ASSERT_EQ(str1, str3);
}
