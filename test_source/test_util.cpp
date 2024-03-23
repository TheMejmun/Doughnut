//
// Created by Sam on 2024-03-23.
//

#include "test_util.h"


REGISTER(hi, {
    TEST(hiya, {
            expect(true, "This should not fail");
            expect(false, "This should fail");
    })
})