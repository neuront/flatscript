#ifndef __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__
#define __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__

#include "automation-base.h"

namespace grammar {

    struct ExprStmtAutomation
        : AutomationBase
    {
        explicit ExprStmtAutomation(util::sref<ClauseBase> clause);

        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    private:
        void _pushColon(AutomationStack& stack, misc::position const& pos);
        util::sptr<Statement> _reduceAsStmt();

        std::vector<util::sptr<Expression const>> _exprs;
        util::sref<ClauseBase> const _clause;
        bool _before_colon;
    };

    struct StandaloneKeyWordAutomation
        : AutomationBase
    {
        explicit StandaloneKeyWordAutomation(misc::position const& ps)
            : pos(ps)
        {}

        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool) const { return true; }
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);

        virtual util::sptr<ClauseBase> createClause(ClauseStackWrapper& wrapper) = 0;

        misc::position const pos;
    };

    struct IfAutomation
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const { return !sub_empty; }
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);

        IfAutomation()
            : _pred_cache(nullptr)
        {}
    protected:
        util::sptr<Expression const> _pred_cache;
    };

    struct ElseAutomation
        : StandaloneKeyWordAutomation
    {
        explicit ElseAutomation(misc::position const& pos)
            : StandaloneKeyWordAutomation(pos)
        {}

        util::sptr<ClauseBase> createClause(ClauseStackWrapper& wrapper);
    };

    struct IfnotAutomation
        : IfAutomation
    {
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);
    };

    struct ForAutomation
        : AutomationBase
    {
        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool sub_empty) const { return this->_finished || !sub_empty; }
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);

        explicit ForAutomation(misc::position const& pos)
            : _pos(pos)
            , _before_ref(true)
            , _finished(false)
        {}
    protected:
        misc::position _pos;
        std::string _ref;
        std::vector<util::sptr<Expression const>> _range_args;
        bool _before_ref;
        bool _finished;
    };

    struct FunctionAutomation
        : AutomationBase
    {
        explicit FunctionAutomation(bool exprt)
            : _before_open_paren(true)
            , _finished(false)
            , _export(exprt)
            , _async_param_index(-1)
        {}

        FunctionAutomation()
            : FunctionAutomation(false)
        {}

        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void accepted(AutomationStack&, util::sptr<Expression const>) {};
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        bool finishOnBreak(bool) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);
    private:
        bool _before_open_paren;
        bool _finished;
        bool const _export;
        misc::position _pos;
        std::string _func_name;
        std::vector<std::string> _params;
        int _async_param_index;
    };

    struct ExprReceiver
        : AutomationBase
    {
        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const { return true; }

        explicit ExprReceiver(util::sref<ClauseBase> clause)
            : _clause(clause)
            , _expr(nullptr)
        {}
    protected:
        util::sref<ClauseBase> const _clause;
        util::sptr<Expression const> _expr;
    };

    struct ReturnAutomation
        : ExprReceiver
    {
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ReturnAutomation(misc::position const& p, util::sref<ClauseBase> clause)
            : ExprReceiver(clause)
            , pos(p)
        {}

        misc::position const pos;
    };

    struct ThrowAutomation
        : ExprReceiver
    {
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);

        ThrowAutomation(misc::position const& p, util::sref<ClauseBase> clause)
            : ExprReceiver(clause)
            , pos(p)
        {}

        misc::position const pos;
    };

    struct TryAutomation
        : StandaloneKeyWordAutomation
    {
        explicit TryAutomation(misc::position const& pos)
            : StandaloneKeyWordAutomation(pos)
        {}

        util::sptr<ClauseBase> createClause(ClauseStackWrapper& wrapper);
    };

    struct CatchAutomation
        : StandaloneKeyWordAutomation
    {
        void pushFactor(AutomationStack& stack, FactorToken& factor);

        explicit CatchAutomation(misc::position const& pos)
            : StandaloneKeyWordAutomation(pos)
        {}

        util::sptr<ClauseBase> createClause(ClauseStackWrapper& wrapper);
    private:
        std::string _except_name;
    };

    struct ClassAutomation
        : AutomationBase
    {
        explicit ClassAutomation(misc::position const& pos)
            : ClassAutomation(pos, false)
        {}

        ClassAutomation(misc::position const& pos, bool exprt)
            : _pos(pos)
            , _base_class(nullptr)
            , _export(exprt)
        {}

        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void accepted(AutomationStack&, util::sptr<Expression const> expr);
        bool finishOnBreak(bool) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);
    private:
        misc::position const _pos;
        std::string _class_name;
        util::sptr<Expression const> _base_class;
        bool const _export;
    };

    struct CtorAutomation
        : AutomationBase
    {
        explicit CtorAutomation(misc::position const& pos);

        void accepted(AutomationStack&, util::sptr<Expression const>) {};
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        bool finishOnBreak(bool sub_empty) const;
        void finish(ClauseStackWrapper& wrapper, AutomationStack& stack, misc::position const&);
    private:
        misc::position const _pos;
        std::function<void(CtorAutomation*, std::vector<util::sptr<Expression const>>)>
                _list_accepted;
        std::vector<std::string> _params;
        std::vector<util::sptr<Expression const>> _super_ctor_args;
        bool _finished;
        bool _super_init;

        static void _acceptParams(CtorAutomation* self
                                , std::vector<util::sptr<Expression const>> list);
        static void _acceptSuperArgs(CtorAutomation* self
                                   , std::vector<util::sptr<Expression const>> list);
    };

    struct ExternAutomation
        : AutomationBase
    {
        ExternAutomation(misc::position const& pos, util::sref<ClauseBase> clause)
            : _pos(pos)
            , _clause(clause)
        {}

        void activated(AutomationStack& stack);
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool) const { return true; }
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    private:
        misc::position const _pos;
        util::sref<ClauseBase> const _clause;
        std::vector<std::string> _externs;
    };

    struct ExportAutomation
        : AutomationBase
    {
        ExportAutomation(misc::position const& pos, util::sref<ClauseBase> clause);
        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void accepted(AutomationStack&, std::vector<util::sptr<Expression const>> list);
        void accepted(AutomationStack&, util::sptr<Expression const> e);
        bool finishOnBreak(bool sub_empty) const { return !sub_empty; }
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    private:
        misc::position const _pos;
        util::sref<ClauseBase> const _clause;
        std::vector<std::string> _export_point;
        util::sptr<Expression const> _value;
    };

    struct EnumAutomation
        : AutomationBase
    {
        explicit EnumAutomation(util::sref<ClauseBase> clause);
        void pushFactor(AutomationStack& stack, FactorToken& factor);
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool sub_empty) const {return this->_after_name || !sub_empty;}
    private:
        int _current_value;
        std::vector<util::sptr<Statement>> _defs;
        util::sref<ClauseBase> const _clause;
        bool _after_name;
    };

    struct IncludeAutomation
        : AutomationBase
    {
        IncludeAutomation(misc::position const& pos, util::sref<ClauseBase> clause);
        void pushFactor(AutomationStack&, FactorToken& factor);
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const& pos);
        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        bool finishOnBreak(bool) const {return !bool(this->_next_factor);}
    private:
        misc::position _pos;
        std::function<void(FactorToken& factor)> _next_factor;
        util::sref<ClauseBase> const _clause;
        std::string _file_path;
        std::string _alias;

        void _fill_path(FactorToken& factor);
        void _as(FactorToken& factor);
        void _fill_alias(FactorToken& factor);
    };

    struct IgnoreAny
        : AutomationBase
    {
        explicit IgnoreAny(bool reported);
        void pushFactor(AutomationStack& stack, FactorToken& factor);
        bool finishOnBreak(bool) const { return true; }
        void accepted(AutomationStack&, util::sptr<Expression const>) {}
        void finish(ClauseStackWrapper&, AutomationStack& stack, misc::position const&);
    private:
        void _report(Token const& t);
        bool _reported;
    };

}

#endif /* __STEKIN_GRAMMAR_STATEMENT_AUTOMATIONS_H__ */
