//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_MACRO_H
#define INC_233_LANGUAGE_233_MACRO_H

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define ENUM_TO_STRING(x) switch (x)
#define ENUM_TO_STRING_CASE(x) case x: return #x

// #define lang233_inline inline __attribute__((always_inline))
#define lang233_inline inline

#endif //INC_233_LANGUAGE_233_MACRO_H
