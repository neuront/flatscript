#include <output/expr-nodes.h>
#include <output/function-impl.h>

#include "function.h"
#include "scope-impl.h"

using namespace semantic;

util::sptr<output::Function const> Function::compile(
        util::sref<SymbolTable> st, bool class_scope) const
{
    return util::mkptr(new output::RegularFunction(
            name, param_names, this->_compileBody(st, class_scope)));
}

util::sptr<output::Expression const> Function::compileToLambda(
                    util::sref<SymbolTable> st, bool class_scope) const
{
    return util::mkptr(new output::Lambda(
            this->pos, this->param_names, this->_compileBody(st, class_scope)
          , false));
}

util::sptr<output::Statement const> Function::_compileBody(
        util::sref<SymbolTable> st, bool class_scope) const
{
    SyncFunctionScope body_scope(pos, st, param_names, class_scope);
    this->body->compile(body_scope);
    return body_scope.deliver();
}

util::sptr<output::Function const> RegularAsyncFunction::compile(
        util::sref<SymbolTable> st, bool class_scope) const
{
    return util::mkptr(new output::RegularAsyncFunction(
            name, param_names, async_param_index, this->_compileBody(st, class_scope)));
}

util::sptr<output::Expression const> RegularAsyncFunction::compileToLambda(
                    util::sref<SymbolTable> st, bool class_scope) const
{
    return util::mkptr(new output::RegularAsyncLambda(
                this->pos, this->param_names, this->async_param_index
              , this->_compileBody(st, class_scope)));
}

util::sptr<output::Statement const> RegularAsyncFunction::_compileBody(
        util::sref<SymbolTable> st, bool class_scope) const
{
    RegularAsyncFuncScope body_scope(pos, st, param_names, class_scope);
    this->body->compile(body_scope);
    return body_scope.deliver();
}

util::sptr<output::Expression const> Lambda::compile(util::sref<Scope> scope) const
{
    SyncFunctionScope body_scope(pos, scope->sym(), param_names, false);
    this->body->compile(body_scope);
    return util::mkptr(new output::Lambda(pos, param_names, body_scope.deliver(), false));
}

util::sptr<output::Expression const> RegularAsyncLambda::compile(util::sref<Scope> scope) const
{
    RegularAsyncFuncScope body_scope(pos, scope->sym(), param_names, false);
    this->body->compile(body_scope);
    return util::mkptr(new output::RegularAsyncLambda(
                    pos, param_names, async_param_index, body_scope.deliver()));
}
