#include <gtest/gtest.h>
#include "pasific.h"

namespace pd = pasific::detail;

template<typename T>
struct pow2ceil_test : public testing::Test {};
TYPED_TEST_CASE_P(pow2ceil_test);

TYPED_TEST_P(pow2ceil_test, exact)
{
    constexpr TypeParam n[] = { 1, 2, 8, 0x80, 0x8000, 0x800000, 0x40000000 };
    for(auto i = 0; i < sizeof(n) / sizeof(TypeParam); ++i) {
        SCOPED_TRACE(n[i]);
        ASSERT_EQ(TypeParam(n[i]), pd::pow2ceil(n[i]));
    }
}

TYPED_TEST_P(pow2ceil_test, round)
{
    constexpr TypeParam n[] = {        3,
                                       5,      6,      7, 
                                    0x41,   0x60,   0x7f,
                                  0x4001, 0x5555, 0x7fff,
                                0x408080 },
                        m[] = {        4,
                                       8,      8,      8,
                                    0x80,   0x80,   0x80,
                                  0x8000, 0x8000, 0x8000,
                                0x800000 };
    static_assert(sizeof(n) == sizeof(m), "invalid length");
    for(auto i = 0; i < sizeof(n) / sizeof(TypeParam); ++i) {
        SCOPED_TRACE(n[i]);
        ASSERT_EQ(TypeParam(m[i]), pd::pow2ceil(n[i]));
    }
}

REGISTER_TYPED_TEST_CASE_P(pow2ceil_test, exact, round);
typedef testing::Types<int, long> pow2ceil_types;
INSTANTIATE_TYPED_TEST_CASE_P(all, pow2ceil_test, pow2ceil_types);

TEST(ring, foo)
{
    auto p = pasific::ring<int, 16>::create();
    ASSERT_EQ(16, p->capacity());
}
