#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include "parser.h"
#include <iostream>

LexicalAnalyzer lexer; // global lexer to get and unget tokens

vector<string> var; // holds all variable names initalized in var_section

// helpful parsing functions
void syntax_error()
{
    cout << "SYNTAX ERROR !!!\n";
    exit(1);
}

Token expectToken(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

Token peek()
{
    Token t = lexer.GetToken();
    lexer.UngetToken(t);
    return t;
}


// program -> var_section body inputs
struct InstructionNode* program()
{
    struct InstructionNode* finalList;

    var_section();
    finalList = body();
    parse_inputs();

    return finalList;
}

// var_section -> id_list SEMICOLON
void var_section()
{
    id_list();
    expectToken(SEMICOLON);
}

// id_list -> ID COMMA id_list | ID
void id_list()
{
    Token t = expectToken(ID);

    /*
    puts the variable name in var vector and sets it equal to 0 in the mem arry
    the name in var vector should be the same index as the one in mem arry
    */
    if (location(t.lexeme) == -1)
    {
        var.push_back(t.lexeme);
        mem[next_available] = 0;
        next_available++;
    }

    t = peek();

    if (t.token_type == COMMA)
    {
        expectToken(COMMA);
        id_list();
    }
}

// body -> LBRACE stmt_list RBRACE
struct InstructionNode* body()
{
    struct InstructionNode* list;

    expectToken(LBRACE);
    list = stmt_list();
    expectToken(RBRACE);

    return list;
}

// stmt_list -> stmt stmt_list | stmt
struct InstructionNode* stmt_list()
{
    struct InstructionNode* state; // for one statment 
    struct InstructionNode* slist; // list for statment list
    
    state = stmt();

    Token t = peek();

    struct InstructionNode* temp = state;

    if (t.token_type == ID || t.token_type == WHILE || t.token_type == IF || t.token_type == SWITCH || t.token_type == FOR || t.token_type == OUTPUT || t.token_type == INPUT)
    {
        slist = stmt_list();

        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = slist;

        return state;
    }
    else
    {
        return state;
    }
}

// stmt -> assign_stmt | while_stmt | if_stmt | switch_stmt | for_stmt | output_stmt | input_stmt
struct InstructionNode* stmt()
{
    Token t = peek();

    switch (t.token_type)
    {
    case ID:
        return assign_stmt();
        break;

    case WHILE:
        return while_stmt();
        break;

    case IF:
        return if_stmt();
        break;

    case SWITCH:
        return switch_stmt();
        break;

    case FOR:
        return for_stmt();
        break;

    case OUTPUT:
        return output_stmt();
        break;

    case INPUT:
        return input_stmt();
        break;
    }
}

// assign_stmt -> ID EQUAL primary SEMICOLON | ID EQUAL expr SEMICOLON
struct InstructionNode* assign_stmt()
{
    Token t = expectToken(ID);

    /*
    creates a new node
    */
    struct InstructionNode* assignInstruct = new InstructionNode;
    assignInstruct->type = ASSIGN;
    assignInstruct->assign_inst.left_hand_side_index = location(t.lexeme);
    assignInstruct->next = NULL;

    expectToken(EQUAL);
    Token t1 = lexer.GetToken(); // ID | NUM
    Token t2 = lexer.GetToken(); // if expr, should be PLUS | MINUS | MULT | DIV

    if (t2.token_type == PLUS || t2.token_type == MINUS || t2.token_type == MULT || t2.token_type == DIV)
    {
        lexer.UngetToken(t2);
        lexer.UngetToken(t1);

        assignInstruct->assign_inst.operand1_index = location(primary().lexeme);

        // switch statment to determine which type of operator it is 
        switch (op().token_type)
        {
        case PLUS:
            assignInstruct->assign_inst.op = OPERATOR_PLUS;
            break;

        case MINUS:
            assignInstruct->assign_inst.op = OPERATOR_MINUS;
            break;

        case MULT:
            assignInstruct->assign_inst.op = OPERATOR_MULT;
            break;

        case DIV:
            assignInstruct->assign_inst.op = OPERATOR_DIV;
            break;
        }

        assignInstruct->assign_inst.operand2_index = location(primary().lexeme);
    }
    else
    {
        lexer.UngetToken(t2);
        lexer.UngetToken(t1);

        assignInstruct->assign_inst.op = OPERATOR_NONE; 
        assignInstruct->assign_inst.operand1_index = location(primary().lexeme);
    }

    expectToken(SEMICOLON);

    return assignInstruct;
}

/*
--------
USELESS
--------

// expr -> primary op primary
Token* expr()
{

    tArray[0] = primary();
    tArray[1] = op();
    tArray[2] = primary();

    return tArray;
}

*/

// primary -> ID | NUM
Token primary()
{
    Token t = peek();

    if (t.token_type == ID)
    {
        t = expectToken(ID);

        /*
        puts the variable name in var vector and sets it equal to 0 in the mem arry
        the name in var vector should be the same index as the one in mem arry
        */
        if (location(t.lexeme) == -1) // makes sure that the variable doesn't exist 
        {
            var.push_back(t.lexeme);
            mem[next_available] = 0;
            next_available++;
        }

        return t;
    }
    else
    {
        t = expectToken(NUM);

        /*
        puts the variable name in var vector and sets it equal to 0 in the mem arry
        the name in var vector should be the same index as the one in mem arry
        */
        if (location(t.lexeme) == -1) // makes sure that the number doesn't exist
        {
            var.push_back(t.lexeme);
            mem[next_available] = std::stoi(t.lexeme);
            next_available++;
        }

        return t;
    }
}

// op -> PLUS | MINUS | MULT | DIV
Token op()
{
    Token t = peek();

    switch (t.token_type)
    {
    case PLUS:
        t = expectToken(PLUS);
        break;

    case MINUS:
        t = expectToken(MINUS);
        break;

    case MULT:
        t = expectToken(MULT);
        break;

    case DIV:
        t = expectToken(DIV);
        break;
    }

    return t;
}

// output_stmt -> OUTPUT ID SEMICOLON
struct InstructionNode* output_stmt()
{
    expectToken(OUTPUT);
    Token t = expectToken(ID);

    /*
    creates a new node
    */
    struct InstructionNode* new_output = new InstructionNode;
    new_output->type = OUT;
    new_output->output_inst.var_index = location(t.lexeme);
    new_output->next = NULL;

    expectToken(SEMICOLON);
    return new_output; // returns the new node
}

// input_stmt -> INPUT ID SEMICOLON
struct InstructionNode* input_stmt()
{
    expectToken(INPUT);
    Token t = expectToken(ID);
    
    /*
    creates a new node 
    */
    struct InstructionNode* new_input = new InstructionNode;
    new_input->type = IN;
    new_input->input_inst.var_index = location(t.lexeme);
    new_input->next = NULL;

    expectToken(SEMICOLON);
    return new_input; // returns the new node
}

// while_stmt -> WHILE condition body
struct InstructionNode* while_stmt()
{
    expectToken(WHILE);

    struct InstructionNode* while_node = new InstructionNode;
    while_node->type = CJMP;
    while_node->cjmp_inst.operand1_index = location(primary().lexeme);

    switch (relop().token_type)
    {
    case GREATER:
        while_node->cjmp_inst.condition_op = CONDITION_GREATER;
        break;

    case LESS:
        while_node->cjmp_inst.condition_op = CONDITION_LESS;
        break;

    case NOTEQUAL:
        while_node->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        break;
    }
    
    while_node->cjmp_inst.operand2_index = location(primary().lexeme);
    while_node->next = body();

    struct InstructionNode* jump_node = new InstructionNode;
    jump_node->type = JMP;
    jump_node->jmp_inst.target = while_node;

    // appends jump_node to end of while_node
    struct InstructionNode* temp = while_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = jump_node;

    struct InstructionNode* no_operate = new InstructionNode;
    no_operate->type = NOOP;
    no_operate->next = NULL;

    while_node->cjmp_inst.target = no_operate;

    // adds no-op to end of while_node
    temp = while_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = no_operate;

    return while_node;
}

// if_stmt -> IF condition body
struct InstructionNode* if_stmt()
{
    expectToken(IF);

    struct InstructionNode* if_node = new InstructionNode;
    if_node->type = CJMP;
    if_node->cjmp_inst.operand1_index = location(primary().lexeme);

    switch(relop().token_type)
    {
    case GREATER:
        if_node->cjmp_inst.condition_op = CONDITION_GREATER;
        break;

    case LESS:
        if_node->cjmp_inst.condition_op = CONDITION_LESS;
        break;

    case NOTEQUAL:
        if_node->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        break;
    }

    if_node->cjmp_inst.operand2_index = location(primary().lexeme);
    if_node->next = body();

    struct InstructionNode* no_operate = new InstructionNode;
    no_operate->type = NOOP;
    no_operate->next = NULL;
    
    if_node->cjmp_inst.target = no_operate;

    struct InstructionNode* temp = if_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = no_operate; // adds no-op to the end of body of if

    return if_node;
}

/*
----------
USELESS
----------
// condition -> primary relop primary
void condition()
{
    primary();
    relop();
    primary();
}
*/


// relop -> GREATER | LESS | NOTEQUAL
Token relop()
{
    Token t = peek();

    switch (t.token_type)
    {
    case GREATER:
        return expectToken(GREATER);

    case LESS:
        return expectToken(LESS);

    case NOTEQUAL:
        return expectToken(NOTEQUAL);
    }
    
}

// switch_stmt -> SWITCH ID LBRACE case_list RBRACE | SWITCH ID LBRACE case_list default_case RBRACE
struct InstructionNode* switch_stmt()
{
    expectToken(SWITCH);

    string var = expectToken(ID).lexeme; // maybe pass as variable to case_list

    expectToken(LBRACE);

    struct InstructionNode* switch_node = case_list();

    // creates a temp node
    struct InstructionNode* temp = switch_node;

    // adds noop node to end of each body in case_list
    struct InstructionNode* no_operate = new InstructionNode;
    no_operate->type = NOOP;
    no_operate->next = NULL;

    // adds the switch var to operand2 and noop to end of each body in each case
    while (temp->next != NULL)
    {
        temp->cjmp_inst.operand2_index = location(var); // adds switch variable

        struct InstructionNode* temp_body = temp->cjmp_inst.target;
        while (temp_body->next != NULL)
        {
            temp_body = temp_body->next;
        }
        temp_body->next = no_operate; // puts noop at the end of the body

        temp = temp->next; // goes to next case
    }
    temp->cjmp_inst.operand2_index = location(var); // accounts for last/first case

    struct InstructionNode* temp_body = temp->cjmp_inst.target;
    while (temp_body->next != NULL)
    {
        temp_body = temp_body->next;
    }
    temp_body->next = no_operate; // puts noop at the end of the body for last/first case


    Token t = peek();

    if (t.token_type == DEFAULT)
    {
        
        struct InstructionNode* def_node = default_case();
        struct InstructionNode* temp1 = switch_node;
        while (temp1->next != NULL)
        {
            temp1 = temp1->next;
        }
        temp1->next = def_node;

        struct InstructionNode* temp2 = def_node;
        while (temp2->next != NULL)
        {
            temp2 = temp2->next;
        }
        temp2->next = no_operate;
        
    }
    else if (t.token_type == RBRACE)
    {
        struct InstructionNode* temp3 = switch_node;
        while (temp3->next != NULL)
        {
            temp3 = temp3->next;
        }
        temp3->next = no_operate;
    }

    expectToken(RBRACE);

    return switch_node;
}

// for_stmt -> FOR LPAREN assign_stmt condition SEMICOLON assign_stmt RPAREN body
struct InstructionNode* for_stmt()
{
    expectToken(FOR);
    expectToken(LPAREN);

    struct InstructionNode* assign1 = assign_stmt();

    struct InstructionNode* for_node = new InstructionNode;
    assign1->next = for_node; // links assign1 to for_node
    for_node->type = CJMP;
    // condition: start 
    for_node->cjmp_inst.operand1_index = location(primary().lexeme);

    switch (relop().token_type)
    {
    case GREATER:
        for_node->cjmp_inst.condition_op = CONDITION_GREATER;
        break;

    case LESS:
        for_node->cjmp_inst.condition_op = CONDITION_LESS;
        break;

    case NOTEQUAL:
        for_node->cjmp_inst.condition_op = CONDITION_NOTEQUAL;
        break;
    }

    for_node->cjmp_inst.operand2_index = location(primary().lexeme);
    // end
    expectToken(SEMICOLON);

    struct InstructionNode* assign2 = assign_stmt();
    
    expectToken(RPAREN);

    for_node->next = body(); // links for_node to body

    struct InstructionNode* temp = for_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = assign2; // links end of body to assign2

    struct InstructionNode* jump_node = new InstructionNode;
    jump_node->type = JMP;
    jump_node->jmp_inst.target = for_node;

    // appends jump_node to end of for_node
    temp = for_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = jump_node;

    struct InstructionNode* no_operate = new InstructionNode;
    no_operate->type = NOOP;
    no_operate->next = NULL;

    for_node->cjmp_inst.target = no_operate;

    // adds no-op to end of for_node
    temp = for_node;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = no_operate;

    return assign1;
    
}

// case_list -> case case_list | case
struct InstructionNode* case_list()
{
    struct InstructionNode* case_node; // for one case

    case_node = parse_case();

    Token t = peek();

    struct InstructionNode* temp = case_node;

    if (t.token_type == CASE)
    {
        temp->next = case_list();
    }
    
    return case_node;
}



// case -> CASE NUM COLON body
struct InstructionNode* parse_case()
{
    expectToken(CASE);
    Token t = expectToken(NUM);
    if (location(t.lexeme) == -1) // makes sure the number doesn't exist
    {
        var.push_back(t.lexeme);
        mem[next_available] = std::stoi(t.lexeme);
        next_available++;
    }

    struct InstructionNode* case_node = new InstructionNode;
    case_node->type = CJMP;
    case_node->cjmp_inst.operand1_index = location(t.lexeme);
    case_node->cjmp_inst.condition_op = CONDITION_NOTEQUAL; // using reverse logic for switch
    case_node->next = NULL;

    expectToken(COLON);

    case_node->cjmp_inst.target = body();

    return case_node;
}



// default_case -> DEFAULT COLON body
struct InstructionNode* default_case()
{
    expectToken(DEFAULT);
    expectToken(COLON);

    struct InstructionNode* default_node = body();

    return default_node;
}


// inputs -> num_list
void parse_inputs()
{
    num_list();
}

// num_list -> NUM num_list | NUM 
void num_list()
{
    Token t = expectToken(NUM);

    /*
    stores input value in inputs vector
    */
    inputs.push_back(std::stoi(t.lexeme));

    t = peek();

    if (t.token_type == NUM)
    {
        num_list();
    }
}

// OTHER FUNCTIONS

/*

returns index of string in mem vector

*/
int location(string var_name)
{
    int result = -1; 
    
    for (int i = 0; i < var.size(); i++)
    {
        if (var_name == var.at(i))
        {
            result = i;
            i = var.size(); // imediately stops the loop
        }
    }

    return result;
}



// WHERE THE PARSER STARTS
struct InstructionNode* parse_generate_intermediate_representation()
{
    return program(); // parses through program
}
