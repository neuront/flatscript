#ifndef __STEKIN_SEMANTIC_FUNCTION_H__
#define __STEKIN_SEMANTIC_FUNCTION_H__

#include "block.h"

namespace semantic {

    struct Function {
        Function(misc::position const& ps
               , std::string const& func_name
               , std::vector<std::string> const& params
               , util::sptr<Filter const> func_body)
            : pos(ps)
            , name(func_name)
            , param_names(params)
            , body(std::move(func_body))
        {}

        util::sptr<output::Function const> compile(util::sref<SymbolTable> st) const;

        misc::position const pos;
        std::string const name;
        std::vector<std::string> const param_names;
        util::sptr<Filter const> const body;
    };

}

#endif /* __STEKIN_SEMANTIC_FUNCTION_H__ */
