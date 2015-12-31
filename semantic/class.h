#ifndef __STEKIN_SEMANTIC_CLASS_H__
#define __STEKIN_SEMANTIC_CLASS_H__

#include <util/arrays.h>
#include <output/class.h>

#include "block.h"

namespace semantic {

    struct Constructor {
        Constructor(misc::position const& ps
                  , std::vector<std::string> params
                  , util::sptr<Statement const> b
                  , std::string const& class_name, bool si
                  , util::ptrarr<Expression const> super_ctor_args);

        util::sptr<output::Constructor const> compile(
                util::sref<SymbolTable> st, bool has_base_class) const;

        bool superInit() const
        {
            return this->super_init.not_nul();
        }

        misc::position const pos;
        std::vector<std::string> const param_names;
        util::sptr<Statement const> const super_init;
        util::sptr<Statement const> const body;
    };

    struct Class
        : Statement
    {
        Class(misc::position const& ps, std::string n, util::sptr<Expression const> base
            , util::sptr<Block const> b, util::sptr<Constructor const> ct)
                : Statement(ps)
                , name(std::move(n))
                , base_class(std::move(base))
                , body(std::move(b))
                , ctor_or_nul(std::move(ct))
        {}

        void compile(util::sref<Scope> scope) const;

        bool isAsync() const { return false; }

        bool hasBaseClass() const
        {
            return this->base_class.not_nul();
        }

        std::string const name;
        util::sptr<Expression const> const base_class;
        util::sptr<Block const> const body;
        util::sptr<Constructor const> const ctor_or_nul;
    };

}

#endif /* __STEKIN_SEMANTIC_CLASS_H__ */
