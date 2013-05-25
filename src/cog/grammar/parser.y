%defines
%locations
%pure-parser
%name-prefix="gra_"
%error-verbose
%verbose
%lex-param { void* scanner }
%parse-param { Gorc::Cog::Grammar::Instance* instance }

%expect 1
%expect-rr 0

%code requires {
	#include "framework/diagnostics/errorlocation.h"
	#define YYLTYPE YYLTYPE
	typedef Gorc::Diagnostics::ErrorLocation YYLTYPE;
	
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

	Gorc::Cog::AST::SymbolField* symbol_field;
	Gorc::Cog::AST::SymbolExtension* symbol_extension;
	std::vector<Gorc::Cog::AST::SymbolExtension*>* symbol_extension_list;
	Gorc::Cog::AST::Symbol* symbol;
	std::vector<Gorc::Cog::AST::Symbol*>* symbol_list;

	Gorc::Cog::AST::UnaryOperator unary_operator;
	Gorc::Cog::AST::InfixOperator infix_operator;
	Gorc::Cog::AST::Expression* expression;
	std::vector<Gorc::Cog::AST::Expression*>* expression_list;

	Gorc::Cog::AST::Statement* statement;
	std::vector<Gorc::Cog::AST::Statement*>* statement_list;
	Gorc::Cog::AST::ForOptionalExpression* for_optional_expression;

	Gorc::Cog::AST::TranslationUnit* translation_unit;
}

/* Main keyword tokens */
%token SYMBOLS CODE END

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
	using namespace Gorc::Cog;
	using namespace Gorc::Cog::AST;
	
	int yylex(YYSTYPE* lvalp, Gorc::Diagnostics::ErrorLocation* llocp, void* scanner);
	
	void yyerror(Gorc::Diagnostics::ErrorLocation* llocp, Gorc::Cog::Grammar::Instance* instance, const char* err) {
		instance->Report.AddError("parser", err, *llocp);
	}
	
	#define scanner instance->GetScanner()
	#define ast instance->Factory
%}

%%

/********** Symbols **********/
symbol_field
	: STRING_FRAGMENT
		{
			$$ = ast.MakeStringFragmentField($1, @$);
		}
	| INTEGER_LITERAL
		{
			$$ = ast.MakeIntegerField($1, @$);
		}
	| FLOAT_LITERAL
		{
			$$ = ast.MakeFloatField($1, @$);
		}
	;

symbol_extension
	: IDENTIFIER
		{
			$$ = ast.MakeBareExtension($1, @$);
		}
	| IDENTIFIER '=' symbol_field
		{
			$$ = ast.MakeValuedExtension($1, $3, @$);
		}
	;

symbol_extension_seq
	: symbol_extension
		{
			$$ = ast.MakeList($1);
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
			$$ = ast.MakeList<SymbolExtension>();
		}
	| symbol_extension_seq
		{
			$$ = $1;
		}
	;

symbol
	: IDENTIFIER IDENTIFIER symbol_extension_part ENDLINE
		{
			$$ = ast.MakeSymbol($1, $2, $3, @$);
		}
	| IDENTIFIER IDENTIFIER '=' symbol_field symbol_extension_part ENDLINE
		{
			$$ = ast.MakeValuedSymbol($1, $2, $5, $4, @$);
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
				$$ = ast.MakeList<Symbol>();
			}
			else
			{
				$$ = ast.MakeList($1);
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
			$$ = ast.MakeList<Symbol>();
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
			$$ = ast.MakeList($1);
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
			$$ = ast.MakeStringLiteralExpression($1, @$);
		}
	| INTEGER_LITERAL
		{
			$$ = ast.MakeIntegerLiteralExpression($1, @$);
		}
	| FLOAT_LITERAL
		{
			$$ = ast.MakeFloatLiteralExpression($1, @$);
		}
	| '\'' vector_component vector_component vector_component '\''
		{
			$$ = ast.MakeVectorLiteralExpression($2, $3, $4, @$);
		}
	| '\'' vector_component ',' vector_component ',' vector_component '\''
		{
			$$ = ast.MakeVectorLiteralExpression($2, $4, $6, @$);
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
			$$ = ast.MakeIdentifierExpression($1, @$);
		}
	;

postfix_expression
	: primary_expression
		{
			$$ = $1;
		}
	| IDENTIFIER '[' expression ']'
		{
			$$ = ast.MakeSubscriptExpression($1, $3, @$);
		}
	| IDENTIFIER '(' ')'
		{
			$$ = ast.MakeMethodCallExpression($1, ast.MakeList<Expression>(), @$);
		}
	| IDENTIFIER '(' argument_expression_list ')'
		{
			$$ = ast.MakeMethodCallExpression($1, $3, @$);
		}
	;

unary_operator
	: '+'
		{
			$$ = UnaryOperator::Plus;
		}
	| '-'
		{
			$$ = UnaryOperator::Minus;
		}
	| '!'
		{
			$$ = UnaryOperator::Not;
		}
	;

unary_expression
	: postfix_expression
		{
			$$ = $1;
		}
	| unary_operator unary_expression
		{
			$$ = ast.MakeUnaryExpression($2, $1, @$);
		}
	;

multiplicative_operator
	: '*'
		{
			$$ = InfixOperator::Multiplication;
		}
	| '/'
		{
			$$ = InfixOperator::Division;
		}
	| '%'
		{
			$$ = InfixOperator::Modulo;
		}
	;

multiplicative_expression
	: unary_expression
		{
			$$ = $1;
		}
	| multiplicative_expression multiplicative_operator unary_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, $2, @$);
		}
	;

additive_operator
	: '+'
		{
			$$ = InfixOperator::Addition;
		}
	| '-'
		{
			$$ = InfixOperator::Subtraction;
		}
	;

additive_expression
	: multiplicative_expression
		{
			$$ = $1;
		}
	| additive_expression additive_operator multiplicative_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, $2, @$);
		}
	;

relational_operator
	: GE_OP
		{
			$$ = InfixOperator::GreaterEqual;
		}
	| LE_OP
		{
			$$ = InfixOperator::LessEqual;
		}
	| '>'
		{
			$$ = InfixOperator::Greater;
		}
	| '<'
		{
			$$ = InfixOperator::Less;
		}
	;

relational_expression
	: additive_expression
		{
			$$ = $1;
		}
	| relational_expression relational_operator additive_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, $2, @$);
		}
	;

equality_operator
	: EQ_OP
		{
			$$ = InfixOperator::Equal;
		}
	| NE_OP
		{
			$$ = InfixOperator::NotEqual;
		}
	;

equality_expression
	: relational_expression
		{
			$$ = $1;
		}
	| equality_expression equality_operator relational_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, $2, @$);
		}
	;

and_expression
	: equality_expression
		{
			$$ = $1;
		}
	| and_expression '&' equality_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, InfixOperator::And, @$);
		}
	;

exclusive_or_expression
	: and_expression
		{
			$$ = $1;
		}
	| exclusive_or_expression '^' and_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, InfixOperator::Xor, @$);
		}
	;

inclusive_or_expression
	: exclusive_or_expression
		{
			$$ = $1;
		}
	| inclusive_or_expression '|' exclusive_or_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, InfixOperator::Or, @$);
		}
	;

logical_and_expression
	: inclusive_or_expression
		{
			$$ = $1;
		}
	| logical_and_expression LOGICAL_AND_OP inclusive_or_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, InfixOperator::LogicalAnd, @$);
		}
	;

logical_or_expression
	: logical_and_expression
		{
			$$ = $1;
		}
	| logical_or_expression LOGICAL_OR_OP logical_and_expression
		{
			$$ = ast.MakeInfixExpression($1, $3, InfixOperator::LogicalOr, @$);
		}
	;

assignment_expression
	: logical_or_expression
		{
			$$ = $1;
		}
	| unary_expression '=' assignment_expression
		{
			$$ = ast.MakeAssignmentExpression($1, $3, @$);
		}
	;

expression
	: assignment_expression
		{
			$$ = $1;
		}
	| expression ',' assignment_expression
		{
			$$ = ast.MakeCommaExpression($1, $3, @$);
		}
	;

/********** Statement **********/
statement_seq
	: statement
		{
			$$ = ast.MakeList($1);
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
			$$ = ast.MakeCompoundStatement(ast.MakeList<Statement>(), @$);
		}
	| '{' statement_seq '}'
		{
			$$ = ast.MakeCompoundStatement($2, @$);
		}
	;

expression_statement
	: expression ';'
		{
			$$ = ast.MakeExpressionStatement($1, @$);
		}
	| ';'
		{
			$$ = ast.MakeEmptyStatement(@$);
		}
	;

jump_statement
	: BREAK ';'
		{
			$$ = ast.MakeBreakStatement(@$);
		}
	| RETURN ';'
		{
			$$ = ast.MakeReturnStatement(@$);
		}
	| CALL IDENTIFIER ';'
		{
			$$ = ast.MakeCallStatement($2, @$);
		}
	;

selection_statement
	: IF '(' expression ')' statement
		{
			$$ = ast.MakeIfStatement($3, $5, @$);
		}
	| IF '(' expression ')' statement ELSE statement
		{
			$$ = ast.MakeIfElseStatement($3, $5, $7, @$);
		}
	;

for_optional_expression
	:
		{
			$$ = ast.MakeForOptionalExpression(@$);
		}
	| expression
		{
			$$ = ast.MakeForExpression($1, @$);
		}
	;

iteration_statement
	: WHILE '(' expression ')' statement
		{
			$$ = ast.MakeWhileStatement($3, $5, @$);
		}
	| DO statement WHILE '(' expression ')' ';'
		{
			$$ = ast.MakeDoStatement($2, $5, @$);
		}
	| FOR '(' for_optional_expression ';' for_optional_expression ';' for_optional_expression ')' statement
		{
			$$ = ast.MakeForStatement($3, $5, $7, $9, @$);
		}
	;

labeled_statement
	: IDENTIFIER ':' statement
		{
			$$ = ast.MakeLabeledStatement($1, $3, @$);
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
			$$ = ast.MakeList<Statement>();
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
			instance->SetReturnValue($$ = ast.MakeTranslationUnit($2, $5, @$));
		}
	;
