#include <gtest/gtest.h>

#include <grammar/stmt-nodes.h>
#include <grammar/expr-nodes.h>
#include <test/phony-errors.h>

#include "test-common.h"

using namespace test;

typedef GrammarTest ClauseTest;

TEST_F(ClauseTest, FuncClause)
{
    misc::position pos(1);
    TestClause receiver;

    grammar::FunctionClause func_acc0(0
                                    , pos
                                    , "func1"
                                    , std::vector<std::string>({ "Duke", "Duran" })
                                    , -1
                                    , false
                                    , util::mkref(receiver));
    func_acc0.acceptStmt(util::mkptr(new grammar::Arithmetics(
                      pos, util::mkptr(new grammar::FloatLiteral(pos, "21.37")))));
    func_acc0.acceptStmt(util::mkptr(new grammar::NameDef(
                      pos, "SonOfKorhal", util::mkptr(new grammar::IntLiteral(pos, "20110116")))));

    func_acc0.deliver();
    receiver.compile()->compile(nulScope());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "func1")
            (pos, PARAMETER, "Duke")
            (pos, PARAMETER, "Duran")
            (BLOCK_BEGIN)
            (pos, ARITHMETICS)
                (pos, FLOATING, "21.37")
            (pos, NAME_DEF, "SonOfKorhal")
                (pos, INTEGER, "20110116")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());

    misc::position pos_else(10);
    TestClause test_receiver;
    grammar::FunctionClause func_acc1(0
                                    , pos
                                    , "func2"
                                    , std::vector<std::string>({ "Mengsk" })
                                    , -1
                                    , false
                                    , util::mkref(test_receiver));
    func_acc1.acceptElse(pos_else, util::mkptr(new grammar::Block));
    ASSERT_TRUE(error::hasError());

    std::vector<PartialStmtNotMatchRec> recs(getPartialStmtNotMatchRecs());
    ASSERT_EQ(1, recs.size());
    ASSERT_EQ(pos_else, recs[0].pos);
    ASSERT_EQ("else", recs[0].successor);
    ASSERT_EQ("if", recs[0].match);
}

TEST_F(ClauseTest, FuncAccNested)
{
    misc::position pos(5);
    TestClause receiver;

    grammar::FunctionClause func_acc0(0
                                    , pos
                                    , "funca"
                                    , std::vector<std::string>({ "firebat", "ghost" })
                                    , -1
                                    , false
                                    , util::mkref(receiver));
    func_acc0.acceptStmt(util::mkptr(new grammar::Arithmetics(pos, util::mkptr(
                                                new grammar::FloatLiteral(pos, "22.15")))));
    func_acc0.acceptStmt(util::mkptr(new grammar::NameDef(pos, "medic", util::mkptr(
                                                new grammar::Identifier(pos, "wraith")))));

    grammar::FunctionClause func_acc1(0
                                    , pos
                                    , "funca"
                                    , std::vector<std::string>({ "vulture" })
                                    , -1
                                    , false
                                    , util::mkref(func_acc0));
    func_acc1.acceptStmt(util::mkptr(new grammar::Arithmetics(pos, util::mkptr(
                                                new grammar::Identifier(pos, "goliath")))));

    func_acc1.deliver();
    func_acc0.deliver();
    receiver.compile()->compile(nulScope());

    DataTree::expectOne()
        (BLOCK_BEGIN)
        (pos, FUNC_DEF, "funca")
            (pos, PARAMETER, "firebat")
            (pos, PARAMETER, "ghost")
            (BLOCK_BEGIN)
            (pos, FUNC_DEF, "funca")
                (pos, PARAMETER, "vulture")
                (BLOCK_BEGIN)
                (pos, ARITHMETICS)
                    (pos, IDENTIFIER, "goliath")
                (BLOCK_END)
            (pos, ARITHMETICS)
                (pos, FLOATING, "22.15")
            (pos, NAME_DEF, "medic")
                (pos, IDENTIFIER, "wraith")
            (BLOCK_END)
        (BLOCK_END)
    ;
    ASSERT_FALSE(error::hasError());
}
