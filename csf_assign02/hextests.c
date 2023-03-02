// Unit tests for hex functions
// These tests should work for both your C implementations and your
// assembly language implementations

#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"
#include "hexfuncs.h"

// test fixture object
typedef struct {
  char test_data_1[16];
  char test_data_2[15];
} TestObjs;

// setup function (to create the test fixture)
TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));
  strcpy(objs->test_data_1, "Hello, world!\0");
  strcpy(objs->test_data_2, "\x24We_eat pizza\xEB\0");
  return objs;
}

// cleanup function (to destroy the test fixture)
void cleanup(TestObjs *objs) {
  free(objs);
}

// Prototypes for test functions
void testFormatOffset(TestObjs *objs);
void testFormatByteAsHex(TestObjs *objs);
void testHexToPrintable(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(testFormatOffset);
  TEST(testFormatByteAsHex);
  TEST(testHexToPrintable);

  TEST_FINI();

  return 0;
}

void testFormatOffset(TestObjs *objs) {
  (void) objs; // suppress warning about unused parameter
  char buf[16];
  hex_format_offset(0x00000001u, buf);
  ASSERT(0 == strcmp(buf, "00000001"));

  hex_format_offset(0xabcd1234u, buf);
  ASSERT(0 == strcmp(buf, "abcd1234"));

  hex_format_offset(0L, buf);
  ASSERT(0 == strcmp(buf, "00000000"));

  /* Test 16's multiples */
  hex_format_offset(16L, buf);
  ASSERT(0 == strcmp(buf, "00000010"));

  /* Test 16's multiples */
  hex_format_offset(256L, buf);
  ASSERT(0 == strcmp(buf, "00000100"));

  hex_format_offset(367L, buf);
  ASSERT(0 == strcmp(buf, "0000016f"));

  hex_format_offset(68798989L, buf);
  ASSERT(0 == strcmp(buf, "0419ca0d"));

  hex_format_offset(4294967295, buf);
  ASSERT(0 == strcmp(buf, "ffffffff"));


}

void testFormatByteAsHex(TestObjs *objs) {
  char buf[16];
  hex_format_byte_as_hex(objs->test_data_1[0], buf);
  ASSERT(0 == strcmp(buf, "48"));

  hex_format_byte_as_hex(objs->test_data_1[1], buf);
  ASSERT(0 == strcmp(buf, "65"));

  hex_format_byte_as_hex(objs->test_data_1[6], buf);
  ASSERT(0 == strcmp(buf, "20"));

    /* Test the nul terminator */
  hex_format_byte_as_hex(objs->test_data_1[14], buf);
  ASSERT(0 == strcmp(buf, "00"));

  hex_format_byte_as_hex(objs->test_data_2[1], buf);
  ASSERT(0 == strcmp(buf, "57"));

  hex_format_byte_as_hex(objs->test_data_2[5], buf);
  ASSERT(0 == strcmp(buf, "61"));

  hex_format_byte_as_hex(objs->test_data_2[6], buf);
  ASSERT(0 == strcmp(buf, "74"));

  hex_format_byte_as_hex(objs->test_data_2[8], buf);
  ASSERT(0 == strcmp(buf, "70"));

  hex_format_byte_as_hex(objs->test_data_2[9], buf);
  ASSERT(0 == strcmp(buf, "69"));

}

void testHexToPrintable(TestObjs *objs) {
  ASSERT('H' == hex_to_printable(objs->test_data_1[0]));
  ASSERT('l' == hex_to_printable(objs->test_data_1[2]));
  /* Test the unprintable character*/
  ASSERT('.' == hex_to_printable(objs->test_data_1[13]));
  ASSERT('$' == hex_to_printable(objs->test_data_2[0]));
  ASSERT('t' == hex_to_printable(objs->test_data_2[6]));
    /* Test the normal character*/
  ASSERT('p' == hex_to_printable(objs->test_data_2[8]));
  /* Test the unprintable character*/
  ASSERT('.' == hex_to_printable(objs->test_data_2[13]));
}
