#ifndef __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__

#include "automation-base.h"
#include "expr-nodes.h"

namespace grammar {

    struct ArithAutomation
        : AutomationBase
    {
        void pushOp(AutomationStack&, Token const& token);
        void pushFactor(AutomationStack& stack
                      , util::sptr<Expression const> factor
                      , std::string const& image);
        void pushOpenParen(AutomationStack& stack, misc::position const& pos);
        void pushOpenBracket(AutomationStack& stack, misc::position const& pos);
        void pushOpenBrace(AutomationStack& stack, misc::position const& pos);
        void matchClosing(AutomationStack& stack, Token const& closer);
        void pushColon(AutomationStack& stack, misc::position const& pos);
        void pushComma(AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        bool eolAsBreak(bool) const;
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ArithAutomation();

        struct Operator;
        typedef std::vector<util::sptr<Operator const>> OpStack;
        typedef std::vector<util::sptr<Expression const>> FactorStack;

        struct Operator {
            virtual ~Operator() {}

            Operator(int p)
                : pri(p)
            {}

            virtual void operate(FactorStack& factors) const = 0;

            int const pri;
        };
    private:
        bool _empty() const;
        void _pushFactor(util::sptr<Expression const> factor, Token const& token);
        void _reduceBinaryOrPostfix(int pri);
        bool _reduceIfPossible(
                AutomationStack& stack, misc::position const& token, std::string const& image);

        bool _need_factor;
        bool _accept_list_for_args;
        std::string _reduce_close_paren;
        FactorStack _factor_stack;
        OpStack _op_stack;
    };

    struct ExprStmtAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void pushColon(AutomationStack& stack, misc::position const&);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool eolAsBreak(bool) const;
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ExprStmtAutomation(util::sref<ClauseBase> acc)
            : _clause(acc)
        {}
    private:
        util::sptr<Statement> _reduceAsStmt();

        std::vector<util::sptr<Expression const>> _exprs;
        util::sref<ClauseBase> const _clause;
    };

    struct ExprListAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void matchClosing(AutomationStack& stack, Token const& closer);
        void pushComma(AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool eolAsBreak(bool) const;
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    protected:
        std::vector<util::sptr<Expression const>> _list;
    };

    struct ListLiteralAutomation
        : ExprListAutomation
    {
        void matchClosing(AutomationStack& stack, Token const& closer);
    };

    struct NestedOrParamsAutomation
        : ExprListAutomation
    {
        void pushOp(AutomationStack& stack, Token const& token);
        void pushOpenParen(AutomationStack& stack, misc::position const& pos);
        void pushOpenBracket(AutomationStack& stack, misc::position const& pos);
        void matchClosing(AutomationStack&, Token const& closer);
        void pushColon(AutomationStack& stack, misc::position const& pos);
        void pushComma(AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack& stack, util::sptr<Expression const> expr);
        void accepted(AutomationStack& stack, misc::position const& pos, Block&& block);
        bool eolAsBreak(bool sub_empty) const;
        void eol(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const& pos);

        NestedOrParamsAutomation()
            : _wait_for_closing(true)
            , _wait_for_colon(false)
            , _lambda_ret_val(nullptr)
        {}
    private:
        void _reduceAsNested(AutomationStack& stack, misc::position const& rp);
        void _reduceAsLambda(AutomationStack& stack);
        void _reduceAsLambda(AutomationStack& stack, misc::position const& pos, Block body);
        bool _afterColon() const;

        bool _wait_for_closing;
        bool _wait_for_colon;
        util::sptr<Expression const> _lambda_ret_val;
    };

    struct BracketedExprAutomation
        : ExprListAutomation
    {
        void matchClosing(AutomationStack& stack, Token const& closer);
    };

    struct DictAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void matchClosing(AutomationStack& stack, Token const& closer);
        void pushComma(AutomationStack& stack, misc::position const& pos);
        void pushColon(AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool eolAsBreak(bool) const;
        void eol(ClauseStackWrapper&, AutomationStack&, misc::position const&);

        DictAutomation()
            : _wait_for_key(true)
            , _wait_for_colon(false)
            , _wait_for_comma(false)
            , _key_cache(nullptr)
        {}
    private:
        bool _wait_for_key;
        bool _wait_for_colon;
        bool _wait_for_comma;
        util::sptr<Expression const> _key_cache;
        std::vector<Dictionary::ItemType> _items;
    };

    struct ExprReceiver
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool eolAsBreak(bool) const;
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ExprReceiver(util::sref<ClauseBase> acc)
            : _clause(acc)
            , _expr(nullptr)
        {}
    protected:
        util::sref<ClauseBase> const _clause;
        util::sptr<Expression const> _expr;
    };

    struct ReturnAutomation
        : ExprReceiver
    {
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        explicit ReturnAutomation(util::sref<ClauseBase> acc)
            : ExprReceiver(acc)
        {}
    };

    struct ExportStmtAutomation
        : ExprReceiver
    {
        void eol(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ExportStmtAutomation(util::sref<ClauseBase> acc, std::vector<std::string> const& ep)
            : ExprReceiver(acc)
            , export_point(ep)
        {}

        std::vector<std::string> const export_point;
    };

}

#endif /* __STEKIN_GRAMMAR_EXPRESSION_AUTOMATIONS_H__ */
