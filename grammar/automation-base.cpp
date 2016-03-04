#include <algorithm>

#include "tokens.h"
#include "automation-base.h"
#include "clauses.h"

using namespace grammar;

void AutomationBase::setPrevious(util::sref<AutomationBase const> previous)
{
    _previous = previous;
}

void AutomationStack::push(util::sptr<AutomationBase> automation)
{
    if (!empty()) {
        automation->setPrevious(top());
    }
    _stack.push_back(std::move(automation));
    top()->activated(*this);
}

void AutomationStack::replace(util::sptr<AutomationBase> automation)
{
    _stack.pop_back();
    push(std::move(automation));
}

util::sref<AutomationBase> AutomationStack::top() const
{
    return *_stack.back();
}

void AutomationStack::reduced(util::sptr<Expression const> expr)
{
    _stack.pop_back();
    top()->accepted(*this, std::move(expr));
}

void AutomationStack::reduced(std::vector<util::sptr<Expression const>> list)
{
    _stack.pop_back();
    top()->accepted(*this, std::move(list));
}

void AutomationStack::pop()
{
    _stack.pop_back();
}

bool AutomationStack::empty() const
{
    return _stack.empty();
}

AutomationBase::AutomationBase()
    : _previous(nullptr)
{
    std::fill(_actions, _actions + TOKEN_TYPE_COUNT, AutomationBase::discardToken);
}

void AutomationBase::nextToken(AutomationStack& stack, TypedToken const& token)
{
    _actions[token.type](stack, token);
}

void AutomationBase::discardToken(AutomationStack&, TypedToken const& token)
{
    token.unexpected();
}

void AutomationBase::pushFactor(AutomationStack&, FactorToken& factor)
{
    factor.unexpected();
}

void AutomationBase::_setFollowings(std::set<TokenType> types)
{
    std::for_each(types.begin()
                , types.end()
                , [&](TokenType tp)
                  {
                      _actions[tp] = [=](AutomationStack& stack, TypedToken const& follower)
                                     {
                                         if (_reduce(stack, follower)) {
                                             stack.top()->nextToken(stack, follower);
                                         }
                                     };
                  });
}

void AutomationBase::_setShifts(std::map<TokenType, std::pair<AutomationCreator, bool>> types)
{
    std::for_each(types.begin()
                , types.end()
                , [&](std::pair<TokenType, std::pair<AutomationCreator, bool>> t)
                  {
                      TokenType tp(t.first);
                      AutomationCreator creator(t.second.first);
                      bool shiftToken(t.second.second);
                      _actions[tp] = [=](AutomationStack& stack, TypedToken const& token)
                                     {
                                         stack.push(creator(token));
                                         if (shiftToken) {
                                             stack.top()->nextToken(stack, token);
                                         }
                                     };
                  });
}

void ClauseStackWrapper::pushBlockReceiver(AutomationBase* blockRecr)
{
    _clauses.push_back(util::mkptr(new BlockReceiverClause(last_indent, _stack, _pos, blockRecr)));
}

void ClauseStackWrapper::pushClause(util::sptr<ClauseBase> clause)
{
    _clauses.push_back(std::move(clause));
}

util::sref<ClauseBase> ClauseStackWrapper::lastClause() const
{
    return *_clauses.back();
}
