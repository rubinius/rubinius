#include "oniguruma.h" // Must be first.
#include "transcoder.h"
#include "regenc.h"

#include "builtin/array.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/integer.hpp"
#include "builtin/io.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "on_stack.hpp"
#include "ontology.hpp"
#include "version.h"

#include <ctype.h>
#include <string.h>

#ifdef HAVE_NL_LANGINFO
#include <locale.h>
#include <langinfo.h>
#endif

#undef  ENC_DEFINE
#undef  ENC_REPLICATE
#undef  ENC_ALIAS
#undef  ENC_DUMMY

#define ENC_DEFINE(name, data)      define(state, name, ONIG_ENCODING_##data);
#define ENC_REPLICATE(name, orig)   replicate(state, name, orig);
#define ENC_ALIAS(name, orig)       alias(state, name, orig);
#define ENC_DUMMY(name)             define_dummy(state, name);

#define TRANS_DECLARE(from, to, lib)  declare(state, from, to, lib);

namespace rubinius {
  void Encoding::init(STATE) {
    onig_init();  // in regexp.cpp too, but idempotent.

    if(LANGUAGE_18_ENABLED) {
      Class* ns = ontology::new_class_under(state, "EncodingClass", G(rubinius));
      GO(encoding).set(ontology::new_class_under(state, "Encoding", ns));
    } else {
      GO(encoding).set(ontology::new_class(state, "Encoding"));
      G(rubinius)->set_const(state, "EncodingClass", G(encoding));
    }

    G(encoding)->set_object_type(state, EncodingType);

    G(encoding)->set_const(state, "EncodingMap", LookupTable::create(state));
    G(encoding)->set_const(state, "EncodingList", Array::create(state, 3));

    G(encoding)->set_ivar(state, state->symbol("@default_external"), G(undefined));
    G(encoding)->set_ivar(state, state->symbol("@default_internal"), G(undefined));

    Encoding* binary = create_bootstrap(state, "ASCII-8BIT", eBinary, ONIG_ENCODING_ASCII);
    Encoding* utf8 = create_bootstrap(state, "UTF-8", eUtf8, ONIG_ENCODING_UTF_8);
    Encoding* ascii = create_bootstrap(state, "US-ASCII", eAscii, ONIG_ENCODING_US_ASCII);

    ascii->name(state, String::create(state, "US-ASCII"));
    binary->name(state, String::create(state, "ASCII-8BIT"));
    utf8->name(state, String::create(state, "UTF-8"));

    GO(usascii_encoding).set(ascii);
    GO(ascii8bit_encoding).set(binary);
    GO(utf8_encoding).set(utf8);

#include "gen/encoding_database.cpp"

    char* locale_charmap;

#ifdef HAVE_NL_LANGINFO
    setlocale(LC_CTYPE, "");
    locale_charmap = nl_langinfo(CODESET);
#else
    locale_charmap = "US-ASCII";
#endif

    G(encoding)->set_const(state, "LocaleCharmap", String::create(state, locale_charmap));

    int index = find_index(state, locale_charmap);
    if(index < 0) index = find_index(state, "US-ASCII");

    create_internal(state, "locale", index);
    create_internal(state, "external", index);
    create_internal(state, "filesystem", index);
    create_internal(state, "internal", -1);

    Transcoding::init(state);
    Converter::init(state);
  }

  static Symbol* encoding_symbol(STATE, const char* name) {
    char* upper = strdup(name);
    if(!upper) return nil<Symbol>();

    for(char *p = upper; *p; p++) {
      *p = toupper((int)*p);
    }

    Symbol* sym = state->symbol(upper);
    free(upper);

    return sym;
  }

  static Tuple* encoding_reference(STATE, int index, const char* alias_name = 0) {
    Tuple* pair = Tuple::create(state, 2);
    if(!alias_name) {
      pair->put(state, 0, cNil);
    } else {
      pair->put(state, 0, String::create(state, alias_name));
    }

    if(index < 0) {
      pair->put(state, 1, cNil);
    } else {
      pair->put(state, 1, Fixnum::from(index));
    }

    return pair;
  }

  Encoding* Encoding::create(STATE, OnigEncodingType* enc, Object* dummy) {
    Encoding* e = state->new_object<Encoding>(G(encoding));

    e->dummy(state, dummy);
    e->encoding_ = enc;
    e->cache_index_ = cCachedOnigDatas - 1;
    e->managed_ = false;

    return e;
  }

  Encoding* Encoding::create_bootstrap(STATE, const char* name,
                                       Index index, OnigEncodingType* enc)
  {
    Encoding* e = create(state, enc);
    e->cache_index_ = (int) index;

    Tuple* ref = encoding_reference(state, index);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    encoding_list(state)->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  void Encoding::create_internal(STATE, const char* name, int index) {
    Tuple* ref = encoding_reference(state, index, name);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
  }

  Encoding* Encoding::define(STATE, const char* name,
                             OnigEncodingType* enc, Object* dummy)
  {
    Encoding* e = create(state, enc, dummy);

    e->name(state, String::create(state, name));

    Array* list = encoding_list(state);
    size_t index = list->size();

    Tuple* ref = encoding_reference(state, index);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    list->set(state, index, e);
    add_constant(state, name, e);

    return e;
  }

  Encoding* Encoding::define_dummy(STATE, const char* name) {
    return define(state, name, ONIG_ENCODING_ASCII, cTrue);
  }

  Encoding* Encoding::replicate(STATE, const char* name, Encoding* enc) {
    Encoding* repl = define(state, name, enc->get_encoding(), enc->dummy());
    repl->make_managed(state, name, enc->get_encoding());

    return repl;
  }

  Encoding* Encoding::replicate(STATE, const char* name, const char* original) {
    Encoding* enc = find(state, original);
    if(enc->nil_p()) return nil<Encoding>();

    return replicate(state, name, enc);
  }

  Encoding* Encoding::alias(STATE, const char* name, const char* original) {
    int index = find_index(state, original);
    if(index < 0) return nil<Encoding>();

    Tuple* ref = encoding_reference(state, index, name);
    encoding_map(state)->store(state, encoding_symbol(state, name), ref);
    add_constant(state, name, from_index(state, index));

    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  Encoding* Encoding::default_external(STATE) {
    Encoding* enc;
    Symbol* default_external = state->symbol("default_external");
    Object* obj = G(encoding)->get_ivar(state, default_external);

    if(!(enc = try_as<Encoding>(obj))) {
      enc = as<Encoding>(Encoding::find(state, "external"));
      G(encoding)->set_ivar(state, default_external, enc);
    }

    return enc;
  }

  Encoding* Encoding::default_internal(STATE) {
    Encoding* enc;
    Symbol* default_internal = state->symbol("default_internal");
    Object* obj = G(encoding)->get_ivar(state, default_internal);

    if(!(enc = try_as<Encoding>(obj))) {
      enc = Encoding::find(state, "internal");
      G(encoding)->set_ivar(state, default_internal, enc);
    }

    return enc;
  }

  Encoding* Encoding::get_object_encoding(STATE, Object* obj) {
    if(String* str = try_as<String>(obj)) {
      return str->encoding(state);
    } else if(Symbol* sym = try_as<Symbol>(obj)) {
      return sym->encoding(state);
    } else if(Regexp* reg = try_as<Regexp>(obj)) {
      return reg->encoding(state);
    } else if(Encoding* enc = try_as<Encoding>(obj)) {
      return enc;
    } else if(try_as<IO>(obj)) {
      // TODO
      return nil<Encoding>();
    } else {
      // MRI permits associating an Encoding with anything.
      Object* e = obj->get_ivar(state, state->symbol("__encoding__"));
      if(Encoding* enc = try_as<Encoding>(e)) return enc;
    }

    return nil<Encoding>();
  }

  void Encoding::set_object_encoding(STATE, Object* obj, Encoding* enc) {
    if(String* str = try_as<String>(obj)) {
      str->encoding(state, enc);
    } else if(Regexp* reg = try_as<Regexp>(obj)) {
      reg->encoding(state, enc);
    } else if(Symbol* sym = try_as<Symbol>(obj)) {
      sym->encoding(state, enc);
    } else if(try_as<IO>(obj)) {
      // TODO
    } else {
      // MRI permits associating an Encoding with anything.
      obj->set_ivar(state, state->symbol("__encoding__"), enc);
    }
  }

  Encoding* Encoding::compatible_p(STATE, Object* a, Object* b) {
    Encoding* enc_a = get_object_encoding(state, a);
    if(enc_a->nil_p()) return nil<Encoding>();

    Encoding* enc_b = get_object_encoding(state, b);
    if(enc_b->nil_p()) return nil<Encoding>();

    if(enc_a == enc_b) return enc_a;

    // XXX This is an ugly hack for the 1.8 mode to avoid unwanted encoding
    // errors.
    //
    // String literals are encoded as US-ASCII in .rbc files (See,
    // Rubinius::CompiledFile::Marshal#marshal in lib/compiler/compiled_file.rb)
    //
    // Normally, the 1.8 mode operates on ASCII-8BIT strings. However, US-ASCII
    // string can come from rbc files. So, ASCII-8BIT binary string and
    // US-ASCII binary string are sometimes checked for compatibility.
    //
    // Finally, the normal logic would return nil and String::append fails.
    // This shouldn't happen.
    //
    // In the ideal world, everything should be ASCII-8BIT in the 1.8 mode...
    if(LANGUAGE_18_ENABLED) {
      return Encoding::ascii8bit_encoding(state);
    }

    String* str_a = try_as<String>(a);
    String* str_b = try_as<String>(b);

    if(str_b && str_b->byte_size() == 0) return enc_a;
    if(str_a && str_a->byte_size() == 0) {
      if(CBOOL(enc_a->ascii_compatible_p(state))
          && str_b && CBOOL(str_b->ascii_only_p(state))) {
        return enc_a;
      } else {
        return enc_b;
      }
    }

    if(!CBOOL(enc_a->ascii_compatible_p(state))
        || !CBOOL(enc_b->ascii_compatible_p(state))) {
      return nil<Encoding>();
    }

    Encoding* ascii = Encoding::usascii_encoding(state);

    if(!str_b && enc_b == ascii) return enc_a;
    if(!str_a && enc_a == ascii) return enc_b;

    if(!str_a && !str_b) return nil<Encoding>();

    if(str_a && str_b) {
      bool ascii_a = CBOOL(str_a->ascii_only_p(state));
      bool ascii_b = CBOOL(str_b->ascii_only_p(state));

      if(ascii_a != ascii_b) {
        if(ascii_a) return enc_b;
        if(ascii_b) return enc_a;
      }

      if(ascii_b) return enc_a;
      if(ascii_a) return enc_b;
    } else if(!str_b) {
      if(str_a && CBOOL(str_a->ascii_only_p(state))) return enc_b;
    }

    return nil<Encoding>();
  }

#define ENCODING_NAMELEN_MAX 63

  void Encoding::add_constant(STATE, const char* name, Encoding* enc) {
    if(ISDIGIT(*name)) return;

    char* s = const_cast<char*>(name);
    bool has_lower = false;
    bool has_upper = false;
    bool valid = false;

    if(ISUPPER(*s)) {
      has_upper = true;
      while(*++s && (ISALNUM(*s) || *s == '_')) {
        if(ISLOWER(*s)) has_lower = true;
      }
    }

    if(!*s) {
      if(s - name > ENCODING_NAMELEN_MAX) return;

      valid = true;
      G(encoding)->set_const(state, state->symbol(name), enc);
    }

    if(!valid || has_lower) {
      size_t len = s - name;
      if(len > ENCODING_NAMELEN_MAX) return;

      if(!has_lower || !has_upper) {
        do {
          if(ISLOWER(*s)) has_lower = true;
          if(ISUPPER(*s)) has_upper = true;
        } while(*++s && (!has_lower || !has_upper));
        len = s - name;
      }

      len += strlen(s);
      if(len++ > ENCODING_NAMELEN_MAX) return;

      char* p = s = strdup(name);
      if(!s) return;
      name = s;

      if(!valid) {
        if(ISLOWER(*s)) *s = ONIGENC_ASCII_CODE_TO_UPPER_CASE((int)*s);
        for(; *s; ++s) {
          if(!ISALNUM(*s)) *s = '_';
        }
        if(has_upper) {
          G(encoding)->set_const(state, state->symbol(name), enc);
        }
      }

      if(has_lower) {
        for(s = (char *)name; *s; ++s) {
          if(ISLOWER(*s)) *s = ONIGENC_ASCII_CODE_TO_UPPER_CASE((int)*s);
        }
        G(encoding)->set_const(state, state->symbol(name), enc);
      }

      free(p);
    }

  }

  Class* Encoding::transcoding_class(STATE) {
    return as<Class>(G(encoding)->get_const(state, state->symbol("Transcoding")));
  }

  Class* Encoding::converter_class(STATE) {
    return as<Class>(G(encoding)->get_const(state, state->symbol("Converter")));
  }

  LookupTable* Encoding::encoding_map(STATE) {
    return as<LookupTable>(G(encoding)->get_const(state, state->symbol("EncodingMap")));
  }

  Array* Encoding::encoding_list(STATE) {
    return as<Array>(G(encoding)->get_const(state, state->symbol("EncodingList")));
  }

  LookupTable* Encoding::transcoding_map(STATE) {
    return as<LookupTable>(G(encoding)->get_const(state, state->symbol("TranscodingMap")));
  }

  Encoding* Encoding::from_index(STATE, int index) {
    if(Encoding* enc = try_as<Encoding>(encoding_list(state)->get(state, index))) {
      return enc;
    } else {
      return nil<Encoding>();
    }
  }

  int Encoding::find_index(STATE, const char* name) {
    Object* obj = encoding_map(state)->fetch(state, encoding_symbol(state, name));

    if(Tuple* ref = try_as<Tuple>(obj)) {
      Fixnum* index = try_as<Fixnum>(ref->at(1));
      if(index) {
        return index->to_native();
      } else {
        return -1;
      }
    } else {
      return -1;
    }
  }

  Encoding* Encoding::find(STATE, const char* name) {
    int index = find_index(state, name);
    if(index < 0) return nil<Encoding>();
    return as<Encoding>(encoding_list(state)->get(state, index));
  }

  // Encoding#replicate primitive
  Encoding* Encoding::replicate(STATE, String* name) {
    return Encoding::replicate(state, name->c_str(state), this);
  }

  bool Encoding::ascii_compatible() {
    return encoding_->min_enc_len == 1 && dummy_->false_p();
  }

  Object* Encoding::ascii_compatible_p(STATE) {
    return RBOOL(ascii_compatible());
  }

  void Encoding::make_managed(STATE, const char* name, OnigEncodingType* enc) {
    ByteArray* enc_ba = ByteArray::create(state, sizeof(OnigEncodingType));
    memcpy(enc_ba->raw_bytes(), enc, sizeof(OnigEncodingType));

    encoding_ = reinterpret_cast<OnigEncodingType*>(enc_ba->raw_bytes());
    write_barrier(state, enc_ba);

    int size = strlen(name);
    if(size >= ENCODING_NAMELEN_MAX) size = ENCODING_NAMELEN_MAX-1;

    ByteArray* name_ba = ByteArray::create(state, size + 1);
    memcpy(name_ba->raw_bytes(), name, size);
    name_ba->raw_bytes()[size] = 0;
    encoding_->name = reinterpret_cast<const char*>(name_ba->raw_bytes());
    write_barrier(state, name_ba);

    managed_ = true;
  }

  native_int Encoding::find_non_ascii_index(const uint8_t* start, const uint8_t* end) {
    uint8_t* p = (uint8_t*) start;
    while(p < end) {
      if(!ISASCII(*p)) {
        return p - start;
      }
      ++p;
    }
    return -1;
  }

  native_int Encoding::find_byte_character_index(const uint8_t* start, const uint8_t* end, native_int index, OnigEncodingType* enc) {
    uint8_t* p = (uint8_t*) start;
    native_int char_index = 0;

    while(p < end && index > 0) {
      native_int char_len = mbclen(p, end, enc);
      p += char_len;
      index -= char_len;
      char_index++;
    }

    return char_index;
  }

  native_int Encoding::find_character_byte_index(const uint8_t* start, const uint8_t* end, native_int index, OnigEncodingType* enc) {
    uint8_t* p = (uint8_t*) start;
    while(p < end && index--) {
      p += mbclen(p, end, enc);
    }

    if(p > end) p = (uint8_t*) end;
    return p - start;
  }

  int Encoding::mbclen(const uint8_t* p, const uint8_t* e, OnigEncodingType* enc) {
    int n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);

    if(ONIGENC_MBCLEN_CHARFOUND_P(n) && ONIGENC_MBCLEN_CHARFOUND_LEN(n) <= e-p) {
      return ONIGENC_MBCLEN_CHARFOUND_LEN(n);
    } else {
      int min = ONIGENC_MBC_MINLEN(enc);
      return min <= e-p ? min : (int)(e-p);
    }
  }

  int Encoding::precise_mbclen(const uint8_t* p, const uint8_t* e, OnigEncodingType* enc) {
    int n;

    if(e <= p) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(1);
    }

    n = ONIGENC_PRECISE_MBC_ENC_LEN(enc, (UChar*)p, (UChar*)e);
    if(e-p < n) {
      return ONIGENC_CONSTRUCT_MBCLEN_NEEDMORE(n-(int)(e-p));
    }

    return n;
  }

  void Encoding::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    Encoding* enc_o = force_as<Encoding>(obj);
    if(!enc_o->managed_) return;

    OnigEncodingType* enc = enc_o->get_encoding();
    if(!enc) return;

    ByteArray* enc_ba = ByteArray::from_body(enc);
    ByteArray* tmp_ba = force_as<ByteArray>(mark.call(enc_ba));

    if(tmp_ba && tmp_ba != enc_ba) {
      enc_o->set_encoding(reinterpret_cast<OnigEncodingType*>(tmp_ba->raw_bytes()));
      mark.just_set(obj, tmp_ba);

      enc = enc_o->get_encoding();
    }

    if(enc->name) {
      ByteArray* ba = ByteArray::from_body(const_cast<char*>(enc->name));
      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp && tmp != ba) {
        enc->name = reinterpret_cast<const char*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }
  }

  void Encoding::Info::show(STATE, Object* self, int level) {
    Encoding* enc = as<Encoding>(self);

    class_header(state, self);
    indent_attribute(++level, "name"); enc->name()->show_simple(state, level);
    indent_attribute(level, "dummy?"); enc->dummy()->show_simple(state, level);
    close_body(level);
  }

  void Transcoding::init(STATE) {
    ontology::new_class_under(state, "Transcoding", G(encoding));

    G(encoding)->set_const(state, "TranscodingMap", LookupTable::create(state));

#include "gen/transcoder_database.cpp"
  }

  Transcoding* Transcoding::create(STATE, OnigTranscodingType* tr) {
    Class* cls = Encoding::transcoding_class(state);
    Transcoding* t = state->new_object<Transcoding>(cls);

    t->source(state, String::create(state, tr->src_encoding));
    t->target(state, String::create(state, tr->dst_encoding));
    t->transcoder_ = tr;

    return t;
  }

  void Transcoding::declare(STATE, const char* from, const char* to, const char* lib) {
    LookupTable* map = Encoding::transcoding_map(state);
    Object* obj = map->fetch(state, encoding_symbol(state, from));
    LookupTable* table;

    if(obj->nil_p()) {
      table = LookupTable::create(state);
      map->store(state, encoding_symbol(state, from), table);
    } else {
      table = as<LookupTable>(obj);
    }

    table->store(state, encoding_symbol(state, to), String::create(state, lib));
  }

  void Transcoding::define(STATE, OnigTranscodingType* tr) {
    LookupTable* map = Encoding::transcoding_map(state);
    Object* obj = map->fetch(state, encoding_symbol(state, tr->src_encoding));
    LookupTable* table;

    if(obj->nil_p()) {
      table = LookupTable::create(state);
      map->store(state, encoding_symbol(state, tr->src_encoding), table);
    } else {
      table = as<LookupTable>(obj);
    }

    Transcoding* t = Transcoding::create(state, tr);
    table->store(state, encoding_symbol(state, tr->dst_encoding), t);
  }

  void Converter::init(STATE) {
    Class* cls = ontology::new_class_under(state, "Converter", G(encoding));

    cls->set_const(state, "INVALID_MASK", Fixnum::from(ECONV_INVALID_MASK));
    cls->set_const(state, "INVALID_REPLACE", Fixnum::from(ECONV_INVALID_REPLACE));
    cls->set_const(state, "UNDEF_MASK", Fixnum::from(ECONV_UNDEF_MASK));
    cls->set_const(state, "UNDEF_REPLACE", Fixnum::from(ECONV_UNDEF_REPLACE));
    cls->set_const(state, "UNDEF_HEX_CHARREF", Fixnum::from(ECONV_UNDEF_HEX_CHARREF));
    cls->set_const(state, "PARTIAL_INPUT", Fixnum::from(ECONV_PARTIAL_INPUT));
    cls->set_const(state, "AFTER_OUTPUT", Fixnum::from(ECONV_AFTER_OUTPUT));
    cls->set_const(state, "UNIVERSAL_NEWLINE_DECORATOR",
                   Fixnum::from(ECONV_UNIVERSAL_NEWLINE_DECORATOR));
    cls->set_const(state, "CRLF_NEWLINE_DECORATOR",
                   Fixnum::from(ECONV_CRLF_NEWLINE_DECORATOR));
    cls->set_const(state, "CR_NEWLINE_DECORATOR",
                   Fixnum::from(ECONV_CR_NEWLINE_DECORATOR));
    cls->set_const(state, "XML_TEXT_DECORATOR",
                   Fixnum::from(ECONV_XML_TEXT_DECORATOR));
    cls->set_const(state, "XML_ATTR_CONTENT_DECORATOR",
                   Fixnum::from(ECONV_XML_ATTR_CONTENT_DECORATOR));
    cls->set_const(state, "XML_ATTR_QUOTE_DECORATOR",
                   Fixnum::from(ECONV_XML_ATTR_QUOTE_DECORATOR));
  }

  Converter* Converter::allocate(STATE, Object* self) {
    Class* cls = Encoding::converter_class(state);
    Converter* c = state->new_object<Converter>(cls);

    c->klass(state, as<Class>(self));

    c->set_converter(NULL);

    state->memory()->needs_finalization(c, (FinalizerFunction)&Converter::finalize);

    return c;
  }

  static Symbol* converter_result_symbol(STATE, rb_econv_result_t status) {
    switch(status) {
    case econv_invalid_byte_sequence:
      return state->symbol("invalid_byte_sequence");
    case econv_undefined_conversion:
      return state->symbol("undefined_conversion");
    case econv_destination_buffer_full:
      return state->symbol("destination_buffer_full");
    case econv_source_buffer_empty:
      return state->symbol("source_buffer_empty");
    case econv_finished:
      return state->symbol("finished");
    case econv_after_output:
      return state->symbol("after_output");
    case econv_incomplete_input:
      return state->symbol("incomplete_input");
    default:
      return state->symbol("unknown_converter_error");
    }
  }

  Symbol* Converter::primitive_convert(STATE, Object* source, String* target,
                                       Fixnum* offset, Fixnum* size, Fixnum* options) {
    String* src = 0;

    if(!source->nil_p()) {
      if(!(src = try_as<String>(source))) {
        return force_as<Symbol>(Primitives::failure());
      }
    }

    OnStack<3> os(state, source, src, target);

    const unsigned char* source_ptr = 0;
    const unsigned char* source_end = 0;

    native_int byte_offset = offset->to_native();
    native_int byte_size = size->to_native();

  retry:

    if(!converter_) {
      size_t num_converters = converters()->size();

      converter_ = rb_econv_alloc(num_converters);

      for(size_t i = 0; i < num_converters; i++) {
        Transcoding* transcoding = as<Transcoding>(converters()->get(state, i));
        rb_transcoder* tr = transcoding->get_transcoder();

        if(rb_econv_add_transcoder_at(converter_, tr, i) == -1) {
          rb_econv_free(converter_);
          converter_ = NULL;
          return force_as<Symbol>(Primitives::failure());
        }
      }
    }

    /* It would be nice to have a heuristic that avoids having to reconvert
     * after growing the destination buffer. This is complicated, however, as
     * a converter may contain more than one transcoder. So, the heuristic
     * would need to be transitive. This requires getting the encoding objects
     * for every stage of the converter to check the min/max byte values.
     */
    if(byte_size == -1) {
      byte_size = src ? src->byte_size() : 4096;
    }

    int flags = converter_->flags = options->to_native();

    if(!replacement()->nil_p()) {
      native_int byte_size = replacement()->byte_size();
      char* buf = (char*)XMALLOC(byte_size + 1);
      strncpy(buf, replacement()->c_str(state), byte_size + 1);
      converter_->replacement_str = (const unsigned char*)buf;
      converter_->replacement_len = replacement()->byte_size();

      String* name = replacement()->encoding()->name();
      byte_size = name->byte_size();
      buf = (char*)XMALLOC(byte_size + 1);
      strncpy(buf, name->c_str(state), byte_size + 1);
      converter_->replacement_enc = (const char*)buf;
      converter_->replacement_allocated = 1;

      size_t num_converters = replacement_converters()->size();
      rb_econv_alloc_replacement_converters(converter_, num_converters / 2);

      for(size_t i = 0, k = 0; i < num_converters; k++, i += 2) {
        rb_econv_replacement_converters* repl_converter;
        repl_converter = converter_->replacement_converters + k;

        name = as<String>(replacement_converters()->get(state, i));
        byte_size = name->byte_size();
        buf = (char*)XMALLOC(byte_size + 1);
        strncpy(buf, name->c_str(state), byte_size + 1);
        repl_converter->destination_encoding_name = (const char*)buf;

        Array* trs = as<Array>(replacement_converters()->get(state, i + 1));
        size_t num_transcoders = trs->size();

        repl_converter->num_transcoders = num_transcoders;
        repl_converter->transcoders = ALLOC_N(rb_transcoder*, num_transcoders);

        for(size_t j = 0; j < num_transcoders; j++) {
          Transcoding* transcoding = as<Transcoding>(trs->get(state, j));
          rb_transcoder* tr = transcoding->get_transcoder();

          repl_converter->transcoders[j] = tr;
        }
      }
    }

    if(src) {
      source_ptr = (const unsigned char*)src->c_str(state);
      source_end = source_ptr + src->byte_size();
    } else {
      source_ptr = source_end = NULL;
    }

    native_int buffer_size = byte_offset + byte_size;
    ByteArray* buffer = ByteArray::create(state, buffer_size);

    unsigned char* buffer_ptr = (unsigned char*)buffer->raw_bytes() + byte_offset;
    unsigned char* buffer_end = buffer_ptr + byte_size;

    rb_econv_result_t result = econv_convert(converter_, &source_ptr, source_end,
        &buffer_ptr, buffer_end, flags);

    native_int output_size = buffer_ptr - (unsigned char*)buffer->raw_bytes();

    if(result == econv_destination_buffer_full && size->to_native() == -1) {
      rb_econv_free(converter_);
      converter_ = NULL;

      byte_size = byte_size < 2 ? 2 : byte_size * 2;

      // Check if the size has overflown, then raise exception
      if(byte_size < 0) {
        Exception::argument_error(state, "string sizes too big");
      }

      goto retry;
    }

    if(byte_offset > 0) {
      memcpy(buffer->raw_bytes(), target->data()->raw_bytes(), byte_offset);
    }

    target->data(state, buffer);
    target->num_bytes(state, Fixnum::from(output_size));
    target->shared(state, cFalse);
    target->hash_value(state, nil<Fixnum>());
    target->ascii_only(state, cNil);
    target->encoding(state, destination_encoding());

    return converter_result_symbol(state, result);
  }

  String* Converter::finish(STATE) {
    return nil<String>();
  }

  LookupTable* Converter::last_error(STATE) {
    if(!converter_) return nil<LookupTable>();

    size_t read_again_length = 0;
    unsigned int codepoint = 0;
    bool codepoint_set = false;

    switch(converter_->last_error.result) {
    case econv_invalid_byte_sequence:
    case econv_incomplete_input:
      read_again_length = converter_->last_error.readagain_len;
      break;
    case econv_undefined_conversion:
      if(strncmp(converter_->last_error.source_encoding, "UTF-8", 5) == 0) {
        const uint8_t* p = (const uint8_t*)converter_->last_error.error_bytes_start;
        size_t len = converter_->last_error.error_bytes_len;
        const uint8_t* e = p + len;
        OnigEncodingType* utf8 = Encoding::utf8_encoding(state)->get_encoding();
        int n = Encoding::precise_mbclen(p, e, utf8);

        if(ONIGENC_MBCLEN_CHARFOUND_P(n) &&
           (size_t)ONIGENC_MBCLEN_CHARFOUND_LEN(n) == len) {
          codepoint = ONIGENC_MBC_TO_CODE(utf8, p, e);
          codepoint_set = true;
        }
      }
      break;
    default:
      return nil<LookupTable>();
    }

    LookupTable* error = LookupTable::create(state);

    Symbol* result = converter_result_symbol(state, converter_->last_error.result);
    error->store(state, state->symbol("result"), result);

    String* src_enc = String::create(state, converter_->last_error.source_encoding);
    error->store(state, state->symbol("source_encoding_name"), src_enc);

    String* dst_enc = String::create(state, converter_->last_error.destination_encoding);
    error->store(state, state->symbol("destination_encoding_name"), dst_enc);

    const char* error_start = (const char*)converter_->last_error.error_bytes_start;
    size_t error_length = converter_->last_error.error_bytes_len;

    String* error_bytes = String::create(state, error_start, error_length);
    error->store(state, state->symbol("error_bytes"), error_bytes);

    if(read_again_length) {
      String* read_again_bytes = String::create(state,
          error_start + error_length, read_again_length);
      error->store(state, state->symbol("read_again_bytes"), read_again_bytes);
    }

    if(codepoint_set) {
      error->store(state, state->symbol("codepoint"), Integer::from(state, codepoint));
    }

    return error;
  }

  Array* Converter::primitive_errinfo(STATE) {
    Array* info = Array::create(state, 5);
    info->set(state, 4, cNil);

    if(!converter_) {
      info->set(state, 0, state->symbol("source_buffer_empty"));
      return info;
    }

    Symbol* result = converter_result_symbol(state, converter_->last_error.result);
    info->set(state, 0, result);

    if(converter_->last_error.source_encoding) {
      info->set(state, 1,
                String::create(state, converter_->last_error.source_encoding));
    }

    if(converter_->last_error.destination_encoding) {
      info->set(state, 2,
                String::create(state, converter_->last_error.destination_encoding));
    }

    if(converter_->last_error.error_bytes_start) {
      const char* error_start = (const char*)converter_->last_error.error_bytes_start;
      size_t error_length = converter_->last_error.error_bytes_len;

      info->set(state, 3, String::create(state, error_start, error_length));
      info->set(state, 4, String::create(state, error_start + error_length,
                                         converter_->last_error.readagain_len));
    }

    return info;
  }

  String* Converter::putback(STATE, Object* maxbytes) {
    return nil<String>();
  }

  void Converter::finalize(STATE, Converter* converter) {
    if(rb_econv_t* ec = converter->get_converter()) {
      rb_econv_free(ec);
    }
  }
}
