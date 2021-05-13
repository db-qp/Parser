#ifndef _PARSER_H_
#define _PARSER_H_

#include "lexer.h"
#include "compiler.h"
#include "string.h"

using namespace std;

// FUNCTIONS
void syntax_error();
Token expectToken(TokenType expected_type);
Token peek();

struct InstructionNode* program();
void var_section();
void id_list();
struct InstructionNode* body();
struct InstructionNode* stmt_list();
struct InstructionNode* stmt();
struct InstructionNode* assign_stmt();
//Token* expr(); [USELESS]
Token primary();
Token op();
struct InstructionNode* output_stmt();
struct InstructionNode* input_stmt();
struct InstructionNode* while_stmt();
struct InstructionNode* if_stmt();
//void condition(); [USELESS]
Token relop();
struct InstructionNode* switch_stmt();
struct InstructionNode* for_stmt();
struct InstructionNode* case_list();
struct InstructionNode* parse_case();
struct InstructionNode* default_case();
void parse_inputs();
void num_list();

int location(string);









#endif /* _PARSER_H_ */
