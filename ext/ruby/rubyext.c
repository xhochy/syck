//
// rubyext.c
//
// $Author$
// $Date$
//
// Copyright (C) 2003 why the lucky stiff
//

#include "ruby.h"
#include "syck.h"
#include <sys/types.h>
#include <time.h>

static ID time_s_mkutc;

SYMID
rb_syck_mktime(str)
    char *str;
{
    VALUE time;
    char *ptr = str;
    VALUE year, mon, day, hour, min, sec;

    // Year
    ptr[4] = '\0';
    year = INT2FIX(strtol(ptr, NULL, 10));

    // Month
    ptr += 4;
    while ( !isdigit( *ptr ) ) ptr++;
    mon = INT2FIX(strtol(ptr, NULL, 10));

    // Day
    ptr += 2;
    while ( !isdigit( *ptr ) ) ptr++;
    day = INT2FIX(strtol(ptr, NULL, 10));

    // Hour
    ptr += 2;
    while ( !isdigit( *ptr ) ) ptr++;
    hour = INT2FIX(strtol(ptr, NULL, 10));

    // Minute 
    ptr += 2;
    while ( !isdigit( *ptr ) ) ptr++;
    min = INT2FIX(strtol(ptr, NULL, 10));

    // Second 
    ptr += 2;
    while ( !isdigit( *ptr ) ) ptr++;
    sec = INT2FIX(strtol(ptr, NULL, 10));

    time = rb_funcall(rb_cTime, time_s_mkutc, 6, year, mon, day, hour, min, sec );
    return time;
}

SYMID
rb_syck_parse_handler(p, n)
    SyckParser *p;
    SyckNode *n;
{
    VALUE obj;
    long i;

    switch (n->kind)
    {
        case syck_str_kind:
            if ( strcmp( n->type_id, "null" ) == 0 )
            {
                obj = Qnil;
            }
            else if ( strcmp( n->type_id, "bool#yes" ) == 0 )
            {
                obj = Qtrue;
            }
            else if ( strcmp( n->type_id, "bool#no" ) == 0 )
            {
                obj = Qfalse;
            }
            else if ( strcmp( n->type_id, "int#hex" ) == 0 )
            {
                obj = rb_cstr2inum( n->data.str->ptr, 16 );
            }
            else if ( strcmp( n->type_id, "int#oct" ) == 0 )
            {
                obj = rb_cstr2inum( n->data.str->ptr, -8 );
            }
            else if ( strcmp( n->type_id, "int" ) == 0 )
            {
                obj = rb_cstr2inum( n->data.str->ptr, 10 );
            }
            else if ( strcmp( n->type_id, "timestamp" ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr );
            }
            else if ( strcmp( n->type_id, "timestamp#iso8601" ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr );
            }
            else if ( strcmp( n->type_id, "timestamp#spaced" ) == 0 )
            {
                obj = rb_syck_mktime( n->data.str->ptr );
            }
            else if ( strcmp( n->type_id, "timestamp#ymd" ) == 0 )
            {
                S_REALLOC_N( n->data.str->ptr, char, 22 );
                strcat( n->data.str->ptr, "T12:00:00Z" );
                obj = rb_syck_mktime( n->data.str->ptr );
            }
            else
            {
                obj = rb_str_new( n->data.str->ptr, n->data.str->len );
            }
        break;

        case syck_seq_kind:
            obj = rb_ary_new2( n->data.list->idx );
            for ( i = 0; i < n->data.list->idx; i++ )
            {
                rb_ary_store( obj, i, syck_seq_read( n, i ) );
            }
        break;

        case syck_map_kind:
            obj = rb_hash_new();
            for ( i = 0; i < n->data.pairs->idx; i++ )
            {
                rb_hash_aset( obj, syck_map_read( n, map_key, i ), syck_map_read( n, map_value, i ) );
            }
        break;
    }
    return obj;
}

void
rb_syck_err_handler(p, msg)
    SyckParser *p;
    char *msg;
{
    p->cursor[1] = '\0';
    rb_raise(rb_eLoadError, "%s on line %d, col %d: `%s'",
           msg,
           p->linect,
           p->cursor - p->lineptr, 
           p->lineptr); 
}

VALUE
rb_syck_load(argc, argv)
    int argc;
    VALUE *argv;
{
    VALUE port, proc;
    VALUE v;
    //OpenFile *fptr;
    SyckParser *parser = syck_new_parser();

    rb_scan_args(argc, argv, "11", &port, &proc);
    if (rb_respond_to(port, rb_intern("to_str"))) {
	    //arg.taint = OBJ_TAINTED(port); /* original taintedness */
	    //StringValue(port);	       /* possible conversion */
	    syck_parser_str( parser, RSTRING(port)->ptr, RSTRING(port)->len, NULL );
    }

    syck_parser_handler( parser, rb_syck_parse_handler );
    syck_parser_error_handler( parser, rb_syck_err_handler );
    v = syck_parse( parser );
    syck_free_parser( parser );

    return v;
}

void
Init_syck()
{
    VALUE rb_syck = rb_define_module( "Syck" );

    time_s_mkutc = rb_intern("utc");
    rb_define_module_function(rb_syck, "load", rb_syck_load, -1);
}

