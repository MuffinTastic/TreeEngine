grammar DEF;

// Parser rules

def
    :   interopDefinition*
;

interopDefinition
    :   (   structDefinition
        |   systemDefinition
        )   
;

structDefinition
    :   struct identifier '{' variable* '}'
;

systemDefinition
    :   (native | managed) system identifier '{' function* '}'
;

struct  :   'struct'    ;
system  :   'system'    ;
native  :   'native'    ;
managed :   'managed'   ;

variable
    :   typeSpecifier identifier endStatement
;

function
    :   typeSpecifier identifier '(' funcArgumentList? ')' endStatement
;

funcArgumentList
    :   funcArgument (',' funcArgument)*
;

funcArgument
    :   typeSpecifier identifier
;

typeSpecifier
    :   baseType
    |   baseType pointer
    |   baseType '[' arraySize ']'
;

baseType
    :   (   void
        |   bool
        |   char
        |   i8
        |   u8
        |   i16
        |   u16
        |   i32
        |   u32
        |   f32
        |   f64
        |   string
        |   enum
        )
    |   identifier
;

arraySize
    :   (   integerLiteral
        |   identifier
        )
;

identifier
    :   IDENTIFIER
;

integerLiteral
    :   INTEGERLITERAL
;

void    :   'void'  ;
bool    :   'bool'  ;
char    :   'char'  ;
i8      :   'i8'    ;
u8      :   'u8'    ;
i16     :   'i16'   ;
u16     :   'u16'   ;
i32     :   'i32'   ;
u32     :   'u32'   ;
f32     :   'f32'   ;
f64     :   'f64'   ;
string  :   'string';
enum    :   'enum'  ;
pointer :   '*'     ;

endStatement : ';'  ;

// Lexer rules

IDENTIFIER
    :   NONDIGIT
        (   NONDIGIT
        |   DIGIT
        )*
    ;

WHITESPACE
    :   [ \t]+
        -> skip
    ;

NEWLINE
    :   (   '\r' '\n'?
        |   '\n'
        )
        -> skip
    ;

BLOCKCOMMENT
    :   '/*' .*? '*/'
        -> skip
    ;

LINECOMMENT
    :   '//' ~[\r\n]*
        -> skip
    ;

INTEGERLITERAL
    :   (   DECIMALLITERAL
        | OCTALLITERAL
        | HEXLITERAL
        | BINARYLITERAL
        )
    ;

DECIMALLITERAL
    : NONZERODIGIT ('\''? DIGIT)*
    ;

OCTALLITERAL
    : '0' ('\''? OCTALDIGIT)*
    ;

HEXLITERAL
    : ('0x' | '0X') HEXADECIMALDIGIT ( '\''? HEXADECIMALDIGIT)*
    ;

BINARYLITERAL
    : ('0b' | '0B') BINARYDIGIT ('\''? BINARYDIGIT)*
    ;

fragment
OCTALDIGIT
    : [0-7]
    ;

fragment
HEXADECIMALDIGIT
    : [0-9a-fA-F]
    ;

fragment
BINARYDIGIT
    : [01]
    ;

fragment
NONDIGIT
    :   [a-zA-Z_]
    ;

fragment
NONZERODIGIT
    :   [1-9]
    ;

fragment
DIGIT
    :   [0-9]
    ;