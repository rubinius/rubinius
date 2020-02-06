#include "oniguruma.h" // Must be first.
#include "transcoder.h"

#include "arguments.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "memory.hpp"
#include "object_utils.hpp"
#include "primitives.hpp"

#include "class/block_environment.hpp"
#include "class/byte_array.hpp"
#include "class/class.hpp"
#include "class/encoding.hpp"
#include "class/exception.hpp"
#include "class/integer.hpp"
#include "class/lookup_table.hpp"
#include "class/proc.hpp"
#include "class/regexp.hpp"
#include "class/string.hpp"
#include "class/symbol.hpp"
#include "class/tuple.hpp"
#include "class/variable_scope.hpp"

#include <sstream>

#define OPTION_IGNORECASE         ONIG_OPTION_IGNORECASE
#define OPTION_EXTENDED           ONIG_OPTION_EXTEND
#define OPTION_MULTILINE          ONIG_OPTION_MULTILINE
#define OPTION_DONT_CAPTURE_GROUP ONIG_OPTION_DONT_CAPTURE_GROUP
#define OPTION_CAPTURE_GROUP      ONIG_OPTION_CAPTURE_GROUP
#define OPTION_MASK               (OPTION_IGNORECASE|OPTION_EXTENDED|OPTION_MULTILINE|ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP)

// Must match up with options/kcode definitions in grammar and regexp.rb
#define KCODE_NONE        (1 << 9)
#define KCODE_EUC         (2 << 9)
#define KCODE_SJIS        (3 << 9)
#define KCODE_UTF8        (4 << 9)
#define KCODE_MASK        (KCODE_NONE|KCODE_EUC|KCODE_SJIS|KCODE_UTF8)

#define OPTION_FIXEDENCODING 16
#define OPTION_NOENCODING    32

namespace rubinius {

  void Regexp::bootstrap(STATE) {
    onig_init();
    GO(regexp).set(state->memory()->new_class<Class, Regexp>(state, "Regexp"));

    GO(matchdata).set(state->memory()->new_class<Class, MatchData>(state, "MatchData"));
  }

  char *Regexp::version(STATE) {
    return (char*)onig_version();
  }

  Encoding* Regexp::encoding(STATE) {
    return source()->encoding(state);
  }

  Encoding* Regexp::encoding(STATE, Encoding* enc) {
    source()->encoding(state, enc);
    return enc;
  }

  struct _gather_data {
    STATE;
    LookupTable* tbl;
  };

  static int _gather_names(const UChar *name, const UChar *name_end,
      int ngroup_num, int *group_nums, regex_t *reg, struct _gather_data *gd) {
    STATE = gd->state;
    Array* ary = Array::create(state, ngroup_num);

    for(int i = 0; i < ngroup_num; i++) {
      ary->set(state, i, Fixnum::from(group_nums[i]));
    }

    gd->tbl->store(state, state->symbol((char*)name), ary);
    return 0;
  }

  /*
   * Only initialize the object, not oniguruma.  This allows copying of the
   * regular expression via Regexp#initialize_copy
   */
  Regexp* Regexp::create(STATE) {
    return Regexp::allocate(state, G(regexp));
  }

  regex_t* Regexp::onig_source_data(STATE) {
    if(source()->nil_p()) return NULL;
    return onig_data[source()->encoding(state)->cache_index()];
  }

  regex_t* Regexp::onig_data_encoded(STATE, Encoding* enc) {
    return onig_data[enc->cache_index()];
  }

  regex_t* Regexp::make_managed(STATE, Encoding* enc, regex_t* reg) {
    Regexp* obj = this;
    regex_t* orig = reg;

    assert(reg->chain == 0);

    ByteArray* reg_ba =
      state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), sizeof(regex_t));
    memcpy(reg_ba->raw_bytes(), reg, sizeof(regex_t));

    reg = reinterpret_cast<regex_t*>(reg_ba->raw_bytes());

    if(reg->p) {
      ByteArray* pattern =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), reg->alloc);
      memcpy(pattern->raw_bytes(), reg->p, reg->alloc);

      reg->p = reinterpret_cast<unsigned char*>(pattern->raw_bytes());

      obj->write_barrier(state, pattern);
    }

    if(reg->exact) {
      int exact_size = reg->exact_end - reg->exact;
      ByteArray* exact =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), exact_size);
      memcpy(exact->raw_bytes(), reg->exact, exact_size);

      reg->exact = reinterpret_cast<unsigned char*>(exact->raw_bytes());
      reg->exact_end = reg->exact + exact_size;

      obj->write_barrier(state, exact);
    }

    int int_map_size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;

    if(reg->int_map) {
      ByteArray* intmap =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), int_map_size);
      memcpy(intmap->raw_bytes(), reg->int_map, int_map_size);

      reg->int_map = reinterpret_cast<int*>(intmap->raw_bytes());

      obj->write_barrier(state, intmap);
    }

    if(reg->int_map_backward) {
      ByteArray* intmap_back =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), int_map_size);
      memcpy(intmap_back->raw_bytes(), reg->int_map_backward, int_map_size);

      reg->int_map_backward = reinterpret_cast<int*>(intmap_back->raw_bytes());

      obj->write_barrier(state, intmap_back);
    }

    if(reg->repeat_range) {
      int rrange_size = sizeof(OnigRepeatRange) * reg->repeat_range_alloc;
      ByteArray* rrange =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), rrange_size);
      memcpy(rrange->raw_bytes(), reg->repeat_range, rrange_size);

      reg->repeat_range = reinterpret_cast<OnigRepeatRange*>(rrange->raw_bytes());

      obj->write_barrier(state, rrange);
    }

    obj->onig_data[enc->cache_index()] = reg;
    obj->write_barrier(state, reg_ba);

    onig_free(orig);
    return reg;
  }

#define REGEXP_ONIG_ERROR_MESSAGE_LEN   ONIG_MAX_ERROR_MESSAGE_LEN + 1024

  // Called with the individual regexp lock held.
  regex_t* Regexp::maybe_recompile(STATE, String* string) {
    const UChar *pat;
    const UChar *end;
    OnigEncoding enc;
    OnigErrorInfo err_info;
    int err;

    if(fixed_encoding()) return onig_source_data(state);

    Encoding* string_enc = string->encoding(state);
    regex_t* onig_encoded = onig_data_encoded(state, string_enc);

    if(onig_encoded) return onig_encoded;

    enc = string_enc->encoding();

    Encoding* source_enc = source()->encoding(state);
    String* converted = source()->convert_escaped(state, source_enc, _fixed_encoding_);
    pat = (UChar*)converted->byte_address();
    end = pat + converted->byte_size();

    int options = onig_source_data(state)->options;
    OnigEncoding orig_enc = onig_source_data(state)->enc;
    regex_t* reg;

    err = onig_new(&reg, pat, end, options,
                   enc, ONIG_SYNTAX_RUBY, &err_info);

    // If it doesn't work out, then abort and reset the encoding back
    // and say that it's forced.
    if(err != ONIG_NORMAL) {

      err = onig_new(&reg, pat, end, options,
                     orig_enc, ONIG_SYNTAX_RUBY, &err_info);

      // Ok, wtf. Well, no way to proceed now.
      if(err != ONIG_NORMAL) {
        UChar onig_err_buf[ONIG_MAX_ERROR_MESSAGE_LEN];
        char err_buf[REGEXP_ONIG_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(onig_err_buf, err, &err_info);
        snprintf(err_buf, REGEXP_ONIG_ERROR_MESSAGE_LEN, "%s: %s", onig_err_buf, pat);

        Exception::raise_regexp_error(state, err_buf);
        return NULL;
      }

      string_enc = source_enc;
      fixed_encoding(true);
    }

    return make_managed(state, string_enc, reg);
  }

  /*
   * This is a primitive so #initialize_copy can work.
   */
  Regexp* Regexp::initialize(STATE, String* pattern, Fixnum* options) {
    const UChar *pat;
    const UChar *end;
    OnigErrorInfo err_info;
    OnigEncoding enc;

    OnigOptionType opts = options->to_native();
    Encoding* original_enc = pattern->encoding(state);

    fixed_encoding(opts & OPTION_FIXEDENCODING);
    no_encoding(opts & OPTION_NOENCODING);

    Encoding* source_enc = original_enc;

    switch(opts & KCODE_MASK) {
    case KCODE_NONE:
      no_encoding(true);
      break;
    case KCODE_EUC:
      source_enc = Encoding::find(state, "EUC-JP");
      fixed_encoding(true);
      break;
    case KCODE_SJIS:
      source_enc = Encoding::find(state, "Windows-31J");
      fixed_encoding(true);
      break;
    case KCODE_UTF8:
      source_enc = Encoding::utf8_encoding(state);
      fixed_encoding(true);
      break;
    }

    if(no_encoding()) source_enc = 0;
    String* converted = pattern->convert_escaped(state, source_enc, _fixed_encoding_);

    pat = (UChar*)converted->byte_address();
    end = pat + converted->byte_size();
    enc = source_enc->encoding();

    pattern = pattern->string_dup(state);
    pattern->encoding(state, source_enc);

    regex_t* reg;

    int err = onig_new(&reg, pat, end, opts & OPTION_MASK, enc, ONIG_SYNTAX_RUBY, &err_info);

    if(err != ONIG_NORMAL) {

      enc = original_enc->encoding();
      fixed_encoding(true);
      err = onig_new(&reg, pat, end, opts & OPTION_MASK, enc, ONIG_SYNTAX_RUBY, &err_info);
      pattern->encoding(state, original_enc);

      if(err != ONIG_NORMAL) {
        UChar onig_err_buf[ONIG_MAX_ERROR_MESSAGE_LEN];
        char err_buf[REGEXP_ONIG_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(onig_err_buf, err, &err_info);
        snprintf(err_buf, REGEXP_ONIG_ERROR_MESSAGE_LEN, "%s: %s", onig_err_buf, pat);

        Exception::raise_regexp_error(state, err_buf);
        return 0;
      }
    }

    this->source(state, pattern);

    int num_names = onig_number_of_names(reg);

    if(num_names == 0) {
      this->names(state, nil<LookupTable>());
    } else {
      struct _gather_data gd;
      gd.state = state;
      LookupTable* tbl = LookupTable::create(state);
      gd.tbl = tbl;
      onig_foreach_name(reg, (int (*)(const OnigUChar*, const OnigUChar*,int,int*,OnigRegex,void*))_gather_names, (void*)&gd);
      this->names(state, tbl);
    }

    make_managed(state, pattern->encoding(), reg);

    return this;
  }

  // 'self' is passed in automatically by the primitive glue
  Regexp* Regexp::allocate(STATE, Object* self) {
    return state->memory()->new_object<Regexp>(state, as<Class>(self));
  }

  Fixnum* Regexp::options(STATE) {
    if(unlikely(!onig_source_data(state))) {
      Exception::raise_type_error(state, "Not properly initialized Regexp");
    }

    int result = ((int)onig_get_options(onig_source_data(state)) & OPTION_MASK);

    if(fixed_encoding()) {
      result |= OPTION_FIXEDENCODING;
    }

    if(no_encoding()) {
      result |= OPTION_NOENCODING;
    }

    return Fixnum::from(result);
  }

  static Tuple* _md_region_to_tuple(STATE, OnigRegion *region, int pos) {
    Tuple* tup = Tuple::create(state, region->num_regs - 1);

    for(int i = 1; i < region->num_regs; i++) {
      int beg = region->beg[i];

      Tuple* sub;

      // If onig says the beginning is less than 0, then it's indicating
      // that there was no match for it. This happens with an optional
      // match like (a)?. We want to preserve this knowledge so we
      // don't add pos to it.
      if(beg < 0) {
        sub = Tuple::from(state, 2,
                          Fixnum::from(region->beg[i]),
                          Fixnum::from(region->end[i]));
      } else {
        sub = Tuple::from(state, 2,
                          Fixnum::from(region->beg[i] + pos),
                          Fixnum::from(region->end[i] + pos));
      }
      tup->put(state, i - 1, sub);
    }

    return tup;
  }

  static MatchData* get_match_data(STATE, OnigRegion *region, String* string, Regexp* regexp, int pos) {
    MatchData* md = state->memory()->new_object<MatchData>(state, G(matchdata));
    md->source(state, string->string_dup(state));
    md->regexp(state, regexp);
    // Unsure if the first region (the full match) can be less than 0 (meaning
    // the match was length length but did still match). We have to support
    // that for the > 0 case, so we'll do the same for 0.
    //

    if(region->beg[0] < 0) {
      Tuple* tup = Tuple::from(state, 2,
                               Fixnum::from(region->beg[0]),
                               Fixnum::from(region->end[0]));
      md->full(state, tup);
    } else {
      Tuple* tup = Tuple::from(state, 2,
                               Fixnum::from(region->beg[0] + pos),
                               Fixnum::from(region->end[0] + pos));
      md->full(state, tup);
    }
    md->region(state, _md_region_to_tuple(state, region, pos));
    return md;
  }

  MatchData* Regexp::match_region(STATE, String* string, Fixnum* start,
                               Fixnum* end, Object* forward)
  {
    int beg, max;
    const UChar *str;

    if(unlikely(!onig_source_data(state))) {
      Exception::raise_argument_error(state, "Not properly initialized Regexp");
    }

    if(unlikely(!CBOOL(string->valid_encoding_p(state)))) {
      std::ostringstream msg;
      msg << "invalid byte sequence in " << string->encoding(state)->name()->to_string(state);
      Exception::raise_argument_error(state, msg.str().c_str());
    }

    max = string->byte_size();
    str = (UChar*)string->byte_address();

    intptr_t i_start = start->to_native();
    intptr_t i_end = end->to_native();

    // Bounds check.
    if(i_start < 0 || i_end < 0 || i_start > max || i_end > max) {
      return nil<MatchData>();
    }

    lock_.lock();

    regex_t* data = maybe_recompile(state, string);
    if(!data) return 0;

    OnigPosition begin_reg[10] = { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };
    OnigPosition end_reg[10] =  { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };

    OnigRegion region = { 10, 0, begin_reg, end_reg, 0, 0 };

    int* back_match = data->int_map_backward;

    if(!CBOOL(forward)) {
      beg = onig_search(data, str, str + max,
                        str + i_end,
                        str + i_start,
                        &region, ONIG_OPTION_NONE);
    } else {
      beg = onig_search(data, str, str + max,
                        str + i_start,
                        str + i_end,
                        &region, ONIG_OPTION_NONE);
    }

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(data->int_map_backward != back_match) {
      intptr_t size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), size);
      memcpy(ba->raw_bytes(), data->int_map_backward, size);

      // Dispose of the old one.
      free(data->int_map_backward);

      data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }

    lock_.unlock();

    if(beg == ONIG_MISMATCH) {
      onig_region_free(&region, 0);
      return nil<MatchData>();
    }

    MatchData* md = get_match_data(state, &region, string, this, 0);
    onig_region_free(&region, 0);
    return md;
  }

  MatchData* Regexp::match_start(STATE, String* string, Fixnum* start) {
    int beg, max;
    const UChar *str;
    const UChar *fin;

    if(unlikely(!onig_source_data(state))) {
      Exception::raise_argument_error(state, "Not properly initialized Regexp");
    }

    if(unlikely(!CBOOL(string->valid_encoding_p(state)))) {
      std::ostringstream msg;
      msg << "invalid byte sequence in " << string->encoding(state)->name()->to_string(state);
      Exception::raise_argument_error(state, msg.str().c_str());
    }

    max = string->byte_size();
    intptr_t pos = start->to_native();

    str = (UChar*)string->byte_address();
    fin = str + max;

    // Bounds check.
    if(pos > max) return nil<MatchData>();
    str += pos;

    lock_.lock();

    regex_t* data = maybe_recompile(state, string);
    if(!data) return 0;

    OnigPosition begin_reg[10] = { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };
    OnigPosition end_reg[10] =  { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };

    OnigRegion region = { 10, 0, begin_reg, end_reg, 0, 0 };

    int* back_match = data->int_map_backward;
    beg = onig_match(data, str, fin, str, &region,
                     ONIG_OPTION_NONE);

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(data->int_map_backward != back_match) {
      intptr_t size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), size);
      memcpy(ba->raw_bytes(), data->int_map_backward, size);

      // Dispose of the old one.
      free(data->int_map_backward);

      data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }

    lock_.unlock();

    MatchData* md = nil<MatchData>();
    if(beg != ONIG_MISMATCH) {
      md = get_match_data(state, &region, string, this, pos);
    }

    onig_region_free(&region, 0);
    return md;
  }

  MatchData* Regexp::search_from(STATE, String* string, Fixnum* start) {
    int beg, max;
    const UChar *str;
    const UChar *fin;

    if(unlikely(!onig_source_data(state))) {
      Exception::raise_argument_error(state, "Not properly initialized Regexp");
    }

    if(unlikely(!CBOOL(string->valid_encoding_p(state)))) {
      std::ostringstream msg;
      msg << "invalid byte sequence in " << string->encoding(state)->name()->to_string(state);
      Exception::raise_argument_error(state, msg.str().c_str());
    }

    lock_.lock();

    regex_t* data = maybe_recompile(state, string);
    if(!data) return 0;

    max = string->byte_size();
    intptr_t pos = start->to_native();

    str = (UChar*)string->byte_address();
    fin = str + max;

    str += pos;

    OnigPosition begin_reg[10] = { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };
    OnigPosition end_reg[10] =  { ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS,
                         ONIG_REGION_NOTPOS, ONIG_REGION_NOTPOS };

    OnigRegion region = { 10, 0, begin_reg, end_reg, 0, 0 };

    int* back_match = data->int_map_backward;

    beg = onig_search(data, str, fin, str, fin,
                      &region, ONIG_OPTION_NONE);

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(data->int_map_backward != back_match) {
      intptr_t size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba =
        state->memory()->new_bytes_pinned<ByteArray>(state, G(bytearray), size);
      memcpy(ba->raw_bytes(), data->int_map_backward, size);

      // Dispose of the old one.
      free(data->int_map_backward);

      data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }

    lock_.unlock();

    MatchData* md = nil<MatchData>();
    if(beg != ONIG_MISMATCH) {
      md = get_match_data(state, &region, string, this, pos);
    }

    onig_region_free(&region, 0);
    return md;
  }

  String* MatchData::matched_string(STATE) {
    Fixnum* beg = as<Fixnum>(full()->at(state, 0));
    Fixnum* fin = as<Fixnum>(full()->at(state, 1));

    intptr_t max = source()->byte_size();
    intptr_t f = fin->to_native();
    intptr_t b = beg->to_native();

    String* string;

    if(f > max || b > max || b < 0) {
      string = String::create(state, 0, 0);
    } else {
      const char* str = (char*)source()->byte_address();
      intptr_t sz = fin->to_native() - beg->to_native();

      string = String::create(state, str + beg->to_native(), sz);
    }

    source()->infect(state, string);
    string->encoding_from(state, source());
    string->klass(state, source()->class_object(state));

    return string;
  }

  String* MatchData::pre_matched(STATE) {
    Fixnum* beg = as<Fixnum>(full()->at(state, 0));

    intptr_t max = source()->byte_size();
    intptr_t sz = beg->to_native();

    String* string;

    if(sz <= 0) {
      string = String::create(state, 0, 0);
    } else {
      if(sz > max) sz = max;

      const char* str = (char*)source()->byte_address();

      string = String::create(state, str, sz);
    }

    source()->infect(state, string);
    string->encoding_from(state, source());
    string->klass(state, source()->class_object(state));

    return string;
  }

  String* MatchData::post_matched(STATE) {
    Fixnum* fin = as<Fixnum>(full()->at(state, 1));

    intptr_t f = fin->to_native();
    intptr_t max = source()->byte_size();

    String* string;

    if(f >= max) {
      string = String::create(state, 0, 0);
    } else {
      const char* str = (char*)source()->byte_address();
      intptr_t sz = max - f;

      if(sz > max) sz = max;

      string = String::create(state, str + f, sz);
    }

    source()->infect(state, string);
    string->encoding_from(state, source());
    string->klass(state, source()->class_object(state));

    return string;
  }

  String* MatchData::nth_capture(STATE, intptr_t which) {
    if(region()->num_fields() <= which) return nil<String>();

    Tuple* sub = try_as<Tuple>(region()->at(state, which));
    if(!sub) return nil<String>();

    Fixnum* beg = as<Fixnum>(sub->at(state, 0));
    Fixnum* fin = as<Fixnum>(sub->at(state, 1));

    intptr_t b = beg->to_native();
    intptr_t f = fin->to_native();
    intptr_t max = source()->byte_size();

    if(f > max ||
       b < 0) {
      return nil<String>();
    }

    const char* str = (char*)source()->byte_address();
    intptr_t sz = f - b;

    if(sz > max) sz = max;

    String* string = String::create(state, str + b, sz);
    source()->infect(state, string);
    string->encoding_from(state, source());
    string->klass(state, source()->class_object(state));

    return string;
  }

  String* MatchData::last_capture(STATE) {
    if(region()->num_fields() == 0) return nil<String>();
    intptr_t captures = region()->num_fields();
    while(captures--) {
      String* capture = nth_capture(state, captures);
      if(!capture->nil_p()) {
        return capture;
      }
    }
    return nil<String>();
  }

  Object* Regexp::last_match_result(STATE, Fixnum* mode, Fixnum* which) {
    Object* current_match = last_match(state);

    if(MatchData* match = try_as<MatchData>(current_match)) {
      switch(mode->to_native()) {
      case 0:
        return match;
      case 1:
        return match->matched_string(state);
      case 2:
        return match->pre_matched(state);
      case 3:
        return match->post_matched(state);
      case 4:
        return match->last_capture(state);
      case 5:
        return match->nth_capture(state, which->to_native());
      }
    }
    return cNil;
  }

  Object* Regexp::last_match(STATE) {
    if(CallFrame* frame = state->get_variables_frame()) {
      return frame->scope->last_match(state);
    }

    return cNil;
  }

  Object* Regexp::last_match(STATE, Arguments& args) {
    if(MatchData* match = try_as<MatchData>(last_match(state))) {
      if(args.total() == 0) return match;
      if(args.total() > 1) return Primitives::failure();

      intptr_t which = as<Fixnum>(args.get_argument(0))->to_native();

      if(which == 0) {
        return match->matched_string(state);
      } else {
        return match->nth_capture(state, which - 1);
      }
    }

    return cNil;
  }

  Object* Regexp::set_last_match(STATE, Object* obj) {
    if(!obj->nil_p() && !kind_of<MatchData>(obj)) {
      return Primitives::failure();
    }

    if(CallFrame* frame = state->get_variables_frame(1)) {
      frame->scope->set_last_match(state, obj);
    }

    return obj;
  }

  Object* Regexp::propagate_last_match(STATE) {
    Object* obj = last_match(state);
    Regexp::set_last_match(state, obj);
    return obj;
  }

  Object* Regexp::set_block_last_match(STATE) {
    Object* blk = state->get_variables_frame()->scope->block();

    if(MatchData* match = try_as<MatchData>(last_match(state))) {
      if(BlockEnvironment* env = try_as<BlockEnvironment>(blk)) {
        env->top_scope()->last_match(state, match);
      } else if(Proc* proc = try_as<Proc>(blk)) {
        proc->block()->top_scope()->last_match(state, match);
      }

      return match;
    }

    return cNil;
  }

  void Regexp::Info::mark(STATE, Object* obj, std::function<void (STATE, Object**)> f) {
    auto_mark(state, obj, f);

    Regexp* reg_o = force_as<Regexp>(obj);
    for(int i = 0; i < cCachedOnigDatas; ++i) {
      regex_t* reg = reg_o->onig_data[i];
      if(!reg) continue;

      Object* reg_ba = ByteArray::from_body(reg);

      // This is pinned, it will not move
      f(state, &reg_ba);

      /* TODO: GC
      if(ByteArray* reg_tmp = force_as<ByteArray>(f(state, obj, reg_ba))) {
        reg_o->onig_data[i] = reinterpret_cast<regex_t*>(reg_tmp->raw_bytes());
        reg = reg_o->onig_data[i];
      }
      */

      if(reg->p) {
        Object* ba = ByteArray::from_body(reg->p);

        // This is pinned, it will not move
        f(state, &ba);

        /* TODO: GC
        if(ByteArray* tmp = force_as<ByteArray>(f(state, obj, ba))) {
          reg->p = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
        }
        */
      }

      if(reg->exact) {
        Object* ba = ByteArray::from_body(reg->exact);

        // This is pinned, it will not move
        f(state, &ba);

        /* TODO: GC
        int exact_size = reg->exact_end - reg->exact;
        if(ByteArray* tmp = force_as<ByteArray>(f(state, obj, ba))) {
          reg->exact = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
          reg->exact_end = reg->exact + exact_size;
        }
        */
      }

      if(reg->int_map) {
        Object* ba = ByteArray::from_body(reg->int_map);

        // This is pinned, it will not move
        f(state, &ba);

        /* TODO: GC
        if(ByteArray* tmp = force_as<ByteArray>(f(state, obj, ba))) {
          reg->int_map = reinterpret_cast<int*>(tmp->raw_bytes());
        }
        */
      }

      if(reg->int_map_backward) {
        Object* ba = ByteArray::from_body(reg->int_map_backward);

        // This is pinned, it will not move
        f(state, &ba);

        /* TODO: GC
        if(ByteArray* tmp = force_as<ByteArray>(f(state, obj, ba))) {
          reg->int_map_backward = reinterpret_cast<int*>(tmp->raw_bytes());
        }
        */
      }

      if(reg->repeat_range) {
        Object* ba = ByteArray::from_body(reg->repeat_range);

        // This is pinned, it will not move
        f(state, &ba);

        /* TODO: GC
        if(ByteArray* tmp = force_as<ByteArray>(f(state, obj, ba))) {
          reg->repeat_range = reinterpret_cast<OnigRepeatRange*>(tmp->raw_bytes());
        }
        */
      }
    }
  }
}
