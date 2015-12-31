#ifndef __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__
#define __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__

#include <semantic/scope.h>
#include <semantic/node-base.h>
#include <semantic/block.h>
#include <semantic/function.h>
#include <semantic/class.h>
#include <output/node-base.h>
#include <output/block.h>
#include <output/function.h>
#include <misc/pos-type.h>
#include <test/data-node.h>
#include <test/data-trees.h>

namespace test {

    std::ostream& dummyos();
    util::sptr<output::Statement const> compile(
                                    semantic::Block& b, util::sref<semantic::SymbolTable> sym);

    struct SemanticData {
        misc::position const pos;
        int const int_val;

        SemanticData(misc::position const& ps, int iv)
            : pos(ps)
            , int_val(iv)
        {}

        explicit SemanticData(misc::position const ps)
            : pos(ps)
            , int_val(-1)
        {}

        explicit SemanticData(int iv)
            : int_val(iv)
        {}

        SemanticData()
            : int_val(-1)
        {}

        bool operator==(SemanticData const& rhs) const
        {
            return pos == rhs.pos && int_val == rhs.int_val;
        }

        std::string str() const;
    };

    struct DataTree
        : DataTreeTempl<SemanticData, DataTree>
    {
        typedef DataTreeTempl<SemanticData, DataTree> BaseType;

        DataTree& operator()(misc::position const& pos
                           , NodeType const& type
                           , std::string const& str);
        DataTree& operator()(NodeType const& type
                           , std::string const& str
                           , int size);
        DataTree& operator()(NodeType const& type);
        DataTree& operator()(NodeType const& type, std::string const& str);
        DataTree& operator()(NodeType const& type, int value);
        DataTree& operator()(misc::position const& pos, NodeType const& type);
        DataTree& operator()(misc::position const& pos, NodeType const& type, int size);
    };

    extern NodeType const UNDEFINED;
    extern NodeType const BOOLEAN;
    extern NodeType const INTEGER;
    extern NodeType const FLOATING;
    extern NodeType const STRING;
    extern NodeType const REGEXP;
    extern NodeType const LIST;
    extern NodeType const THIS;
    extern NodeType const SUPER_FUNC;
    extern NodeType const BINARY_OP;
    extern NodeType const PRE_UNARY_OP;
    extern NodeType const CONDITIONAL;
    extern NodeType const EXCEPTION_OBJ;
    extern NodeType const COND_CALLBACK_PARAM;
    extern NodeType const REFERENCE;
    extern NodeType const SUB_REFERENCE;
    extern NodeType const TRANSIENT_PARAMETER;
    extern NodeType const IMPORTED_NAME;
    extern NodeType const PIPE_ELEMENT;
    extern NodeType const PIPE_INDEX;
    extern NodeType const PIPE_KEY;
    extern NodeType const PIPE_RESULT;

    extern NodeType const SYNC_FOR_RANGE;
    extern NodeType const ASYNC_FOR_RANGE;
    extern NodeType const BREAK;
    extern NodeType const CONTINUE;
    extern NodeType const SYNC_BREAK;
    extern NodeType const CALL_NEXT;

    extern NodeType const CALL;
    extern NodeType const SUPER_CONSTRUCTOR_CALL;
    extern NodeType const ASYNC_REFERENCE;

    extern NodeType const ASYNC_PIPELINE;
    extern NodeType const SYNC_PIPELINE;
    extern NodeType const ROOT_SYNC_PIPELINE;
    extern NodeType const SYNC_PIPELINE_RETURN;

    extern NodeType const LIST_SLICE;

    extern NodeType const DICT_BEGIN;
    extern NodeType const DICT_END;
    extern NodeType const DICT_ITEM;

    extern NodeType const ASYNC_RESULT_DEF;

    extern NodeType const STATEMENT;
    extern NodeType const ARITHMETICS;
    extern NodeType const RETURN;
    extern NodeType const EXPORT;
    extern NodeType const EXPORT_VALUE;
    extern NodeType const DEC_THIS;
    extern NodeType const BRANCH;
    extern NodeType const TRY;
    extern NodeType const CATCH;

    extern NodeType const CLASS_INIT_FN;
    extern NodeType const CLASS_INIT_ST;
    extern NodeType const MEMBER_FUNC;

    extern NodeType const FUNCTION;
    extern NodeType const PARAMETER;
    extern NodeType const FWD_DECL;
    extern NodeType const MANGLE_AS_PARAM;
    extern NodeType const REGULAR_ASYNC_RETURN;

    extern NodeType const EXC_THROW;
    extern NodeType const EXC_CALLBACK;
    extern NodeType const ASYNC_CATCH_FUNC;

    extern NodeType const SCOPE_BEGIN;
    extern NodeType const SCOPE_END;

    struct SemanticTest
        : testing::Test
    {
        void SetUp();
        void TearDown();
    };

}

#endif /* __STEKIN_SEMANTIC_TEST_TEST_COMMON_H__ */