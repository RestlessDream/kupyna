#include "unity.h"
#include "../src/kupyna.h"
#include "../src/util.h"
#include "stdlib.h"
#include "stdio.h"

extern kupyna_config kupyna_256;
extern kupyna_config kupyna_384;
extern kupyna_config kupyna_512;
extern kupyna_config * kupyna;
extern size_t rows_count;

void setUp(void) {
    setup(&kupyna_256);
}

void tearDown(void) {
}

void test_add_round_key() {
    dw state[8] = { 0x0001020304050607, 0x08090A0B0C0D0E0F, 0x1011121314151617,
                    0x18191A1B1C1D1E1F, 0x2021222324252627, 0x28292A2B2C2D2E2F,
                    0x3031323334353637, 0x38393A3B3C3D3E3F };
    to_endian(state, 8);

    dw expected[8] = { 0xF3F1F2F3F4F5F677, 0xFBF9FAFBFCFDFE6F, 0x0302030405060768,
                       0x0B0A0B0C0D0E0F60, 0x1312131415161758, 0x1B1A1B1C1D1E1F50,
                       0x2322232425262748, 0x2B2A2B2C2D2E2F40 };
    to_endian(expected, 8);

    add_round_key(state, 0);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kupyna->state_rows);
}

void test_sub_bytes() {
    dw state[8] = { 0x1101020304050607, 0x08090A0B0C0D0E0F, 0x1011121314151617,
                    0x18191A1B1C1D1E1F, 0x2021222324252627, 0x28292A2B2C2D2E2F,
                    0x3031323334353637, 0x38393A3B3C3D3E3F };
    dw expected[8] = { 0xF3BB9A4D6BCB452A, 0x713ADFB31790511F, 0x6D152B3DC91CBB83,
                       0x795C71D56F5716BD, 0x3EF6C002B4F4AD11, 0x1F0F7A5E496DD166,
                       0x9226C445D15DB794, 0xF4140E1A5810B2DF };
    to_endian(state, 8);
    to_endian(expected, 8);

    sub_bytes(state);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kupyna->state_rows);
}

void test_shift_rows() {
    dw state[8] = { 0xF3BB9A4D6BCB452A, 0x713ADFB31790511F, 0x6D152B3DC91CBB83,
                    0x795C71D56F5716BD, 0x3EF6C002B4F4AD11, 0x1F0F7A5E496DD166,
                    0x9226C445D15DB794, 0xF4140E1A5810B2DF };
    to_endian(state, 8);

    shift_rows(state);


    dw expected[8] = { 0xF314C45EB457BB1F, 0x71BB0E4549F41683, 0x6D3A9A1AD16DADBD,
                       0x7915DF4D585DD111, 0x3E5C2BB36B10B766, 0x1FF6713D17CBB294,
                       0x920FC0D5C99045DF, 0xF4267A026F1C512A };
    to_endian(expected, 8);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kupyna->state_rows);
}

void test_mix_columns() {
    dw state[8] = { 0xF314C45EB457BB1F, 0x71BB0E4549F41683, 0x6D3A9A1AD16DADBD,
                    0x7915DF4D585DD111, 0x3E5C2BB36B10B766, 0x1FF6713D17CBB294,
                    0x920FC0D5C99045DF, 0xF4267A026F1C512A };
    to_endian(state, 8);

    mix_columns(state);

    dw expected[8] = { 0x16D6D3F421137557, 0x9D40CE70414C6086, 0x9A3801A4784DB204,
                       0xC5766C433973AC84, 0xEE9B1D8953D5CF42, 0x1773C8EF40AFFCCD,
                       0x738240440513C00A, 0x4A038A1805B8C578 };
    to_endian(state, 8);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kupyna->state_rows);
}

void test_xor_round_key() {
    dw state[8] = {0x0001020304050607, 0x08090A0B0C0D0E0F, 0x1011121314151617,
                   0x18191A1B1C1D1E1F, 0x2021222324252627, 0x28292A2B2C2D2E2F,
                   0x3031323334353637, 0x38393A3B3C3D3E3F};
    to_endian(state, 8);

    xor_round_key(state, 0);

    dw expected[8] = {0x0001020304050607, 0x08090A0B0C0D0E0F, 0x1011121314151617,
                   0x18191A1B1C1D1E1F, 0x2021222324252627, 0x28292A2B2C2D2E2F,
                   0x3031323334353637, 0x38393A3B3C3D3E3F};
    to_endian(expected, 8);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected, state, kupyna->state_rows);

    dw state2[8] = { 0x544AB381EA8ACA34, 0x49A9DD1F7D9FB448, 0x4E6AB7B5F93A61B1,
                     0x8D05B4760C023FB9, 0xFA5FA01A21EEBF29, 0xE1662942BAB4A85A,
                     0x779BEF260345DD88, 0x73D6AE28FF1D16FC };
    to_endian(state2, 8);
    xor_round_key(state2, 1);
    dw expected2[8] = { 0x564AB381EA8ACA34, 0x5BA9DD1F7D9FB448, 0x6C6AB7B5F93A61B1,
                        0xBF05B4760C023FB9, 0xB85FA01A21EEBF29, 0xB3662942BAB4A85A,
                        0x159BEF260345DD88, 0x01D6AE28FF1D16FC };

    to_endian(expected2, 8);

    TEST_ASSERT_EQUAL_UINT64_ARRAY(expected2, state2, kupyna->state_rows);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_add_round_key);
    RUN_TEST(test_sub_bytes);
    RUN_TEST(test_shift_rows);
    RUN_TEST(test_mix_columns);
    RUN_TEST(test_xor_round_key);

    return UNITY_END();
}

