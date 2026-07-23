#pragma once

// implemented these incase they cause funny memes with scheduling / regswaps

#define NN_DETAIL_SDK_ASSERT_IMPL(condition, conditionString, ...) static_cast<void>(0)

#define NN_SDK_ASSERT(condition, ...)                                                              \
    NN_DETAIL_SDK_ASSERT_IMPL((condition), NN_MACRO_STRINGIZE(condition), ##__VA_ARGS__)