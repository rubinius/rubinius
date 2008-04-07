/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#define _CXXTEST_ABORT_TEST_ON_FAIL
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "test_array.hpp"

static TestArray suite_TestArray;

static CxxTest::List Tests_TestArray = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestArray( "test_array.hpp", 9, "TestArray", suite_TestArray, Tests_TestArray );

static class TestDescription_TestArray_test_get : public CxxTest::RealTestDescription {
public:
 TestDescription_TestArray_test_get() : CxxTest::RealTestDescription( Tests_TestArray, suiteDescription_TestArray, 23, "test_get" ) {}
 void runTest() { suite_TestArray.test_get(); }
} testDescription_TestArray_test_get;

static class TestDescription_TestArray_test_set_resizes : public CxxTest::RealTestDescription {
public:
 TestDescription_TestArray_test_set_resizes() : CxxTest::RealTestDescription( Tests_TestArray, suiteDescription_TestArray, 32, "test_set_resizes" ) {}
 void runTest() { suite_TestArray.test_set_resizes(); }
} testDescription_TestArray_test_set_resizes;

#include "test_bignum.hpp"

static TestBignum suite_TestBignum;

static CxxTest::List Tests_TestBignum = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestBignum( "test_bignum.hpp", 9, "TestBignum", suite_TestBignum, Tests_TestBignum );

static class TestDescription_TestBignum_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_create() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 29, "test_create" ) {}
 void runTest() { suite_TestBignum.test_create(); }
} testDescription_TestBignum_test_create;

static class TestDescription_TestBignum_test_to_i : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_to_i() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 35, "test_to_i" ) {}
 void runTest() { suite_TestBignum.test_to_i(); }
} testDescription_TestBignum_test_to_i;

static class TestDescription_TestBignum_test_create_negative : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_create_negative() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 40, "test_create_negative" ) {}
 void runTest() { suite_TestBignum.test_create_negative(); }
} testDescription_TestBignum_test_create_negative;

static class TestDescription_TestBignum_test_new_unsigned : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_new_unsigned() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 45, "test_new_unsigned" ) {}
 void runTest() { suite_TestBignum.test_new_unsigned(); }
} testDescription_TestBignum_test_new_unsigned;

static class TestDescription_TestBignum_test_normalize : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_normalize() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 50, "test_normalize" ) {}
 void runTest() { suite_TestBignum.test_normalize(); }
} testDescription_TestBignum_test_normalize;

static class TestDescription_TestBignum_test_add : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_add() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 76, "test_add" ) {}
 void runTest() { suite_TestBignum.test_add(); }
} testDescription_TestBignum_test_add;

static class TestDescription_TestBignum_test_sub : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_sub() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 81, "test_sub" ) {}
 void runTest() { suite_TestBignum.test_sub(); }
} testDescription_TestBignum_test_sub;

static class TestDescription_TestBignum_test_mul : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_mul() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 86, "test_mul" ) {}
 void runTest() { suite_TestBignum.test_mul(); }
} testDescription_TestBignum_test_mul;

static class TestDescription_TestBignum_test_equal : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_equal() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 92, "test_equal" ) {}
 void runTest() { suite_TestBignum.test_equal(); }
} testDescription_TestBignum_test_equal;

static class TestDescription_TestBignum_test_compare : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_compare() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 100, "test_compare" ) {}
 void runTest() { suite_TestBignum.test_compare(); }
} testDescription_TestBignum_test_compare;

static class TestDescription_TestBignum_test_gt : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_gt() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 108, "test_gt" ) {}
 void runTest() { suite_TestBignum.test_gt(); }
} testDescription_TestBignum_test_gt;

static class TestDescription_TestBignum_test_ge : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_ge() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 115, "test_ge" ) {}
 void runTest() { suite_TestBignum.test_ge(); }
} testDescription_TestBignum_test_ge;

static class TestDescription_TestBignum_test_lt : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_lt() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 122, "test_lt" ) {}
 void runTest() { suite_TestBignum.test_lt(); }
} testDescription_TestBignum_test_lt;

static class TestDescription_TestBignum_test_le : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_le() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 129, "test_le" ) {}
 void runTest() { suite_TestBignum.test_le(); }
} testDescription_TestBignum_test_le;

static class TestDescription_TestBignum_test_from_string_detect : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_from_string_detect() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 136, "test_from_string_detect" ) {}
 void runTest() { suite_TestBignum.test_from_string_detect(); }
} testDescription_TestBignum_test_from_string_detect;

static class TestDescription_TestBignum_test_from_string : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_from_string() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 142, "test_from_string" ) {}
 void runTest() { suite_TestBignum.test_from_string(); }
} testDescription_TestBignum_test_from_string;

static class TestDescription_TestBignum_test_size : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_size() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 148, "test_size" ) {}
 void runTest() { suite_TestBignum.test_size(); }
} testDescription_TestBignum_test_size;

static class TestDescription_TestBignum_test_hash_bignum : public CxxTest::RealTestDescription {
public:
 TestDescription_TestBignum_test_hash_bignum() : CxxTest::RealTestDescription( Tests_TestBignum, suiteDescription_TestBignum, 154, "test_hash_bignum" ) {}
 void runTest() { suite_TestBignum.test_hash_bignum(); }
} testDescription_TestBignum_test_hash_bignum;

#include "test_hash.hpp"

static TestHash suite_TestHash;

static CxxTest::List Tests_TestHash = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestHash( "test_hash.hpp", 8, "TestHash", suite_TestHash, Tests_TestHash );

static class TestDescription_TestHash_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_create() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 38, "test_create" ) {}
 void runTest() { suite_TestHash.test_create(); }
} testDescription_TestHash_test_create;

static class TestDescription_TestHash_test_dup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_dup() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 42, "test_dup" ) {}
 void runTest() { suite_TestHash.test_dup(); }
} testDescription_TestHash_test_dup;

static class TestDescription_TestHash_test_add_and_get : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_add_and_get() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 52, "test_add_and_get" ) {}
 void runTest() { suite_TestHash.test_add_and_get(); }
} testDescription_TestHash_test_add_and_get;

static class TestDescription_TestHash_test_add_on_chained_buckets : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_add_on_chained_buckets() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 60, "test_add_on_chained_buckets" ) {}
 void runTest() { suite_TestHash.test_add_on_chained_buckets(); }
} testDescription_TestHash_test_add_on_chained_buckets;

static class TestDescription_TestHash_test_add_replaces_on_same_hash : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_add_replaces_on_same_hash() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 78, "test_add_replaces_on_same_hash" ) {}
 void runTest() { suite_TestHash.test_add_replaces_on_same_hash(); }
} testDescription_TestHash_test_add_replaces_on_same_hash;

static class TestDescription_TestHash_test_set : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_set() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 86, "test_set" ) {}
 void runTest() { suite_TestHash.test_set(); }
} testDescription_TestHash_test_set;

static class TestDescription_TestHash_test_get_returns_nil_on_not_found : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_get_returns_nil_on_not_found() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 92, "test_get_returns_nil_on_not_found" ) {}
 void runTest() { suite_TestHash.test_get_returns_nil_on_not_found(); }
} testDescription_TestHash_test_get_returns_nil_on_not_found;

static class TestDescription_TestHash_test_lookup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_lookup() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 96, "test_lookup" ) {}
 void runTest() { suite_TestHash.test_lookup(); }
} testDescription_TestHash_test_lookup;

static class TestDescription_TestHash_test_lookup2 : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_lookup2() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 123, "test_lookup2" ) {}
 void runTest() { suite_TestHash.test_lookup2(); }
} testDescription_TestHash_test_lookup2;

static class TestDescription_TestHash_test_assign : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_assign() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 138, "test_assign" ) {}
 void runTest() { suite_TestHash.test_assign(); }
} testDescription_TestHash_test_assign;

static class TestDescription_TestHash_test_assign_redistributes : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_assign_redistributes() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 149, "test_assign_redistributes" ) {}
 void runTest() { suite_TestHash.test_assign_redistributes(); }
} testDescription_TestHash_test_assign_redistributes;

static class TestDescription_TestHash_test_get_undef : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_get_undef() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 160, "test_get_undef" ) {}
 void runTest() { suite_TestHash.test_get_undef(); }
} testDescription_TestHash_test_get_undef;

static class TestDescription_TestHash_test_remove : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_remove() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 167, "test_remove" ) {}
 void runTest() { suite_TestHash.test_remove(); }
} testDescription_TestHash_test_remove;

static class TestDescription_TestHash_test_remove_from_chained_bins : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_remove_from_chained_bins() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 174, "test_remove_from_chained_bins" ) {}
 void runTest() { suite_TestHash.test_remove_from_chained_bins(); }
} testDescription_TestHash_test_remove_from_chained_bins;

static class TestDescription_TestHash_test_from_tuple : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_from_tuple() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 183, "test_from_tuple" ) {}
 void runTest() { suite_TestHash.test_from_tuple(); }
} testDescription_TestHash_test_from_tuple;

static class TestDescription_TestHash_test_csm_new : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_csm_new() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 196, "test_csm_new" ) {}
 void runTest() { suite_TestHash.test_csm_new(); }
} testDescription_TestHash_test_csm_new;

static class TestDescription_TestHash_test_csm_add_and_find : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_csm_add_and_find() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 201, "test_csm_add_and_find" ) {}
 void runTest() { suite_TestHash.test_csm_add_and_find(); }
} testDescription_TestHash_test_csm_add_and_find;

static class TestDescription_TestHash_test_csm_add_returns_false_on_full : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_csm_add_returns_false_on_full() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 210, "test_csm_add_returns_false_on_full" ) {}
 void runTest() { suite_TestHash.test_csm_add_returns_false_on_full(); }
} testDescription_TestHash_test_csm_add_returns_false_on_full;

static class TestDescription_TestHash_test_csm_into_hash : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_csm_into_hash() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 218, "test_csm_into_hash" ) {}
 void runTest() { suite_TestHash.test_csm_into_hash(); }
} testDescription_TestHash_test_csm_into_hash;

static class TestDescription_TestHash_test_csm_into_lookuptable : public CxxTest::RealTestDescription {
public:
 TestDescription_TestHash_test_csm_into_lookuptable() : CxxTest::RealTestDescription( Tests_TestHash, suiteDescription_TestHash, 228, "test_csm_into_lookuptable" ) {}
 void runTest() { suite_TestHash.test_csm_into_lookuptable(); }
} testDescription_TestHash_test_csm_into_lookuptable;

#include "test_lookuptable.hpp"

static TestLookupTable suite_TestLookupTable;

static CxxTest::List Tests_TestLookupTable = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestLookupTable( "test_lookuptable.hpp", 8, "TestLookupTable", suite_TestLookupTable, Tests_TestLookupTable );

static class TestDescription_TestLookupTable_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_create() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 13, "test_create" ) {}
 void runTest() { suite_TestLookupTable.test_create(); }
} testDescription_TestLookupTable_test_create;

static class TestDescription_TestLookupTable_test_store_fetch : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_store_fetch() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 21, "test_store_fetch" ) {}
 void runTest() { suite_TestLookupTable.test_store_fetch(); }
} testDescription_TestLookupTable_test_store_fetch;

static class TestDescription_TestLookupTable_test_store_overwrites_previous : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_store_overwrites_previous() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 34, "test_store_overwrites_previous" ) {}
 void runTest() { suite_TestLookupTable.test_store_overwrites_previous(); }
} testDescription_TestLookupTable_test_store_overwrites_previous;

static class TestDescription_TestLookupTable_test_store_handles_entries_in_same_bin : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_store_handles_entries_in_same_bin() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 53, "test_store_handles_entries_in_same_bin" ) {}
 void runTest() { suite_TestLookupTable.test_store_handles_entries_in_same_bin(); }
} testDescription_TestLookupTable_test_store_handles_entries_in_same_bin;

static class TestDescription_TestLookupTable_test_store_resizes_table : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_store_resizes_table() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 81, "test_store_resizes_table" ) {}
 void runTest() { suite_TestLookupTable.test_store_resizes_table(); }
} testDescription_TestLookupTable_test_store_resizes_table;

static class TestDescription_TestLookupTable_test_store_resizes_table_with_chained_bins : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_store_resizes_table_with_chained_bins() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 102, "test_store_resizes_table_with_chained_bins" ) {}
 void runTest() { suite_TestLookupTable.test_store_resizes_table_with_chained_bins(); }
} testDescription_TestLookupTable_test_store_resizes_table_with_chained_bins;

static class TestDescription_TestLookupTable_test_find_entry : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_find_entry() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 124, "test_find_entry" ) {}
 void runTest() { suite_TestLookupTable.test_find_entry(); }
} testDescription_TestLookupTable_test_find_entry;

static class TestDescription_TestLookupTable_test_find : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_find() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 138, "test_find" ) {}
 void runTest() { suite_TestLookupTable.test_find(); }
} testDescription_TestLookupTable_test_find;

static class TestDescription_TestLookupTable_test_remove : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_remove() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 152, "test_remove" ) {}
 void runTest() { suite_TestLookupTable.test_remove(); }
} testDescription_TestLookupTable_test_remove;

static class TestDescription_TestLookupTable_test_remove_redistributes : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_remove_redistributes() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 170, "test_remove_redistributes" ) {}
 void runTest() { suite_TestLookupTable.test_remove_redistributes(); }
} testDescription_TestLookupTable_test_remove_redistributes;

static class TestDescription_TestLookupTable_test_remove_works_for_chained_bins : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_remove_works_for_chained_bins() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 192, "test_remove_works_for_chained_bins" ) {}
 void runTest() { suite_TestLookupTable.test_remove_works_for_chained_bins(); }
} testDescription_TestLookupTable_test_remove_works_for_chained_bins;

static class TestDescription_TestLookupTable_test_remove_works_for_unknown_key : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_remove_works_for_unknown_key() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 212, "test_remove_works_for_unknown_key" ) {}
 void runTest() { suite_TestLookupTable.test_remove_works_for_unknown_key(); }
} testDescription_TestLookupTable_test_remove_works_for_unknown_key;

static class TestDescription_TestLookupTable_test_has_key : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_has_key() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 223, "test_has_key" ) {}
 void runTest() { suite_TestLookupTable.test_has_key(); }
} testDescription_TestLookupTable_test_has_key;

static class TestDescription_TestLookupTable_test_dup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_dup() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 237, "test_dup" ) {}
 void runTest() { suite_TestLookupTable.test_dup(); }
} testDescription_TestLookupTable_test_dup;

static class TestDescription_TestLookupTable_test_all_keys : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_all_keys() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 251, "test_all_keys" ) {}
 void runTest() { suite_TestLookupTable.test_all_keys(); }
} testDescription_TestLookupTable_test_all_keys;

static class TestDescription_TestLookupTable_test_all_values : public CxxTest::RealTestDescription {
public:
 TestDescription_TestLookupTable_test_all_values() : CxxTest::RealTestDescription( Tests_TestLookupTable, suiteDescription_TestLookupTable, 265, "test_all_values" ) {}
 void runTest() { suite_TestLookupTable.test_all_values(); }
} testDescription_TestLookupTable_test_all_values;

#include "test_object.hpp"

static TestObject suite_TestObject;

static CxxTest::List Tests_TestObject = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestObject( "test_object.hpp", 9, "TestObject", suite_TestObject, Tests_TestObject );

static class TestDescription_TestObject_test_i2n : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_i2n() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 23, "test_i2n" ) {}
 void runTest() { suite_TestObject.test_i2n(); }
} testDescription_TestObject_test_i2n;

static class TestDescription_TestObject_test_ui2n : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_ui2n() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 33, "test_ui2n" ) {}
 void runTest() { suite_TestObject.test_ui2n(); }
} testDescription_TestObject_test_ui2n;

static class TestDescription_TestObject_test_dup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_dup() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 37, "test_dup" ) {}
 void runTest() { suite_TestObject.test_dup(); }
} testDescription_TestObject_test_dup;

static class TestDescription_TestObject_test_dup_bytes : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_dup_bytes() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 46, "test_dup_bytes" ) {}
 void runTest() { suite_TestObject.test_dup_bytes(); }
} testDescription_TestObject_test_dup_bytes;

static class TestDescription_TestObject_test_kind_of_p : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_kind_of_p() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 58, "test_kind_of_p" ) {}
 void runTest() { suite_TestObject.test_kind_of_p(); }
} testDescription_TestObject_test_kind_of_p;

static class TestDescription_TestObject_test_hash : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_hash() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 65, "test_hash" ) {}
 void runTest() { suite_TestObject.test_hash(); }
} testDescription_TestObject_test_hash;

static class TestDescription_TestObject_test_metaclass : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_metaclass() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 70, "test_metaclass" ) {}
 void runTest() { suite_TestObject.test_metaclass(); }
} testDescription_TestObject_test_metaclass;

static class TestDescription_TestObject_test_get_ivar : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_get_ivar() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 83, "test_get_ivar" ) {}
 void runTest() { suite_TestObject.test_get_ivar(); }
} testDescription_TestObject_test_get_ivar;

static class TestDescription_TestObject_test_id : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObject_test_id() : CxxTest::RealTestDescription( Tests_TestObject, suiteDescription_TestObject, 95, "test_id" ) {}
 void runTest() { suite_TestObject.test_id(); }
} testDescription_TestObject_test_id;

#include "test_objectmemory.hpp"

static TestObjectMemory suite_TestObjectMemory;

static CxxTest::List Tests_TestObjectMemory = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestObjectMemory( "test_objectmemory.hpp", 8, "TestObjectMemory", suite_TestObjectMemory, Tests_TestObjectMemory );

static class TestDescription_TestObjectMemory_test_new_object : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_new_object() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 11, "test_new_object" ) {}
 void runTest() { suite_TestObjectMemory.test_new_object(); }
} testDescription_TestObjectMemory_test_new_object;

static class TestDescription_TestObjectMemory_test_write_barrier : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_write_barrier() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 28, "test_write_barrier" ) {}
 void runTest() { suite_TestObjectMemory.test_write_barrier(); }
} testDescription_TestObjectMemory_test_write_barrier;

static class TestDescription_TestObjectMemory_test_write_barrier_not_called_for_immediates : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_write_barrier_not_called_for_immediates() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 50, "test_write_barrier_not_called_for_immediates" ) {}
 void runTest() { suite_TestObjectMemory.test_write_barrier_not_called_for_immediates(); }
} testDescription_TestObjectMemory_test_write_barrier_not_called_for_immediates;

static class TestDescription_TestObjectMemory_test_collect_young : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 63, "test_collect_young" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young(); }
} testDescription_TestObjectMemory_test_collect_young;

static class TestDescription_TestObjectMemory_test_collect_young_through_references : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_through_references() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 91, "test_collect_young_through_references" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_through_references(); }
} testDescription_TestObjectMemory_test_collect_young_through_references;

static class TestDescription_TestObjectMemory_test_collect_young_skips_byte_storage : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_skips_byte_storage() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 121, "test_collect_young_skips_byte_storage" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_skips_byte_storage(); }
} testDescription_TestObjectMemory_test_collect_young_skips_byte_storage;

static class TestDescription_TestObjectMemory_test_new_object_mature : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_new_object_mature() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 141, "test_new_object_mature" ) {}
 void runTest() { suite_TestObjectMemory.test_new_object_mature(); }
} testDescription_TestObjectMemory_test_new_object_mature;

static class TestDescription_TestObjectMemory_test_new_large_object : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_new_large_object() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 158, "test_new_large_object" ) {}
 void runTest() { suite_TestObjectMemory.test_new_large_object(); }
} testDescription_TestObjectMemory_test_new_large_object;

static class TestDescription_TestObjectMemory_test_collect_young_doesnt_move_mature_objects : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_doesnt_move_mature_objects() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 172, "test_collect_young_doesnt_move_mature_objects" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_doesnt_move_mature_objects(); }
} testDescription_TestObjectMemory_test_collect_young_doesnt_move_mature_objects;

static class TestDescription_TestObjectMemory_test_collect_young_uses_remember_set : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_uses_remember_set() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 188, "test_collect_young_uses_remember_set" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_uses_remember_set(); }
} testDescription_TestObjectMemory_test_collect_young_uses_remember_set;

static class TestDescription_TestObjectMemory_test_collect_young_promotes_objects : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_promotes_objects() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 208, "test_collect_young_promotes_objects" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_promotes_objects(); }
} testDescription_TestObjectMemory_test_collect_young_promotes_objects;

static class TestDescription_TestObjectMemory_test_collect_young_resets_remember_set : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_resets_remember_set() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 229, "test_collect_young_resets_remember_set" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_resets_remember_set(); }
} testDescription_TestObjectMemory_test_collect_young_resets_remember_set;

static class TestDescription_TestObjectMemory_test_collect_young_uses_forwarding_pointers : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_uses_forwarding_pointers() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 257, "test_collect_young_uses_forwarding_pointers" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_uses_forwarding_pointers(); }
} testDescription_TestObjectMemory_test_collect_young_uses_forwarding_pointers;

static class TestDescription_TestObjectMemory_test_collect_young_copies_byte_bodies : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_copies_byte_bodies() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 280, "test_collect_young_copies_byte_bodies" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_copies_byte_bodies(); }
} testDescription_TestObjectMemory_test_collect_young_copies_byte_bodies;

static class TestDescription_TestObjectMemory_test_collect_young_class_considered : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_class_considered() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 296, "test_collect_young_class_considered" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_class_considered(); }
} testDescription_TestObjectMemory_test_collect_young_class_considered;

static class TestDescription_TestObjectMemory_test_collect_mature : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_mature() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 313, "test_collect_mature" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_mature(); }
} testDescription_TestObjectMemory_test_collect_mature;

static class TestDescription_TestObjectMemory_test_collect_mature_marks_young_objects : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_mature_marks_young_objects() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 333, "test_collect_mature_marks_young_objects" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_mature_marks_young_objects(); }
} testDescription_TestObjectMemory_test_collect_mature_marks_young_objects;

static class TestDescription_TestObjectMemory_test_collect_mature_stops_at_already_marked_objects : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_mature_stops_at_already_marked_objects() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 354, "test_collect_mature_stops_at_already_marked_objects" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_mature_stops_at_already_marked_objects(); }
} testDescription_TestObjectMemory_test_collect_mature_stops_at_already_marked_objects;

static class TestDescription_TestObjectMemory_test_collect_young_stops_at_already_marked_objects : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_stops_at_already_marked_objects() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 377, "test_collect_young_stops_at_already_marked_objects" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_stops_at_already_marked_objects(); }
} testDescription_TestObjectMemory_test_collect_young_stops_at_already_marked_objects;

static class TestDescription_TestObjectMemory_test_collect_young_tells_objectmemory_about_collection : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_young_tells_objectmemory_about_collection() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 401, "test_collect_young_tells_objectmemory_about_collection" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_young_tells_objectmemory_about_collection(); }
} testDescription_TestObjectMemory_test_collect_young_tells_objectmemory_about_collection;

static class TestDescription_TestObjectMemory_test_new_young_spills_to_mature : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_new_young_spills_to_mature() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 416, "test_new_young_spills_to_mature" ) {}
 void runTest() { suite_TestObjectMemory.test_new_young_spills_to_mature(); }
} testDescription_TestObjectMemory_test_new_young_spills_to_mature;

static class TestDescription_TestObjectMemory_test_collect_mature_tells_objectmemory_about_collection : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_collect_mature_tells_objectmemory_about_collection() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 430, "test_collect_mature_tells_objectmemory_about_collection" ) {}
 void runTest() { suite_TestObjectMemory.test_collect_mature_tells_objectmemory_about_collection(); }
} testDescription_TestObjectMemory_test_collect_mature_tells_objectmemory_about_collection;

static class TestDescription_TestObjectMemory_test_valid_object_p : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjectMemory_test_valid_object_p() : CxxTest::RealTestDescription( Tests_TestObjectMemory, suiteDescription_TestObjectMemory, 448, "test_valid_object_p" ) {}
 void runTest() { suite_TestObjectMemory.test_valid_object_p(); }
} testDescription_TestObjectMemory_test_valid_object_p;

#include "test_objects.hpp"

static TestObjects suite_TestObjects;

static CxxTest::List Tests_TestObjects = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestObjects( "test_objects.hpp", 8, "TestObjects", suite_TestObjects, Tests_TestObjects );

static class TestDescription_TestObjects_test_object : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_object() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 13, "test_object" ) {}
 void runTest() { suite_TestObjects.test_object(); }
} testDescription_TestObjects_test_object;

static class TestDescription_TestObjects_test_class : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_class() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 20, "test_class" ) {}
 void runTest() { suite_TestObjects.test_class(); }
} testDescription_TestObjects_test_class;

static class TestDescription_TestObjects_test_metaclass_is_completely_setup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_metaclass_is_completely_setup() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 30, "test_metaclass_is_completely_setup" ) {}
 void runTest() { suite_TestObjects.test_metaclass_is_completely_setup(); }
} testDescription_TestObjects_test_metaclass_is_completely_setup;

static class TestDescription_TestObjects_test_module : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_module() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 42, "test_module" ) {}
 void runTest() { suite_TestObjects.test_module(); }
} testDescription_TestObjects_test_module;

static class TestDescription_TestObjects_test_metaclass : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_metaclass() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 52, "test_metaclass" ) {}
 void runTest() { suite_TestObjects.test_metaclass(); }
} testDescription_TestObjects_test_metaclass;

static class TestDescription_TestObjects_test_tuple : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_tuple() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 64, "test_tuple" ) {}
 void runTest() { suite_TestObjects.test_tuple(); }
} testDescription_TestObjects_test_tuple;

static class TestDescription_TestObjects_test_lookuptable : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_lookuptable() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 76, "test_lookuptable" ) {}
 void runTest() { suite_TestObjects.test_lookuptable(); }
} testDescription_TestObjects_test_lookuptable;

static class TestDescription_TestObjects_test_methtbl : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_methtbl() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 88, "test_methtbl" ) {}
 void runTest() { suite_TestObjects.test_methtbl(); }
} testDescription_TestObjects_test_methtbl;

static class TestDescription_TestObjects_test_symbol : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_symbol() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 101, "test_symbol" ) {}
 void runTest() { suite_TestObjects.test_symbol(); }
} testDescription_TestObjects_test_symbol;

static class TestDescription_TestObjects_test_array : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_array() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 112, "test_array" ) {}
 void runTest() { suite_TestObjects.test_array(); }
} testDescription_TestObjects_test_array;

static class TestDescription_TestObjects_test_bytearray : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_bytearray() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 123, "test_bytearray" ) {}
 void runTest() { suite_TestObjects.test_bytearray(); }
} testDescription_TestObjects_test_bytearray;

static class TestDescription_TestObjects_test_string : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_string() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 135, "test_string" ) {}
 void runTest() { suite_TestObjects.test_string(); }
} testDescription_TestObjects_test_string;

static class TestDescription_TestObjects_test_symtbl : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_symtbl() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 147, "test_symtbl" ) {}
 void runTest() { suite_TestObjects.test_symtbl(); }
} testDescription_TestObjects_test_symtbl;

static class TestDescription_TestObjects_test_cmethod : public CxxTest::RealTestDescription {
public:
 TestDescription_TestObjects_test_cmethod() : CxxTest::RealTestDescription( Tests_TestObjects, suiteDescription_TestObjects, 158, "test_cmethod" ) {}
 void runTest() { suite_TestObjects.test_cmethod(); }
} testDescription_TestObjects_test_cmethod;

#include "test_regexp.hpp"

static TestRegexp suite_TestRegexp;

static CxxTest::List Tests_TestRegexp = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestRegexp( "test_regexp.hpp", 8, "TestRegexp", suite_TestRegexp, Tests_TestRegexp );

static class TestDescription_TestRegexp_test_version : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_version() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 23, "test_version" ) {}
 void runTest() { suite_TestRegexp.test_version(); }
} testDescription_TestRegexp_test_version;

static class TestDescription_TestRegexp_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_create() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 28, "test_create" ) {}
 void runTest() { suite_TestRegexp.test_create(); }
} testDescription_TestRegexp_test_create;

static class TestDescription_TestRegexp_test_create_with_named_captures : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_create_with_named_captures() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 35, "test_create_with_named_captures" ) {}
 void runTest() { suite_TestRegexp.test_create_with_named_captures(); }
} testDescription_TestRegexp_test_create_with_named_captures;

static class TestDescription_TestRegexp_test_create_with_bad_pattern : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_create_with_bad_pattern() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 42, "test_create_with_bad_pattern" ) {}
 void runTest() { suite_TestRegexp.test_create_with_bad_pattern(); }
} testDescription_TestRegexp_test_create_with_bad_pattern;

static class TestDescription_TestRegexp_test_options : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_options() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 52, "test_options" ) {}
 void runTest() { suite_TestRegexp.test_options(); }
} testDescription_TestRegexp_test_options;

static class TestDescription_TestRegexp_test_match_region : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_match_region() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 59, "test_match_region" ) {}
 void runTest() { suite_TestRegexp.test_match_region(); }
} testDescription_TestRegexp_test_match_region;

static class TestDescription_TestRegexp_test_match_region_with_captures : public CxxTest::RealTestDescription {
public:
 TestDescription_TestRegexp_test_match_region_with_captures() : CxxTest::RealTestDescription( Tests_TestRegexp, suiteDescription_TestRegexp, 76, "test_match_region_with_captures" ) {}
 void runTest() { suite_TestRegexp.test_match_region_with_captures(); }
} testDescription_TestRegexp_test_match_region_with_captures;

#include "test_string.hpp"

static TestString suite_TestString;

static CxxTest::List Tests_TestString = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestString( "test_string.hpp", 8, "TestString", suite_TestString, Tests_TestString );

static class TestDescription_TestString_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_TestString_test_create() : CxxTest::RealTestDescription( Tests_TestString, suiteDescription_TestString, 23, "test_create" ) {}
 void runTest() { suite_TestString.test_create(); }
} testDescription_TestString_test_create;

static class TestDescription_TestString_test_create2 : public CxxTest::RealTestDescription {
public:
 TestDescription_TestString_test_create2() : CxxTest::RealTestDescription( Tests_TestString, suiteDescription_TestString, 30, "test_create2" ) {}
 void runTest() { suite_TestString.test_create2(); }
} testDescription_TestString_test_create2;

static class TestDescription_TestString_test_hash_string : public CxxTest::RealTestDescription {
public:
 TestDescription_TestString_test_hash_string() : CxxTest::RealTestDescription( Tests_TestString, suiteDescription_TestString, 35, "test_hash_string" ) {}
 void runTest() { suite_TestString.test_hash_string(); }
} testDescription_TestString_test_hash_string;

static class TestDescription_TestString_test_to_sym : public CxxTest::RealTestDescription {
public:
 TestDescription_TestString_test_to_sym() : CxxTest::RealTestDescription( Tests_TestString, suiteDescription_TestString, 45, "test_to_sym" ) {}
 void runTest() { suite_TestString.test_to_sym(); }
} testDescription_TestString_test_to_sym;

#include "test_symbol.hpp"

static TestSymbol suite_TestSymbol;

static CxxTest::List Tests_TestSymbol = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestSymbol( "test_symbol.hpp", 9, "TestSymbol", suite_TestSymbol, Tests_TestSymbol );

static class TestDescription_TestSymbol_test_lookup : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_lookup() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 23, "test_lookup" ) {}
 void runTest() { suite_TestSymbol.test_lookup(); }
} testDescription_TestSymbol_test_lookup;

static class TestDescription_TestSymbol_test_lookup_colliding_hash : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_lookup_colliding_hash() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 31, "test_lookup_colliding_hash" ) {}
 void runTest() { suite_TestSymbol.test_lookup_colliding_hash(); }
} testDescription_TestSymbol_test_lookup_colliding_hash;

static class TestDescription_TestSymbol_test_lookup_colliding_hash2 : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_lookup_colliding_hash2() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 40, "test_lookup_colliding_hash2" ) {}
 void runTest() { suite_TestSymbol.test_lookup_colliding_hash2(); }
} testDescription_TestSymbol_test_lookup_colliding_hash2;

static class TestDescription_TestSymbol_test_lookup_enlarges_table : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_lookup_enlarges_table() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 54, "test_lookup_enlarges_table" ) {}
 void runTest() { suite_TestSymbol.test_lookup_enlarges_table(); }
} testDescription_TestSymbol_test_lookup_enlarges_table;

static class TestDescription_TestSymbol_test_find_string : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_find_string() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 66, "test_find_string" ) {}
 void runTest() { suite_TestSymbol.test_find_string(); }
} testDescription_TestSymbol_test_find_string;

static class TestDescription_TestSymbol_test_to_str : public CxxTest::RealTestDescription {
public:
 TestDescription_TestSymbol_test_to_str() : CxxTest::RealTestDescription( Tests_TestSymbol, suiteDescription_TestSymbol, 74, "test_to_str" ) {}
 void runTest() { suite_TestSymbol.test_to_str(); }
} testDescription_TestSymbol_test_to_str;

#include "test_vm.hpp"

static TestVM suite_TestVM;

static CxxTest::List Tests_TestVM = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_TestVM( "test_vm.hpp", 9, "TestVM", suite_TestVM, Tests_TestVM );

static class TestDescription_TestVM_test_symbol : public CxxTest::RealTestDescription {
public:
 TestDescription_TestVM_test_symbol() : CxxTest::RealTestDescription( Tests_TestVM, suiteDescription_TestVM, 23, "test_symbol" ) {}
 void runTest() { suite_TestVM.test_symbol(); }
} testDescription_TestVM_test_symbol;

static class TestDescription_TestVM_test_new_object_uses_field_count_from_class : public CxxTest::RealTestDescription {
public:
 TestDescription_TestVM_test_new_object_uses_field_count_from_class() : CxxTest::RealTestDescription( Tests_TestVM, suiteDescription_TestVM, 30, "test_new_object_uses_field_count_from_class" ) {}
 void runTest() { suite_TestVM.test_new_object_uses_field_count_from_class(); }
} testDescription_TestVM_test_new_object_uses_field_count_from_class;

#include <cxxtest/Root.cpp>
