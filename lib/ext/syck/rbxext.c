#include "ruby.h"
#include "syck.h"
#include <sys/types.h>

#define SYM(str) ((VALUE)rb_intern(str))

static VALUE rbx_syck_handler(SyckParser *p, SyckNode *n) {
  VALUE type, style, val, obj;
  
  if(n->type_id == NULL) {
    type = Qnil;
  } else {
    type = ID2SYM(rb_intern(n->type_id));
  }
  
  style = Qnil;
  
  switch(n->kind) {
  case syck_str_kind:
    {
      val = rb_str_new(n->data.str->ptr, n->data.str->len);
      if ( n->data.str->style == scalar_1quote )
      {
          style = SYM("quote1");
      }
      else if ( n->data.str->style == scalar_2quote )
      {
          style = SYM("quote2");
      }
      else if ( n->data.str->style == scalar_fold )
      {
          style = SYM("fold");
      }
      else if ( n->data.str->style == scalar_literal )
      {
          style = SYM("literal");
      }
      else if ( n->data.str->style == scalar_plain )
      {
          style = SYM("plain");
      }
      obj = rb_ary_new2(4);
      rb_ary_push(obj, SYM("scalar"));
      rb_ary_push(obj, type);
      rb_ary_push(obj, style);
      rb_ary_push(obj, val);
    }
    break;
  
  case syck_seq_kind:
   {
     int i;
     obj = rb_ary_new2(syck_seq_count(n) + 3);
     rb_ary_push(obj, SYM("sequence"));
     rb_ary_push(obj, type);
     if(n->data.list->style == seq_inline) {
       rb_ary_push(obj, SYM("inline"));
     } else {
       rb_ary_push(obj, Qnil);
     }
     
     for(i = 0; i < syck_seq_count(n); i++) {
       rb_ary_push(obj, (VALUE)syck_seq_read(n, i));
     }
   }
   break;
   
   case syck_map_kind:
    {
      int i, count;
      count = syck_map_count(n);
      obj = rb_ary_new2((count * 2) + 3);
      rb_ary_push(obj, SYM("map"));
      rb_ary_push(obj, type);
      if(n->data.pairs->style == seq_inline) {
        rb_ary_push(obj, SYM("inline"));        
      } else {
        rb_ary_push(obj, Qnil);
      }
      
      for(i = 0; i < count; i++) {
        rb_ary_push(obj, (VALUE)syck_map_read(n, map_key, i));
        rb_ary_push(obj, (VALUE)syck_map_read(n, map_value, i));
      }
    }
    break;
    
    default:
    obj = Qnil;
  }
  
  return obj;
}

static enum scalar_style translate_style_scalar(VALUE style) {
  if(style == SYM("quote1")) 
  {
    return scalar_1quote; 
  }
  else if(style == SYM("quote2"))
  {
    return scalar_2quote;
  }
  else if(style == SYM("fold"))
  {
    return scalar_fold;
  }
  else if(style == SYM("literal"))
  {
    return scalar_literal;
  }
  else if(style == SYM("plain"))
  {
    return scalar_plain;
  } else {
    return scalar_none;
  }
}

static enum seq_style translate_style_seq(VALUE style) {
  if(style == SYM("inline")) 
  {
    return seq_inline; 
  }
  return seq_none;
}

static enum map_style translate_style_map(VALUE style) {
  if(style == SYM("inline")) 
  {
    return map_inline; 
  }
  return map_none;
}

static void rbx_syck_emitter(SyckEmitter *e, st_data_t data) {
  VALUE ary = (VALUE)data;
  VALUE sym, type, style;
  char *ts;
  int i, j;
  
  sym =   rb_ary_entry(ary, 0);
  type =  rb_ary_entry(ary, 1);
  style = rb_ary_entry(ary, 2);
  
  if(type == Qnil) {
    ts = NULL;
  } else if(SYMBOL_P(type)) {
    ts = rb_id2name(SYM2ID(type));
  } else {
    ts = StringValuePtr(type);
  }
  
  if(sym == SYM("sequence")) {
    syck_emit_seq(e, ts, translate_style_seq(style));
    for(j = 3; j < rb_ary_size(ary); j++) {
      syck_emit_item(e, (st_data_t)rb_ary_entry(ary, j));
    }
    syck_emit_end(e);
  } else if(sym == SYM("map")) {
    syck_emit_map(e, ts, translate_style_map(style));
    for(j = 3; j < rb_ary_size(ary); j++) {
      syck_emit_item(e, (st_data_t)rb_ary_entry(ary, j++));
      syck_emit_item(e, (st_data_t)rb_ary_entry(ary, j));
    }
    syck_emit_end(e);
  } else if(sym == SYM("scalar")) {
    char *data;
    int len;
    data = StringValuePtr(rb_ary_entry(ary, 3));
    len = strlen(data);
    syck_emit_scalar(e, ts, translate_style_scalar(style), 0, 0, 0, data, len);
  }
}

static SyckParser* new_parser() {
  SyckParser *parser = syck_new_parser();
  syck_parser_set_root_on_error( parser, Qnil );
  syck_parser_handler( parser, rbx_syck_handler );
  syck_parser_implicit_typing( parser, 1 );
  syck_parser_taguri_expansion( parser, 1 );
  syck_parser_set_input_type( parser, syck_yaml_utf8 );
  return parser;
}

static VALUE syck_parse_file(VALUE self, VALUE file_name) {
  FILE *fp;
  VALUE obj;
  SyckParser *parser;
  
  fp = fopen(StringValuePtr(file_name), "r");
  if(!fp) return Qnil;
  
  parser = new_parser();
  
  // syck_parser_error_handler( parser, rbx_syck_error_handler );
  // syck_parser_bad_anchor_handler( parser, rbx_syck_bad_anchor_handler );
  
  syck_parser_file( parser, fp, NULL );
  
  obj = (VALUE)syck_parse( parser );
  syck_free_parser(parser);
  fclose(fp);
  return obj;
}

static VALUE syck_parse_string(VALUE self, VALUE str) {
  FILE *fp;
  VALUE obj;
  SyckParser *parser;
  char *data;
  int len;
    
  data = strdup(StringValuePtr(str));
  
  parser = new_parser();
  len = strlen(data);
  
  // syck_parser_error_handler( parser, rbx_syck_error_handler );
  // syck_parser_bad_anchor_handler( parser, rbx_syck_bad_anchor_handler );
  
  syck_parser_str( parser, data, len, NULL );
  
  obj = (VALUE)syck_parse( parser );
  syck_free_parser(parser);
  
  free(data);
  return obj;
}

struct emitter_extra {
  VALUE object;
};

static void rbx_syck_emitter_output(SyckEmitter *emitter, char *str, int len) {
  struct emitter_extra *bonus = (struct emitter_extra *)emitter->bonus;
  bonus->object = rb_str_new(str, len);
}

static VALUE syck_emit_string(VALUE self, VALUE ary) {
  SyckEmitter *emitter = syck_new_emitter();
  struct emitter_extra xtra;
  emitter->bonus = &xtra;
  xtra.object = Qnil;
  syck_emitter_handler( emitter, rbx_syck_emitter );
  syck_output_handler( emitter, rbx_syck_emitter_output );    
  syck_emitter_mark_node(emitter, (st_data_t)ary);
  syck_emit(emitter, (st_data_t)ary);
  syck_emitter_flush(emitter, 0);
  syck_free_emitter(emitter);
  return xtra.object;
}

void Init_rbxext() {
  VALUE mod;
  
  mod = rb_define_module("YAML");
  rb_define_singleton_method(mod, "parse_file", syck_parse_file, 1);
  rb_define_singleton_method(mod, "parse_string", syck_parse_string, 1);
  rb_define_singleton_method(mod, "emit_yaml", syck_emit_string, 1);
}

