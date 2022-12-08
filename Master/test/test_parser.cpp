#include <unity.h>
#include <Arduino.h>
#include "parser.h"
void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_no_data_to_parse() {
    String* toParse;
    toParse = "";
    TEST_ASSERT_EQUAL(parseString(toParse),-1);
}


int main( int argc, char **argv) {
    UNITY_BEGIN();

    RUN_TEST(test_no_data_to_parse);


    UNITY_END();
}