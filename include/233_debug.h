//
// Created by lyx on 19-1-2.
//

#ifndef INC_233_LANGUAGE_233_DEBUG_H
#define INC_233_LANGUAGE_233_DEBUG_H

#ifdef LANG233_DEBUG
#define DEBUG_OUTPUT(...) \
printf("[DEBUG] File %s, line %d: ", __FILE__, __LINE__); \
printf(__VA_ARGS__); \
printf("\n")
#else
#define DEBUG_OUTPUT(...)
#endif

#endif //INC_233_LANGUAGE_233_DEBUG_H
