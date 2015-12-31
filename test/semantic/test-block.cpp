#include <gtest/gtest.h>

#include <semantic/expr-nodes.h>
#include <semantic/stmt-nodes.h>
#include <semantic/symbol-table.h>
#include <semantic/function.h>
#include <test/phony-errors.h>
#include <test/common.h>

#include "test-common.h"

using namespace test;

struct BlockTest
    : SemanticTest
{
    BlockTest()
        : _scope(nullptr)
    {}

    void SetUp()
    {
        SemanticTest::SetUp();
        this->_scope = semantic::Scope::global();
    }

    util::sref<semantic::SymbolTable> refSym()
    {
        return this->_scope->sym();
    }

    util::sptr<semantic::Scope> _scope;
};

TEST_F(BlockTest, Block)
{
    misc::position pos(1);

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "1"))
                                 , "+"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "235.7"))));

    semantic::Block block0(pos);
    refSym()->defName(pos, "kobayakawa");
    refSym()->defName(pos, "yutaka");
    refSym()->defName(pos, "tamura");
    refSym()->defName(pos, "hiyori");
    refSym()->defName(pos, "minami");

    block0.addStmt(util::mkptr(new semantic::Arithmetics(pos, util::mkptr(
                                        new semantic::BoolLiteral(pos, true)))));
    block0.addStmt(util::mkptr(new semantic::NameDef(pos, "soujirou", std::move(binary))));
    block0.addStmt(util::mkptr(new semantic::NameDef(pos, "iwasaki", util::mkptr(
                                                new semantic::Reference(pos, "minami")))));
    util::sptr<semantic::Block> consq_block(new semantic::Block(pos));
    misc::position pos_consq(100);
    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_consq, util::mkptr(
                                        new semantic::Reference(pos_consq, "kobayakawa")))));
    util::sptr<semantic::Block> alter_block(new semantic::Block(pos));
    misc::position pos_alter(101);
    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter , util::mkptr(
                                        new semantic::Reference(pos_alter, "yutaka")))));
    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter , util::mkptr(
                                        new semantic::Reference(pos_alter, "hiyori")))));

    block0.addStmt(util::mkptr(new semantic::Branch(
                      pos
                    , util::mkptr(new semantic::Reference(pos, "tamura"))
                    , std::move(consq_block)
                    , std::move(alter_block))));
    block0.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::Undefined(pos)))));

    compile(block0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (FWD_DECL, "iwasaki")
            (ARITHMETICS)
                (pos, BINARY_OP, "[=]")
                    (pos, REFERENCE, "iwasaki")
                    (pos, REFERENCE, "minami")
            (BRANCH)
                (pos, REFERENCE, "tamura")
            (SCOPE_BEGIN)
            (SCOPE_END)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(BlockTest, TwoPathBranchFoldedOnFalse)
{
    misc::position pos(6);
    refSym()->defName(pos, "yui");
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , ">"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Block block0(pos);
    util::sptr<semantic::Block> consq_block(new semantic::Block(pos));
    misc::position pos_consq(600);
    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_consq, util::mkptr(
                                            new semantic::Reference(pos_consq, "yui")))));
    util::sptr<semantic::Block> alter_block(new semantic::Block(pos));
    misc::position pos_alter(601);
    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter, util::mkptr(
                                            new semantic::Reference(pos_alter, "narumi")))));

    block0.addStmt(util::mkptr(new semantic::Branch(
                    pos, std::move(binary), std::move(consq_block), std::move(alter_block))));
    block0.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::Undefined(pos)))));

    compile(block0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(BlockTest, TwoPathBranchFoldedOnTrue)
{
    misc::position pos(7);
    refSym()->defName(pos, "yui");
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , "<"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Block block0(pos);
    util::sptr<semantic::Block> consq_block(new semantic::Block(pos));
    misc::position pos_consq(700);
    consq_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_consq, util::mkptr(
                                                new semantic::Reference(pos_consq, "yui")))));
    util::sptr<semantic::Block> alter_block(new semantic::Block(pos));
    misc::position pos_alter(701);
    alter_block->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter, util::mkptr(
                                                new semantic::Reference(pos_alter, "narumi")))));

    block0.addStmt(util::mkptr(new semantic::Branch(
                    pos, std::move(binary), std::move(consq_block), std::move(alter_block))));
    block0.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::Undefined(pos)))));

    compile(block0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(BlockTest, IfNotFoldedOnFalse)
{
    misc::position pos(8);
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , ">"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Block block0(pos);
    util::sptr<semantic::Block> block_alter(new semantic::Block(pos));
    misc::position pos_alter(801);
    block_alter->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter, util::mkptr(
                                            new semantic::Reference(pos_alter, "narumi")))));

    block0.addStmt(util::mkptr(new semantic::Branch(
        pos, std::move(binary), util::mkptr(new semantic::Block(pos)), std::move(block_alter))));
    block0.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::Undefined(pos)))));

    compile(block0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}

TEST_F(BlockTest, IfNotFoldedOnTrue)
{
    misc::position pos(8);
    refSym()->defName(pos, "narumi");

    util::sptr<semantic::BinaryOp> binary(
            new semantic::BinaryOp(pos
                                 , util::mkptr(new semantic::IntLiteral(pos, "2048"))
                                 , "<"
                                 , util::mkptr(new semantic::FloatLiteral(pos, "11235.8"))));

    semantic::Block block0(pos);
    util::sptr<semantic::Block> block_alter(new semantic::Block(pos));
    misc::position pos_alter(801);
    block_alter->addStmt(util::mkptr(new semantic::Arithmetics(pos_alter, util::mkptr(
                                            new semantic::Reference(pos_alter, "narumi")))));

    block0.addStmt(util::mkptr(new semantic::Branch(
        pos, std::move(binary), util::mkptr(new semantic::Block(pos)), std::move(block_alter))));
    block0.addStmt(util::mkptr(new semantic::Return(
                    pos, util::mkptr(new semantic::Undefined(pos)))));

    compile(block0, refSym())->write(dummyos());
    EXPECT_FALSE(error::hasError());

    DataTree::expectOne()
        (SCOPE_BEGIN)
            (SCOPE_BEGIN)
            (SCOPE_END)
            (RETURN)
                (pos, UNDEFINED)
        (SCOPE_END)
    ;
}