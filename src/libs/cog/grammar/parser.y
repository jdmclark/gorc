%defines
%locations
%pure-parser
%name-prefix "gra_"
%error-verbose
%verbose
%lex-param { void *scanner }
%parse-param { grammar *instance }

%expect 1

%code requires
{
    #pragma GCC diagnostic ignored "-Wconversion"

    #include "log/log.hpp"
    using namespace gorc;
    using namespace gorc::cog;

    #define YYLTYPE diagnostic_context_location

    #define YYLLOC_DEFAULT(Cur, Rhs, N)                                                 \
    do {                                                                                \
        if(N) {                                                                         \
            (Cur) = diagnostic_context_location(nullptr,                                \
                                                YYRHSLOC(Rhs, 1).first_line,            \
                                                YYRHSLOC(Rhs, 1).first_col,             \
                                                YYRHSLOC(Rhs, N).last_line,             \
                                                YYRHSLOC(Rhs, N).last_col);             \
        }                                                                               \
        else {                                                                          \
            (Cur) = diagnostic_context_location(nullptr,                                \
                                                YYRHSLOC(Rhs, 0).last_line,             \
                                                YYRHSLOC(Rhs, 0).last_col,              \
                                                YYRHSLOC(Rhs, 0).last_line,             \
                                                YYRHSLOC(Rhs, 0).last_col);             \
        }                                                                               \
    } while(0)
}

%code top
{
    #include "grammar.hpp"
    #include "log/log.hpp"

    using namespace gorc;
    using namespace gorc::cog;
    using namespace gorc::cog::ast;
}

%union
{
    char *string;
    int integer;
    float floating;

    ast::identifier *identifier;

    ast::symbol_field *symbol_field;
    ast::symbol_extension *symbol_extension;
    ast::list_node<ast::symbol_extension*> *symbol_extension_list;
    ast::symbol *symbol;
    ast::list_node<ast::symbol*> *symbol_list;

    ast::unary_operator unary_operator;
    ast::infix_operator infix_operator;
    ast::expression *expression;
    ast::list_node<ast::expression*> *expression_list;

    ast::statement *statement;
    ast::list_node<ast::statement*> *statement_list;
    ast::for_optional_expression *for_optional_expression;

    ast::translation_unit *translation_unit;
}

/* Destructors */
%destructor { free($$); } <string>

/* Main keyword tokens */
%token FLAGS SYMBOLS CODE END

/* Symbols keyword tokens */
%token ENDLINE

/* Code keyword tokens */
%token BREAK CALL DO ELSE FOR IF RETURN WHILE

/* Code operator tokens */
%token EQ_OP NE_OP GE_OP LE_OP LOGICAL_AND_OP LOGICAL_OR_OP

/* Literals */
%token <string> RAW_IDENTIFIER STRING_FRAGMENT STRING_LITERAL
%token <integer> INTEGER_LITERAL
%token <floating> FLOAT_LITERAL

%type <identifier> identifier

/* Symbols */
%type <symbol_field> symbol_field
%type <symbol_extension> symbol_extension
%type <symbol_extension_list> symbol_extension_seq optional_symbol_extension_seq
%type <symbol> inner_symbol symbol
%type <symbol_list> symbol_seq optional_symbol_seq symbol_section

/* Expressions */
%type <floating> vector_component_scalar vector_component
%type <unary_operator> unary_operator
%type <infix_operator> multiplicative_operator additive_operator relational_operator
%type <infix_operator> equality_operator
%type <expression> literal_expression primary_expression postfix_expression unary_expression
%type <expression> multiplicative_expression additive_expression relational_expression
%type <expression> equality_expression and_expression exclusive_or_expression
%type <expression> inclusive_or_expression logical_and_expression logical_or_expression
%type <expression> assignment_expression expression vector_literal_expression
%type <expression_list> argument_expression_list optional_argument_expression_list

/* Statements */
%type <statement> compound_statement expression_statement jump_statement selection_statement
%type <statement> iteration_statement labeled_statement statement
%type <statement_list> statement_seq optional_statement_seq code_section
%type <for_optional_expression> for_optional_expression

/* Translation unit */
%type <integer> flags_section
%type <translation_unit> translation_unit

%start translation_unit

%{
    using namespace gorc;
    using namespace gorc::cog;

    int yylex(YYSTYPE *lvalp, diagnostic_context_location *llocp, void *scanner);
    int yyparse(grammar *instance);

    void yyerror(diagnostic_context_location *llocp, grammar*, char const *err)
    {
        diagnostic_context dc(*llocp);
        LOG_ERROR(err);
    }

    gorc::cog::ast::translation_unit* gorc::cog::grammar::parse()
    {
        yyparse(this);
        return tu;
    }

    #define scanner instance->get_scanner()
    #define ast instance->factory
%}

%%

identifier
    : RAW_IDENTIFIER {
            $$ = ast.make<identifier>(@$, $1);
        }
    ;

/********** Symbols **********/
symbol_field
    : STRING_FRAGMENT {
            $$ = ast.make_var<symbol_field, string_fragment_field>(@$, $1);
        }
    | INTEGER_LITERAL {
            $$ = ast.make_var<symbol_field, integer_field>(@$, $1);
        }
    | FLOAT_LITERAL {
            $$ = ast.make_var<symbol_field, float_field>(@$, $1);
        }
    ;

symbol_extension
    : identifier {
            $$ = ast.make_var<symbol_extension, bare_extension>(@$, $1);
        }
    | identifier '=' symbol_field {
            $$ = ast.make_var<symbol_extension, valued_extension>(@$, $1, $3);
        }
    ;

symbol_extension_seq
    : symbol_extension {
            $$ = ast.make<list_node<symbol_extension*>>(@$, $1);
        }
    | symbol_extension_seq symbol_extension {
            ($1)->elements.push_back($2);
            $$ = $1;
        }
    ;

optional_symbol_extension_seq
    : /* Blank */ {
            $$ = ast.make<list_node<symbol_extension*>>(@$);
        }
    | symbol_extension_seq
    ;

inner_symbol
    : identifier identifier optional_symbol_extension_seq {
            $$ = ast.make<symbol>(@$, $1, $2, $3);
        }
    | identifier identifier '=' symbol_field optional_symbol_extension_seq {
            $$ = ast.make<symbol>(@$, $1, $2, $5, $4);
        }
    ;

symbol
    : inner_symbol ENDLINE
    | ENDLINE {
            $$ = nullptr;
        }
    ;

symbol_seq
    : symbol {
            if($1 == nullptr) {
                $$ = ast.make<list_node<symbol*>>(@$);
            }
            else {
                $$ = ast.make<list_node<symbol*>>(@$, $1);
            }
        }
    | symbol_seq symbol {
            if($2 != nullptr) {
                ($1)->elements.push_back($2);
            }

            $$ = $1;
        }
    ;

optional_symbol_seq
    : /* Blank */ {
            $$ = ast.make<list_node<symbol*>>(@$);
        }
    | symbol_seq
    ;

symbol_section
    : SYMBOLS optional_symbol_seq END {
            $$ = $2;
        }
    ;

/********** Expressions **********/
argument_expression_list
    : assignment_expression {
            $$ = ast.make<list_node<expression*>>(@$, $1);
        }
    | argument_expression_list ',' assignment_expression {
            ($1)->elements.push_back($3);
            $$ = $1;
        }
    ;

optional_argument_expression_list
    : /* Blank */ {
            $$ = ast.make<list_node<expression*>>(@$);
        }
    | argument_expression_list
    ;

vector_component_scalar
    : INTEGER_LITERAL {
            $$ = static_cast<float>($1);
        }
    | FLOAT_LITERAL
    ;

vector_component
    : vector_component_scalar
    | '+' vector_component_scalar {
            $$ = $2;
        }
    | '-' vector_component_scalar {
            $$ = (-$2);
        }
    ;

vector_literal_expression
    : '\'' vector_component vector_component vector_component '\'' {
            $$ = ast.make_var<expression, vector_literal_expression>(@$, $2, $3, $4);
        }
    | '\'' vector_component ',' vector_component ',' vector_component '\'' {
            $$ = ast.make_var<expression, vector_literal_expression>(@$, $2, $4, $6);
        }
    ;

literal_expression
    : STRING_LITERAL {
            $$ = ast.make_var<expression, string_literal_expression>(@$, $1);
        }
    | INTEGER_LITERAL {
            $$ = ast.make_var<expression, integer_literal_expression>(@$, $1);
        }
    | FLOAT_LITERAL {
            $$ = ast.make_var<expression, float_literal_expression>(@$, $1);
        }
    | vector_literal_expression
    ;

primary_expression
    : literal_expression
    | '(' expression ')' {
            $$ = $2;
        }
    | identifier {
            $$ = ast.make_var<expression, identifier_expression>(@$, $1);
        }
    ;

postfix_expression
    : primary_expression
    | identifier '[' expression ']' {
            $$ = ast.make_var<expression, subscript_expression>(@$, $1, $3);
        }
    | identifier '(' optional_argument_expression_list ')' {
            $$ = ast.make_var<expression, method_call_expression>(@$, $1, $3);
        }
    ;

unary_operator
    : '+' { $$ = unary_operator::plus; }
    | '-' { $$ = unary_operator::minus; }
    | '!' { $$ = unary_operator::logical_not; }
    ;

unary_expression
    : postfix_expression
    | unary_operator unary_expression {
            $$ = ast.make_var<expression, unary_expression>(@$, $2, $1);
        }
    ;

multiplicative_operator
    : '*' { $$ = infix_operator::multiplication; }
    | '/' { $$ = infix_operator::division; }
    | '%' { $$ = infix_operator::modulo; }
    ;

multiplicative_expression
    : unary_expression
    | multiplicative_expression multiplicative_operator unary_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, $2);
        }
    ;

additive_operator
    : '+' { $$ = infix_operator::addition; }
    | '-' { $$ = infix_operator::subtraction; }
    ;

additive_expression
    : multiplicative_expression
    | additive_expression additive_operator multiplicative_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, $2);
        }
    ;

relational_operator
    : GE_OP { $$ = infix_operator::greater_equal; }
    | LE_OP { $$ = infix_operator::less_equal; }
    | '>' { $$ = infix_operator::greater; }
    | '<' { $$ = infix_operator::less; }
    ;

relational_expression
    : additive_expression
    | relational_expression relational_operator additive_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, $2);
        }
    ;

equality_operator
    : EQ_OP { $$ = infix_operator::equal; }
    | NE_OP { $$ = infix_operator::not_equal; }
    ;

equality_expression
    : relational_expression
    | equality_expression equality_operator relational_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, $2);
        }
    ;

and_expression
    : equality_expression
    | and_expression '&' equality_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, infix_operator::bitwise_and);
        }
    ;

exclusive_or_expression
    : and_expression
    | exclusive_or_expression '^' and_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, infix_operator::bitwise_xor);
        }
    ;

inclusive_or_expression
    : exclusive_or_expression
    | inclusive_or_expression '|' exclusive_or_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, infix_operator::bitwise_or);
        }
    ;

logical_and_expression
    : inclusive_or_expression
    | logical_and_expression LOGICAL_AND_OP inclusive_or_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, infix_operator::logical_and);
        }
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression LOGICAL_OR_OP logical_and_expression {
            $$ = ast.make_var<expression, infix_expression>(@$, $1, $3, infix_operator::logical_or);
        }
    ;

assignment_expression
    : logical_or_expression
    | unary_expression '=' assignment_expression {
            $$ = ast.make_var<expression, assignment_expression>(@$, $1, $3);
        }
    ;

expression
    : assignment_expression
    | expression ',' assignment_expression {
            $$ = ast.make_var<expression, comma_expression>(@$, $1, $3);
        }
    ;

/********** Statements **********/
statement_seq
    : statement {
            $$ = ast.make<list_node<statement*>>(@$, $1);
        }
    | statement_seq statement {
            ($1)->elements.push_back($2);
            $$ = $1;
        }
    ;

optional_statement_seq
    : /* Blank */ {
            $$ = ast.make<list_node<statement*>>(@$);
        }
    | statement_seq
    ;

compound_statement
    : '{' optional_statement_seq '}' {
            $$ = ast.make_var<statement, compound_statement>(@$, $2);
        }
    ;

expression_statement
    : expression ';' {
            $$ = ast.make_var<statement, expression_statement>(@$, $1);
        }
    | ';' {
            $$ = ast.make_var<statement, empty_statement>(@$);
        }
    ;

jump_statement
    : BREAK ';' {
            $$ = ast.make_var<statement, break_statement>(@$);
        }
    | RETURN ';' {
            $$ = ast.make_var<statement, return_statement>(@$);
        }
    | CALL identifier ';' {
            $$ = ast.make_var<statement, call_statement>(@$, $2);
        }
    ;

selection_statement
    : IF '(' expression ')' statement {
            $$ = ast.make_var<statement, if_statement>(@$, $3, $5);
        }
    | IF '(' expression ')' statement ELSE statement {
            $$ = ast.make_var<statement, if_else_statement>(@$, $3, $5, $7);
        }
    ;

for_optional_expression
    : /* Blank */ {
            $$ = ast.make_var<for_optional_expression, for_empty_expression>(@$);
        }
    | expression {
            $$ = ast.make_var<for_optional_expression, for_expression>(@$, $1);
        }
    ;

iteration_statement
    : WHILE '(' expression ')' statement {
            $$ = ast.make_var<statement, while_statement>(@$, $3, $5);
        }
    | DO statement WHILE '(' expression ')' ';' {
            $$ = ast.make_var<statement, do_statement>(@$, $2, $5);
        }
    | FOR '(' for_optional_expression ';'
              for_optional_expression ';'
              for_optional_expression ')' statement {
            $$ = ast.make_var<statement, for_statement>(@$, $3, $5, $7, $9);
        }
    ;

labeled_statement
    : identifier ':' statement {
            $$ = ast.make_var<statement, labeled_statement>(@$, $1, $3);
        }
    ;

statement
    : expression_statement
    | jump_statement
    | selection_statement
    | iteration_statement
    | compound_statement
    | labeled_statement
    ;

code_section
    : CODE optional_statement_seq END {
            $$ = $2;
        }
    ;

/********** Translation Unit **********/

flags_section
    : /* Blank */ {
            $$ = 0;
        }
    | FLAGS '=' INTEGER_LITERAL {
            $$ = $3;
        }
    ;

translation_unit
    : flags_section symbol_section code_section {
            instance->set_return_value($$ = ast.make<translation_unit>(@$, $1, $2, $3));
        }
    ;
