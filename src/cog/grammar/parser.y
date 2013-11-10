%defines
%locations
%pure-parser
%name-prefix="gra_"
%error-verbose
%verbose
%lex-param { void* scanner }
%parse-param { gorc::cog::grammar::instance* instance }

%expect 1
%expect-rr 0

%code requires {
	#include "framework/text/location.h"
	#define YYLTYPE YYLTYPE
	typedef gorc::text::location YYLTYPE;
	
	#define YYLLOC_DEFAULT(Cur, Rhs, N)                        \
     do                                                        \
       if (N)                                                  \
         {                                                     \
           (Cur).first_line   = YYRHSLOC(Rhs, 1).first_line;   \
           (Cur).first_column = YYRHSLOC(Rhs, 1).first_column; \
           (Cur).last_line    = YYRHSLOC(Rhs, N).last_line;    \
           (Cur).last_column  = YYRHSLOC(Rhs, N).last_column;  \
           (Cur).filename     = YYRHSLOC(Rhs, N).filename;     \
         }                                                     \
       else                                                    \
         {                                                     \
           (Cur).first_line   = (Cur).last_line   =            \
             YYRHSLOC(Rhs, 0).last_line;                       \
           (Cur).first_column = (Cur).last_column =            \
             YYRHSLOC(Rhs, 0).last_column;                     \
           (Cur).filename = YYRHSLOC(Rhs, 0).filename;         \
         }                                                     \
     while (0)
}

%code top {
	#include "instance.h"
}

%union
{
	char* string;
	char character;
	int integer;
	float floating;

	gorc::cog::ast::symbol_field* symbol_field;
	gorc::cog::ast::symbol_extension* symbol_extension;
	std::vector<gorc::cog::ast::symbol_extension*>* symbol_extension_list;
	gorc::cog::ast::symbol* symbol;
	std::vector<gorc::cog::ast::symbol*>* symbol_list;

	gorc::cog::ast::unary_operator unary_operator;
	gorc::cog::ast::infix_operator infix_operator;
	gorc::cog::ast::expression* expression;
	std::vector<gorc::cog::ast::expression*>* expression_list;

	gorc::cog::ast::statement* statement;
	std::vector<gorc::cog::ast::statement*>* statement_list;
	gorc::cog::ast::for_optional_expression* for_optional_expression;

	gorc::cog::ast::translation_unit* translation_unit;
}

/* Main keyword tokens */
%token FLAGS SYMBOLS CODE END

/* Symbols keyword tokens */
%token ENDLINE

/* Code keyword tokens */
%token BREAK CALL DO ELSE FOR IF RETURN WHILE

/* Code operator tokens */
%token EQ_OP NE_OP GE_OP LE_OP LOGICAL_AND_OP LOGICAL_OR_OP

/* Literals */
%token <string> IDENTIFIER STRING_FRAGMENT STRING_LITERAL
%token <integer> INTEGER_LITERAL
%token <floating> FLOAT_LITERAL

/* Symbol section */
%type <symbol_field> symbol_field
%type <symbol_extension> symbol_extension
%type <symbol_extension_list> symbol_extension_seq symbol_extension_part
%type <symbol> symbol
%type <symbol_list> symbol_seq symbol_section

/* Expressions */
%type <floating> vector_component
%type <unary_operator> unary_operator
%type <infix_operator> multiplicative_operator additive_operator relational_operator equality_operator
%type <expression> literal_expression primary_expression postfix_expression unary_expression multiplicative_expression
%type <expression> additive_expression relational_expression equality_expression and_expression exclusive_or_expression
%type <expression> inclusive_or_expression logical_and_expression logical_or_expression assignment_expression expression
%type <expression_list> argument_expression_list

/* Statements */
%type <statement> compound_statement expression_statement jump_statement selection_statement iteration_statement labeled_statement statement
%type <statement_list> statement_seq code_section
%type <for_optional_expression> for_optional_expression

/* Translation unit */
%type <translation_unit> translation_unit

/* Destructors */
%destructor { free($$); } <string>

%start translation_unit

%{
	using namespace gorc::cog;
	using namespace gorc::cog::ast;
	
	int yylex(YYSTYPE* lvalp, gorc::text::location* llocp, void* scanner);
	
	void yyerror(gorc::text::location* llocp, gorc::cog::grammar::instance* instance, const char* err) {
		instance->report.add_error("parser", err, *llocp);
	}
	
	#define scanner instance->get_scanner()
	#define ast instance->factory
%}

%%

/********** Symbols **********/
symbol_field
	: STRING_FRAGMENT
		{
			$$ = ast.make<string_fragment_field>(@$, $1);
		}
	| INTEGER_LITERAL
		{
			$$ = ast.make<integer_field>(@$, $1);
		}
	| FLOAT_LITERAL
		{
			$$ = ast.make<float_field>(@$, $1);
		}
	;

symbol_extension
	: IDENTIFIER
		{
			$$ = ast.make<bare_extension>(@$, $1);
		}
	| IDENTIFIER '=' symbol_field
		{
			$$ = ast.make<valued_extension>(@$, $1, $3);
		}
	;

symbol_extension_seq
	: symbol_extension
		{
			$$ = ast.make_list($1);
		}
	| symbol_extension_seq symbol_extension
		{
			($1)->push_back($2);
			$$ = $1;
		}
	;

symbol_extension_part
	: /* Blank */
		{
			$$ = ast.make_list<symbol_extension>();
		}
	| symbol_extension_seq
		{
			$$ = $1;
		}
	;

symbol
	: IDENTIFIER IDENTIFIER symbol_extension_part ENDLINE
		{
			$$ = ast.make<symbol>(@$, $1, $2, $3);
		}
	| IDENTIFIER IDENTIFIER '=' symbol_field symbol_extension_part ENDLINE
		{
			$$ = ast.make<valued_symbol>(@$, $1, $2, $5, $4);
		}
	| ENDLINE
		{
			$$ = nullptr;
		}
	;

symbol_seq
	: symbol
		{
			if($1 == nullptr)
			{
				$$ = ast.make_list<symbol>();
			}
			else
			{
				$$ = ast.make_list($1);
			}
		}
	| symbol_seq symbol
		{
			if($2 != nullptr)
			{
				($1)->push_back($2);
			}

			$$ = $1;
		}
	;

symbol_section
	:
		{
			$$ = ast.make_list<symbol>();
		}
	| symbol_seq
		{
			$$ = $1;
		}
	;

/********** Expression **********/
argument_expression_list
	: assignment_expression
		{
			$$ = ast.make_list($1);
		}
	| argument_expression_list ',' assignment_expression
		{
			($1)->push_back($3);
			$$ = $1;
		}
	;

vector_component
	: INTEGER_LITERAL
		{
			$$ = static_cast<float>($1);
		}
	| FLOAT_LITERAL
		{
			$$ = $1;
		}
	| '+' INTEGER_LITERAL
		{
			$$ = static_cast<float>($2);
		}
	| '+' FLOAT_LITERAL
		{
			$$ = $2;
		}
	| '-' INTEGER_LITERAL
		{
			$$ = -static_cast<float>($2);
		}
	| '-' FLOAT_LITERAL
		{
			$$ = -$2;
		}
	;

literal_expression
	: STRING_LITERAL
		{
			$$ = ast.make<string_literal_expression>(@$, $1);
		}
	| INTEGER_LITERAL
		{
			$$ = ast.make<integer_literal_expression>(@$, $1);
		}
	| FLOAT_LITERAL
		{
			$$ = ast.make<float_literal_expression>(@$, $1);
		}
	| '\'' vector_component vector_component vector_component '\''
		{
			$$ = ast.make<vector_literal_expression>(@$, $2, $3, $4);
		}
	| '\'' vector_component ',' vector_component ',' vector_component '\''
		{
			$$ = ast.make<vector_literal_expression>(@$, $2, $4, $6);
		}
	;

primary_expression
	: literal_expression
		{
			$$ = $1;
		}
	| '(' expression ')'
		{
			$$ = $2;
		}
	| IDENTIFIER
		{
			$$ = ast.make<identifier_expression>(@$, $1);
		}
	;

postfix_expression
	: primary_expression
		{
			$$ = $1;
		}
	| IDENTIFIER '[' expression ']'
		{
			$$ = ast.make<subscript_expression>(@$, $1, $3);
		}
	| IDENTIFIER '(' ')'
		{
			$$ = ast.make<method_call_expression>(@$, $1, ast.make_list<expression>());
		}
	| IDENTIFIER '(' argument_expression_list ')'
		{
			$$ = ast.make<method_call_expression>(@$, $1, $3);
		}
	;

unary_operator
	: '+'
		{
			$$ = unary_operator::plus;
		}
	| '-'
		{
			$$ = unary_operator::minus;
		}
	| '!'
		{
			$$ = unary_operator::logical_not;
		}
	;

unary_expression
	: postfix_expression
		{
			$$ = $1;
		}
	| unary_operator unary_expression
		{
			$$ = ast.make<unary_expression>(@$, $2, $1);
		}
	;

multiplicative_operator
	: '*'
		{
			$$ = infix_operator::multiplication;
		}
	| '/'
		{
			$$ = infix_operator::division;
		}
	| '%'
		{
			$$ = infix_operator::modulo;
		}
	;

multiplicative_expression
	: unary_expression
		{
			$$ = $1;
		}
	| multiplicative_expression multiplicative_operator unary_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, $2);
		}
	;

additive_operator
	: '+'
		{
			$$ = infix_operator::addition;
		}
	| '-'
		{
			$$ = infix_operator::subtraction;
		}
	;

additive_expression
	: multiplicative_expression
		{
			$$ = $1;
		}
	| additive_expression additive_operator multiplicative_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, $2);
		}
	;

relational_operator
	: GE_OP
		{
			$$ = infix_operator::greater_equal;
		}
	| LE_OP
		{
			$$ = infix_operator::less_equal;
		}
	| '>'
		{
			$$ = infix_operator::greater;
		}
	| '<'
		{
			$$ = infix_operator::less;
		}
	;

relational_expression
	: additive_expression
		{
			$$ = $1;
		}
	| relational_expression relational_operator additive_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, $2);
		}
	;

equality_operator
	: EQ_OP
		{
			$$ = infix_operator::equal;
		}
	| NE_OP
		{
			$$ = infix_operator::not_equal;
		}
	;

equality_expression
	: relational_expression
		{
			$$ = $1;
		}
	| equality_expression equality_operator relational_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, $2);
		}
	;

and_expression
	: equality_expression
		{
			$$ = $1;
		}
	| and_expression '&' equality_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, infix_operator::bitwise_and);
		}
	;

exclusive_or_expression
	: and_expression
		{
			$$ = $1;
		}
	| exclusive_or_expression '^' and_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, infix_operator::bitwise_xor);
		}
	;

inclusive_or_expression
	: exclusive_or_expression
		{
			$$ = $1;
		}
	| inclusive_or_expression '|' exclusive_or_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, infix_operator::bitwise_or);
		}
	;

logical_and_expression
	: inclusive_or_expression
		{
			$$ = $1;
		}
	| logical_and_expression LOGICAL_AND_OP inclusive_or_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, infix_operator::logical_and);
		}
	;

logical_or_expression
	: logical_and_expression
		{
			$$ = $1;
		}
	| logical_or_expression LOGICAL_OR_OP logical_and_expression
		{
			$$ = ast.make<infix_expression>(@$, $1, $3, infix_operator::logical_or);
		}
	;

assignment_expression
	: logical_or_expression
		{
			$$ = $1;
		}
	| unary_expression '=' assignment_expression
		{
			$$ = ast.make<assignment_expression>(@$, $1, $3);
		}
	;

expression
	: assignment_expression
		{
			$$ = $1;
		}
	| expression ',' assignment_expression
		{
			$$ = ast.make<comma_expression>(@$, $1, $3);
		}
	;

/********** Statement **********/
statement_seq
	: statement
		{
			$$ = ast.make_list($1);
		}
	| statement_seq statement
		{
			($1)->push_back($2);
			$$ = $1;
		}
	;

compound_statement
	: '{' '}'
		{
			$$ = ast.make<compound_statement>(@$, ast.make_list<statement>());
		}
	| '{' statement_seq '}'
		{
			$$ = ast.make<compound_statement>(@$, $2);
		}
	;

expression_statement
	: expression ';'
		{
			$$ = ast.make<expression_statement>(@$, $1);
		}
	| ';'
		{
			$$ = ast.make<empty_statement>(@$);
		}
	;

jump_statement
	: BREAK ';'
		{
			$$ = ast.make<break_statement>(@$);
		}
	| RETURN ';'
		{
			$$ = ast.make<return_statement>(@$);
		}
	| CALL IDENTIFIER ';'
		{
			$$ = ast.make<call_statement>(@$, $2);
		}
	;

selection_statement
	: IF '(' expression ')' statement
		{
			$$ = ast.make<if_statement>(@$, $3, $5);
		}
	| IF '(' expression ')' statement ELSE statement
		{
			$$ = ast.make<if_else_statement>(@$, $3, $5, $7);
		}
	;

for_optional_expression
	:
		{
			$$ = ast.make<for_optional_expression>(@$);
		}
	| expression
		{
			$$ = ast.make<for_expression>(@$, $1);
		}
	;

iteration_statement
	: WHILE '(' expression ')' statement
		{
			$$ = ast.make<while_statement>(@$, $3, $5);
		}
	| DO statement WHILE '(' expression ')' ';'
		{
			$$ = ast.make<do_statement>(@$, $2, $5);
		}
	| FOR '(' for_optional_expression ';' for_optional_expression ';' for_optional_expression ')' statement
		{
			$$ = ast.make<for_statement>(@$, $3, $5, $7, $9);
		}
	;

labeled_statement
	: IDENTIFIER ':' statement
		{
			$$ = ast.make<labeled_statement>(@$, $1, $3);
		}
	;

statement
	: expression_statement
		{
			$$ = $1;
		}
	| jump_statement
		{
			$$ = $1;
		}
	| selection_statement
		{
			$$ = $1;
		}
	| iteration_statement
		{
			$$ = $1;
		}
	| compound_statement
		{
			$$ = $1;
		}
	| labeled_statement
		{
			$$ = $1;
		}
	;

/********** Code **********/
code_section
	:
		{
			$$ = ast.make_list<statement>();
		}
	| statement_seq
		{
			$$ = $1;
		}
	;

/********** Translation Unit **********/
translation_unit
	: SYMBOLS symbol_section END CODE code_section END
		{
			instance->set_return_value($$ = ast.make<translation_unit>(@$, 0, $2, $5));
		}
	| FLAGS '=' INTEGER_LITERAL SYMBOLS symbol_section END CODE code_section END
		{
			instance->set_return_value($$ = ast.make<translation_unit>(@$, $3, $5, $8));
		}
	;
