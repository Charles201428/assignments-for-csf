#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero;
  UInt256 one;
  UInt256 large1;
  UInt256 huge;

  const char *hex1;
  const char *hex2;
  const char *hex3;
  const char *hex4;
  const char *hex5;
  const char *hex6;
  const char *hex7;
  const char *hex8;
} TestObjs;

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Helper functions for implementing tests
int check(UInt256 val, uint64_t val3, uint64_t val2, uint64_t val1, uint64_t val0);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u64(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add_1(TestObjs *objs);
void test_add_2(TestObjs *objs);
void test_add_3(TestObjs *objs);
void test_add_4(TestObjs *objs);
void test_sub_1(TestObjs *objs);
void test_sub_2(TestObjs *objs);
void test_sub_3(TestObjs *objs);
void test_sub_4(TestObjs *objs);
void test_sub_5(TestObjs *objs);
void test_mul_1(TestObjs *objs);
void test_mul_2(TestObjs *objs);
void test_mul_3(TestObjs *objs);
void test_mul_4(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u64);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add_1);
  TEST(test_add_2);
  TEST(test_add_3);
  TEST(test_add_4);
  TEST(test_sub_1);
  TEST(test_sub_2);
  TEST(test_sub_3);
  TEST(test_sub_4);
  TEST(test_sub_5);
  TEST(test_mul_1);
  TEST(test_mul_2);
  TEST(test_mul_3);
  TEST(test_mul_4);

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = (TestObjs *) malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"

  objs->zero.data[3] = 0U;
  objs->zero.data[2] = 0U;
  objs->zero.data[1] = 0U;
  objs->zero.data[0] = 0U;

  objs->one.data[3] = 0U;
  objs->one.data[2] = 0U;
  objs->one.data[1] = 0U;
  objs->one.data[0] = 1U;

  objs->large1.data[3] = 0xAA;
  objs->large1.data[2] = 0xBB;
  objs->large1.data[1] = 0xCC;
  objs->large1.data[0] = 0xDD;

  objs->huge.data[3] = 0xFFFFFFFFFFFFFFFF;
  objs->huge.data[2] = 0xFFFFFFFFFFFFFFFF;
  objs->huge.data[1] = 0xFFFFFFFFFFFFFFFF;
  objs->huge.data[0] = 0xFFFFFFFFFFFFFFFF;

  // example hex strings

  objs->hex1 = "0";
  objs->hex2 = "cafe";
  objs->hex3 = "4a4b72ebb654226ef77ed83d884f4940e4243bc3913ceaf5781b28d25fb00b0";
  objs->hex4 = "bcfeeac";
  objs->hex5 = "10000000000000000";
  objs->hex6 = "1000000000000000000000000000000000000000000000000000000000000000";
  objs->hex7 = "11000000000000000000000000000000000000000000000000000000000000000";
  objs->hex8 = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

int check(UInt256 val, uint64_t val3, uint64_t val2, uint64_t val1, uint64_t val0) {
  return uint256_get_bits(val, 3) == val3
      && uint256_get_bits(val, 2) == val2
      && uint256_get_bits(val, 1) == val1
      && uint256_get_bits(val, 0) == val0;
}

void test_get_bits(TestObjs *objs) {
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 0));

  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(1U == uint256_get_bits(objs->one, 0));
}

void test_create_from_u64(TestObjs *objs) {
  objs->zero = uint256_create_from_u64(0U);
  objs->one = uint256_create_from_u64(1U);

  ASSERT(check(objs->zero, 0U, 0U, 0U, 0U));
  ASSERT(check(objs->one, 0U, 0U, 0U, 1U));
}

void test_create(TestObjs *objs) {
  // Note that the initializers list the uint64_t values to
  // be stored in order from least significant to most significant
  uint64_t zero_init[] =   {0U, 0U, 0U, 0U};
  uint64_t one_init[] =    {1U, 0U, 0U, 0U};

  objs->zero = uint256_create(zero_init);
  objs->one = uint256_create(one_init);

  // In the calls to check, the uint64_t values are specified
  // in the order most significant to least significant
  // (i.e., the reverse of the order in the array passed to
  // uint256_create)
  ASSERT(check(objs->zero, 0U, 0U, 0U, 0U));
  ASSERT(check(objs->one, 0U, 0U, 0U, 1U));
}

void test_create_from_hex(TestObjs *objs) {
  UInt256 val;

  val = uint256_create_from_hex(objs->hex1);
  ASSERT(check(val, 0x0UL, 0x0UL, 0x0UL, 0x0UL));

  val = uint256_create_from_hex(objs->hex2);
  ASSERT(check(val, 0x0UL, 0x0UL, 0x0UL, 0xcafeUL));

  val = uint256_create_from_hex(objs->hex3);
  ASSERT(check(val, 0x4a4b72ebb654226UL, 0xef77ed83d884f494UL, 0x0e4243bc3913ceafUL, 0x5781b28d25fb00b0UL));

  val = uint256_create_from_hex(objs->hex4);
  ASSERT(check(val, 0x0UL, 0x0UL, 0x0UL, 0xbcfeeacUL));
  
  //edge case: when the val1 is 0, while all the others are zero
  val = uint256_create_from_hex(objs->hex5);
  ASSERT(check(val, 0x0UL, 0x0UL, 0x1UL, 0x0UL));

  val = uint256_create_from_hex(objs->hex6);
  ASSERT(check(val, 0x1000000000000000UL, 0x0UL, 0x0UL, 0x0UL));

  //edge case: when the string’s length is more than 64 characters, only use rightmost 64 hex digits
  val = uint256_create_from_hex(objs->hex7);
  ASSERT(check(val, 0x1000000000000000UL, 0x0UL, 0x0UL, 0x0UL));

  //edge case: test the largest number
  val = uint256_create_from_hex(objs->hex8);
  ASSERT(check(val, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL));
 
}



void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  // test whether resulting hex string will not have unnecessary leading 0 digits
  s = uint256_format_as_hex(objs->large1);
  ASSERT(0 == strcmp("aa00000000000000bb00000000000000cc00000000000000dd", s));
  free(s);

  // edge case: test the largest number
  s = uint256_format_as_hex(objs->huge);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);

}

void test_add_1(TestObjs *objs) {
  // basic addition tests

  UInt256 sum;

  sum = uint256_add(objs->zero, objs->one);

  ASSERT(0UL == sum.data[3]);
  ASSERT(0UL == sum.data[2]);
  ASSERT(0UL == sum.data[1]);
  ASSERT(1UL == sum.data[0]);
}

void test_add_2(TestObjs *objs) {
  // "medium" addition tests

  (void) objs;

  UInt256 left, right, result;

  // a932467b27ccb2b4cb6c96c4810cb5e + b37aef92a5179d60af2f9cad0a4b38a = 15cad360dcce450157a9c33718b57ee8
  left.data[0] = 0x4cb6c96c4810cb5eUL;
  left.data[1] = 0xa932467b27ccb2bUL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0x0af2f9cad0a4b38aUL;
  right.data[1] = 0xb37aef92a5179d6UL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_add(left, right);
  ASSERT(0x57a9c33718b57ee8UL == result.data[0]);
  ASSERT(0x15cad360dcce4501UL == result.data[1]);
  ASSERT(0x0UL == result.data[2]);
  ASSERT(0x0UL == result.data[3]);
}

void test_add_3(TestObjs *objs) {
  // "large" addition tests

  (void) objs;

  UInt256 left, right, result;

  // 9515af0631ecc4779e0c122009e87b1ff1076115dab87f8190bc8c2ceb84ceb + 173ba2210b102e74dc94683c12d20ceda74af7d4cf5fc80aa154504e7e58712 = ac5151273cfcf2ec7aa07a5c1cba880d985258eaaa18478c3210dc7b69dd3fd
  left.data[0] = 0x190bc8c2ceb84cebUL;
  left.data[1] = 0xff1076115dab87f8UL;
  left.data[2] = 0x79e0c122009e87b1UL;
  left.data[3] = 0x9515af0631ecc47UL;
  right.data[0] = 0xaa154504e7e58712UL;
  right.data[1] = 0xda74af7d4cf5fc80UL;
  right.data[2] = 0x4dc94683c12d20ceUL;
  right.data[3] = 0x173ba2210b102e7UL;
  result = uint256_add(left, right);
  ASSERT(0xc3210dc7b69dd3fdUL == result.data[0]);
  ASSERT(0xd985258eaaa18478UL == result.data[1]);
  ASSERT(0xc7aa07a5c1cba880UL == result.data[2]);
  ASSERT(0xac5151273cfcf2eUL == result.data[3]);
}


void test_add_4(TestObjs *objs) {
  // "another large" addition tests

  (void) objs;

  UInt256 left, right, result;

  // 713d3c74065a911fee8027e6ff3d6621f38d52e9636eca90324f9c1da16ea8f + 4540b2bd68d0300e607e2ded97b39c5df75b078fb51bac00948b182a75be9a4 = b67def316f2ac12e4efe55d496f1027feae85a79188a7690c6dab448172d433
  left.data[0] = 0x0324f9c1da16ea8fUL;
  left.data[1] = 0x1f38d52e9636eca9UL;
  left.data[2] = 0xfee8027e6ff3d662UL;
  left.data[3] = 0x713d3c74065a911UL;
  right.data[0] = 0x0948b182a75be9a4UL;
  right.data[1] = 0xdf75b078fb51bac0UL;
  right.data[2] = 0xe607e2ded97b39c5UL;
  right.data[3] = 0x4540b2bd68d0300UL;
  result = uint256_add(left, right);
  ASSERT(0x0c6dab448172d433UL == result.data[0]);
  ASSERT(0xfeae85a79188a769UL == result.data[1]);
  ASSERT(0xe4efe55d496f1027UL == result.data[2]);
  ASSERT(0xb67def316f2ac12UL == result.data[3]);
}

void test_sub_1(TestObjs *objs) {
  // basic subtraction tests

  UInt256 result;

  result = uint256_sub(objs->one, objs->zero);
  ASSERT(0UL == result.data[3]);
  ASSERT(0UL == result.data[2]);
  ASSERT(0UL == result.data[1]);
  ASSERT(1UL == result.data[0]);
}

void test_sub_2(TestObjs *objs) {
  // "medium" subtraction tests

  (void) objs;

  UInt256 left, right, result;

  // af7939833038cfa010f734368aaad66 - 6473865d77af47d71f7aedf64364960 = 4b05b325b88987c8f17c46404746406
  left.data[0] = 0x010f734368aaad66UL;
  left.data[1] = 0xaf7939833038cfaUL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0x71f7aedf64364960UL;
  right.data[1] = 0x6473865d77af47dUL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_sub(left, right);
  ASSERT(0x8f17c46404746406UL == result.data[0]);
  ASSERT(0x4b05b325b88987cUL == result.data[1]);
  ASSERT(0x0UL == result.data[2]);
  ASSERT(0x0UL == result.data[3]);
}

void test_sub_3(TestObjs *objs) {
  // "large" subtraction tests

  (void) objs;

  UInt256 left, right, result;

  // bc556287a225313cc07a1509f4ebb335034f5d413945ac7d0bdb42962a6ae8c - 7209ef9bebd10ecdc8fb3ccc6c9c69f41f2b217da808c18793c019c3cabaddc = 4a4b72ebb654226ef77ed83d884f4940e4243bc3913ceaf5781b28d25fb00b0
  left.data[0] = 0xd0bdb42962a6ae8cUL;
  left.data[1] = 0x5034f5d413945ac7UL;
  left.data[2] = 0xcc07a1509f4ebb33UL;
  left.data[3] = 0xbc556287a225313UL;
  right.data[0] = 0x793c019c3cabaddcUL;
  right.data[1] = 0x41f2b217da808c18UL;
  right.data[2] = 0xdc8fb3ccc6c9c69fUL;
  right.data[3] = 0x7209ef9bebd10ecUL;
  result = uint256_sub(left, right);
  ASSERT(0x5781b28d25fb00b0UL == result.data[0]);
  ASSERT(0x0e4243bc3913ceafUL == result.data[1]);
  ASSERT(0xef77ed83d884f494UL == result.data[2]);
  ASSERT(0x4a4b72ebb654226UL == result.data[3]);
}

void test_sub_4(TestObjs *objs) {
  // "another large" subtraction tests

  (void) objs;

  UInt256 left, right, result;

  //f1e613d9abcc050fd08d24f9d4f8f051112d9101643f7c6e151e4fa9f66d079 - 76486b4f5574ca28f82b5bd6183d7016a35aa3237c1c6e2cc25ad9b1b840baa = 7b9da88a56573ae6d861c923bcbb803a6dd2eddde8230e4152c375f83e2c4cf
  left.data[0] = 0xe151e4fa9f66d079UL;
  left.data[1] = 0x1112d9101643f7c6UL;
  left.data[2] = 0xfd08d24f9d4f8f05UL;
  left.data[3] = 0xf1e613d9abcc050UL;
  right.data[0] = 0xcc25ad9b1b840baaUL;
  right.data[1] = 0x6a35aa3237c1c6e2UL;
  right.data[2] = 0x8f82b5bd6183d701UL;
  right.data[3] = 0x76486b4f5574ca2UL;
  result = uint256_sub(left, right);
  ASSERT(0x152c375f83e2c4cfUL == result.data[0]);
  ASSERT(0xa6dd2eddde8230e4UL == result.data[1]);
  ASSERT(0x6d861c923bcbb803UL == result.data[2]);
  ASSERT(0x7b9da88a56573aeUL == result.data[3]);
}

//* test the edge case (subtracting 1 from 0 should yield a difference that is equal to the maximum value that can be represented)
void test_sub_5(TestObjs *objs) {
  // "large" subtraction tests

  (void) objs;

  UInt256 left, right, result;

  //edge case: substracting 1 from 0
  left.data[0] = 0x0UL;
  left.data[1] = 0x0UL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0x1UL;
  right.data[1] = 0x0UL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_sub(left, right);
  ASSERT(0xffffffffffffffffUL == result.data[0]);
  ASSERT(0xffffffffffffffffUL == result.data[1]);
  ASSERT(0xffffffffffffffffUL == result.data[2]);
  ASSERT(0xffffffffffffffffUL == result.data[3]);
}

void test_mul_1(TestObjs *objs) {
  // basic multiplication tests

  UInt256 result;

  result = uint256_mul(objs->one, objs->one);
  ASSERT(check(result, 0UL, 0UL, 0UL, 1UL));

  result = uint256_mul(objs->one, objs->zero);
  ASSERT(check(result, 0UL, 0UL, 0UL, 0UL));
}

void test_mul_2(TestObjs *objs) {
  (void) objs;

  UInt256 left, right, result;

  // 761544a98b82abc63f23766d1391782 * 14bf658bd8053a9484c32d955a47a2f = 991f2125eacd361abad710163aa9be6117fa57cddf52e73c97a28d7f744de
  left.data[0] = 0x63f23766d1391782UL;
  left.data[1] = 0x761544a98b82abcUL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0x484c32d955a47a2fUL;
  right.data[1] = 0x14bf658bd8053a9UL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_mul(left, right);
  ASSERT(0x73c97a28d7f744deUL == result.data[0]);
  ASSERT(0xe6117fa57cddf52eUL == result.data[1]);
  ASSERT(0x61abad710163aa9bUL == result.data[2]);
  ASSERT(0x991f2125eacd3UL == result.data[3]);
}


void test_mul_3(TestObjs *objs) {
  (void) objs;

  UInt256 left, right, result;

  //61c1fd64715fe7b923be73100dd0edb * 943e6eedd617c75f22d8a8c7860dccb = 389bfdd59d18e4472deccde9eaf1f96c5b9a07020bbebf605b9039e22efba9
  left.data[0] = 0x923be73100dd0edbUL;
  left.data[1] = 0x61c1fd64715fe7bUL;
  left.data[2] = 0x0UL;
  left.data[3] = 0x0UL;
  right.data[0] = 0xf22d8a8c7860dccbUL;
  right.data[1] = 0x943e6eedd617c75UL;
  right.data[2] = 0x0UL;
  right.data[3] = 0x0UL;
  result = uint256_mul(left, right);
  ASSERT(0x605b9039e22efba9UL == result.data[0]);
  ASSERT(0x6c5b9a07020bbebfUL == result.data[1]);
  ASSERT(0x472deccde9eaf1f9UL == result.data[2]);
  ASSERT(0x389bfdd59d18e4UL == result.data[3]);
}

void test_mul_4(TestObjs *objs) {
  // test 0 * 0

  UInt256 result;

  result = uint256_mul(objs->zero, objs->zero);
  ASSERT(check(result, 0UL, 0UL, 0UL, 0UL));

}


