//
// implicit.re
//
// $Author$
// $Date$
//
// Copyright (C) 2003 why the lucky stiff
//

#include "syck.h"

#define YYCTYPE     char
#define YYCURSOR    cursor
#define YYMARKER    marker
#define YYLIMIT     limit
#define YYFILL(n)

#define TAG_IMPLICIT( tid )     n->type_id = tid; return;

void
try_tag_implicit( SyckNode *n )
{
    char *cursor, *limit, *marker;
    if ( n->kind != syck_str_kind )
        return;

    cursor = n->data.str->ptr;
    limit = cursor + n->data.str->len;

/*!re2c

WORDC = [A-Za-z0-9_-\.]+ ;
ENDSPC = [ \n]+ ;
NULL = [\000] ;
ANY = [\001-\377] ;
DIGITS = [0-9] ;
SIGN = [-+] ;
HEX = [0-9a-fA-F,] ;
OCT = [0-7,] ;
INTHEX = SIGN? "0x" HEX+ ; 
INTOCT = SIGN? "0" OCT+ ;
INTCANON = SIGN? DIGITS ( DIGITS | "," )* ;
NULLTYPE = ( "~" | "null" | "Null" | "NULL" ) ;
BOOLYES = ( "true" | "True" | "TRUE" | "yes" | "Yes" | "YES" | "on" | "On" | "ON" ) ;
BOOLNO = ( "false" | "False" | "FALSE" | "no" | "No" | "NO" | "off" | "Off" | "OFF" ) ;

NULLTYPE NULL       {   TAG_IMPLICIT( "null" ); }

BOOLYES NULL        {   TAG_IMPLICIT( "bool#yes" ); }

BOOLNO NULL         {   TAG_IMPLICIT( "bool#no" ); }

INTHEX NULL         {   TAG_IMPLICIT( "int#hex" ); }

INTOCT NULL         {   TAG_IMPLICIT( "int#oct" ); }

INTCANON NULL       {   TAG_IMPLICIT( "int" ); }

ANY                 {   TAG_IMPLICIT( "str" ); }

*/

}
