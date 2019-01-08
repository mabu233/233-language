//
// Created by lyx on 19-1-6.
//

#ifndef INC_233_LANGUAGE_CORE_H
#define INC_233_LANGUAGE_CORE_H

#include "233_ext.h"

void lang233_core_init();

namespace lang233_core
{
    /**
     * output
     */
    void output_init();
    LANG233_FUNC(var_dump);
    LANG233_FUNC(print);
}

#endif //INC_233_LANGUAGE_CORE_H
