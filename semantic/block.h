#ifndef __STEKIN_SEMANTIC_BLOCK_H__
#define __STEKIN_SEMANTIC_BLOCK_H__

#include <string>

#include <output/fwd-decl.h>
#include <util/arrays.h>
#include <misc/pos-type.h>

#include "fwd-decl.h"
#include "node-base.h"

namespace semantic {

    struct Block {
        Block() = default;

        Block(Block const&) = delete;

        Block(Block&& rhs)
            : _stmts(std::move(rhs._stmts))
            , _funcs(std::move(rhs._funcs))
        {}

        bool isAsync() const;
        void checkNoAsync(misc::position const& check_pos) const;
        void append(Block following);
        void compile(util::sref<Scope> scope) const;

        void compile(Scope& scope) const
        {
            this->compile(util::mkref(scope));
        }

        void addStmt(util::sptr<Statement const> stmt)
        {
            this->_stmts.append(std::move(stmt));
        }

        void addFunc(util::sptr<Function const> func)
        {
            this->_funcs.append(std::move(func));
        }

        util::ptrarr<Function const> const& getFuncs() const
        {
            return this->_funcs;
        }
    private:
        util::ptrarr<Statement const> _stmts;
        util::ptrarr<Function const> _funcs;
    };

}

#endif /* __STEKIN_SEMANTIC_BLOCK_H__ */
