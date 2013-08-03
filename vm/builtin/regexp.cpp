#include "oniguruma.h" // Must be first.
#include "transcoder.h"

#include "arguments.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/class.hpp"
#include "builtin/encoding.hpp"
#include "builtin/exception.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/proc.hpp"
#include "builtin/regexp.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/variable_scope.hpp"
#include "call_frame.hpp"
#include "configuration.hpp"
#include "object_utils.hpp"
#include "objectmemory.hpp"
#include "ontology.hpp"
#include "version.h"

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

  void Regexp::init(STATE) {
    onig_init();
    GO(regexp).set(ontology::new_class(state, "Regexp", G(object), 0));
    G(regexp)->set_object_type(state, RegexpType);

    GO(matchdata).set(ontology::new_class(state, "MatchData", G(object), 0));
    G(matchdata)->set_object_type(state, MatchDataType);
  }

  char *Regexp::version(STATE) {
    return (char*)onig_version();
  }

  Encoding* Regexp::encoding(STATE) {
    return source_->encoding(state);
  }

  Encoding* Regexp::encoding(STATE, Encoding* enc) {
    source_->encoding(state, enc);
    return enc;
  }

  static OnigEncoding get_enc_from_kcode(int kcode) {
    OnigEncoding r;

    r = ONIG_ENCODING_ASCII;
    switch (kcode) {
      case KCODE_NONE:
        r = ONIG_ENCODING_ASCII;
        break;
      case KCODE_EUC:
        r = ONIG_ENCODING_EUC_JP;
        break;
      case KCODE_SJIS:
        r = ONIG_ENCODING_Shift_JIS;
        break;
      case KCODE_UTF8:
        r = ONIG_ENCODING_UTF_8;
        break;
    }
    return r;
  }

  int get_kcode_from_enc(OnigEncoding enc) {
    int r = 0;
    if(enc == ONIG_ENCODING_ASCII)       r = KCODE_NONE;
    if(enc == ONIG_ENCODING_EUC_JP)      r = KCODE_EUC;
    if(enc == ONIG_ENCODING_Shift_JIS)   r = KCODE_SJIS;
    if(enc == ONIG_ENCODING_UTF_8)       r = KCODE_UTF8;
    return r;
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
    Regexp* o_reg = state->new_object<Regexp>(G(regexp));

    for(int i = 0; i < cCachedOnigDatas; ++i) {
      o_reg->onig_data[i] = NULL;
    }
    o_reg->lock_.init();
    o_reg->fixed_encoding_ = false;
    o_reg->no_encoding_ = false;

    return o_reg;
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

    ByteArray* reg_ba = ByteArray::create_dirty(state, sizeof(regex_t));
    memcpy(reg_ba->raw_bytes(), reg, sizeof(regex_t));

    reg = reinterpret_cast<regex_t*>(reg_ba->raw_bytes());

    if(reg->p) {
      ByteArray* pattern = ByteArray::create_dirty(state, reg->alloc);
      memcpy(pattern->raw_bytes(), reg->p, reg->alloc);

      reg->p = reinterpret_cast<unsigned char*>(pattern->raw_bytes());

      obj->write_barrier(state, pattern);
    }

    if(reg->exact) {
      int exact_size = reg->exact_end - reg->exact;
      ByteArray* exact = ByteArray::create_dirty(state, exact_size);
      memcpy(exact->raw_bytes(), reg->exact, exact_size);

      reg->exact = reinterpret_cast<unsigned char*>(exact->raw_bytes());
      reg->exact_end = reg->exact + exact_size;

      obj->write_barrier(state, exact);
    }

    int int_map_size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;

    if(reg->int_map) {
      ByteArray* intmap = ByteArray::create_dirty(state, int_map_size);
      memcpy(intmap->raw_bytes(), reg->int_map, int_map_size);

      reg->int_map = reinterpret_cast<int*>(intmap->raw_bytes());

      obj->write_barrier(state, intmap);
    }

    if(reg->int_map_backward) {
      ByteArray* intmap_back = ByteArray::create_dirty(state, int_map_size);
      memcpy(intmap_back->raw_bytes(), reg->int_map_backward, int_map_size);

      reg->int_map_backward = reinterpret_cast<int*>(intmap_back->raw_bytes());

      obj->write_barrier(state, intmap_back);
    }

    if(reg->repeat_range) {
      int rrange_size = sizeof(OnigRepeatRange) * reg->repeat_range_alloc;
      ByteArray* rrange = ByteArray::create_dirty(state, rrange_size);
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

    if(fixed_encoding_) return onig_source_data(state);

    Encoding* string_enc = string->get_encoding_kcode_fallback(state);
    regex_t* onig_encoded = onig_data_encoded(state, string_enc);

    if(onig_encoded) return onig_encoded;

    enc = string_enc->get_encoding();

    Encoding* source_enc = source()->encoding(state);
    String* converted = source()->convert_escaped(state, source_enc, fixed_encoding_);
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

        Exception::regexp_error(state, err_buf);
        return NULL;
      }

      string_enc = source_enc;
      fixed_encoding_ = true;
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

    if(LANGUAGE_18_ENABLED) {
      int kcode = opts & KCODE_MASK;

      pat = (UChar*)pattern->byte_address();
      end = pat + pattern->byte_size();

      if(kcode == 0) {
        enc = pattern->get_encoding_kcode_fallback(state)->get_encoding();
      } else {
        // Don't attempt to fix the encoding later, it's been specified by the
        // user.
        enc = get_enc_from_kcode(kcode);
        fixed_encoding_ = true;
      }
    } else {
      fixed_encoding_ = opts & OPTION_FIXEDENCODING;
      no_encoding_    = opts & OPTION_NOENCODING;

      Encoding* source_enc = original_enc;

      switch(opts & KCODE_MASK) {
      case KCODE_NONE:
        no_encoding_ = true;
        break;
      case KCODE_EUC:
        source_enc = Encoding::find(state, "EUC-JP");
        fixed_encoding_ = true;
        break;
      case KCODE_SJIS:
        source_enc = Encoding::find(state, "Windows-31J");
        fixed_encoding_ = true;
        break;
      case KCODE_UTF8:
        source_enc = Encoding::utf8_encoding(state);
        fixed_encoding_ = true;
        break;
      }

      if(no_encoding_) source_enc = 0;
      String* converted = pattern->convert_escaped(state, source_enc, fixed_encoding_);

      pat = (UChar*)converted->byte_address();
      end = pat + converted->byte_size();
      enc = source_enc->get_encoding();

      pattern = pattern->string_dup(state);
      pattern->encoding(state, source_enc);
    }

    regex_t* reg;

    int err = onig_new(&reg, pat, end, opts & OPTION_MASK, enc, ONIG_SYNTAX_RUBY, &err_info);

    if(err != ONIG_NORMAL) {

      enc = original_enc->get_encoding();
      fixed_encoding_ = true;
      err = onig_new(&reg, pat, end, opts & OPTION_MASK, enc, ONIG_SYNTAX_RUBY, &err_info);
      pattern->encoding(state, original_enc);

      if(err != ONIG_NORMAL) {
        UChar onig_err_buf[ONIG_MAX_ERROR_MESSAGE_LEN];
        char err_buf[REGEXP_ONIG_ERROR_MESSAGE_LEN];
        onig_error_code_to_str(onig_err_buf, err, &err_info);
        snprintf(err_buf, REGEXP_ONIG_ERROR_MESSAGE_LEN, "%s: %s", onig_err_buf, pat);

        Exception::regexp_error(state, err_buf);
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
    Regexp* re = Regexp::create(state);
    re->klass(state, as<Class>(self));
    return re;
  }

  Fixnum* Regexp::options(STATE) {
    if(unlikely(!onig_source_data(state))) {
      Exception::type_error(state, "Not properly initialized Regexp");
    }

    int result = ((int)onig_get_options(onig_source_data(state)) & OPTION_MASK);

    if(LANGUAGE_18_ENABLED) {
      if(fixed_encoding_) {
        result |= get_kcode_from_enc(onig_get_encoding(onig_source_data(state)));
      }
    } else {
      if(fixed_encoding_) {
        result |= OPTION_FIXEDENCODING;
      }
      if(no_encoding_) {
        result |= OPTION_NOENCODING;
      }
    }

    return Fixnum::from(result);
  }

  Object* Regexp::fixed_encoding_p(STATE) {
    return RBOOL(fixed_encoding_);
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
    MatchData* md = state->new_object<MatchData>(G(matchdata));
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
      Exception::argument_error(state, "Not properly initialized Regexp");
    }

    max = string->byte_size();
    str = (UChar*)string->byte_address();

    native_int i_start = start->to_native();
    native_int i_end = end->to_native();

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
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create_dirty(state, size);
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
      Exception::argument_error(state, "Not properly initialized Regexp");
    }

    max = string->byte_size();
    native_int pos = start->to_native();

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
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create_dirty(state, size);
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
      Exception::argument_error(state, "Not properly initialized Regexp");
    }

    lock_.lock();

    regex_t* data = maybe_recompile(state, string);
    if(!data) return 0;

    max = string->byte_size();
    native_int pos = start->to_native();

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
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create_dirty(state, size);
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
    Fixnum* beg = try_as<Fixnum>(full_->at(state, 0));
    Fixnum* fin = try_as<Fixnum>(full_->at(state, 1));

    native_int max = source_->byte_size();
    native_int f = fin->to_native();
    native_int b = beg->to_native();

    String* string;

    if(!beg || !fin ||
        f > max || b > max || b < 0) {
      string = String::create(state, 0, 0);
    } else {
      const char* str = (char*)source_->byte_address();
      native_int sz = fin->to_native() - beg->to_native();

      string = String::create(state, str + beg->to_native(), sz);
    }

    string->encoding(state, source_->encoding());
    string->ascii_only(state, source_->ascii_only());

    return string;
  }

  String* MatchData::pre_matched(STATE) {
    Fixnum* beg = try_as<Fixnum>(full_->at(state, 0));

    native_int max = source_->byte_size();
    native_int sz = beg->to_native();

    String* string;

    if(!beg || sz <= 0) {
      string = String::create(state, 0, 0);
    } else {
      if(sz > max) sz = max;

      const char* str = (char*)source_->byte_address();

      string = String::create(state, str, sz);
    }

    string->encoding(state, source_->encoding());
    string->ascii_only(state, source_->ascii_only());

    return string;
  }

  String* MatchData::post_matched(STATE) {
    Fixnum* fin = try_as<Fixnum>(full_->at(state, 1));

    native_int f = fin->to_native();
    native_int max = source_->byte_size();

    String* string;

    if(!fin || f >= max) {
      string = String::create(state, 0, 0);
    } else {
      const char* str = (char*)source_->byte_address();
      native_int sz = max - f;

      if(sz > max) sz = max;

      string = String::create(state, str + f, sz);
    }

    string->encoding(state, source_->encoding());
    string->ascii_only(state, source_->ascii_only());

    return string;
  }

  String* MatchData::nth_capture(STATE, native_int which) {
    if(region_->num_fields() <= which) return nil<String>();

    Tuple* sub = try_as<Tuple>(region_->at(state, which));
    if(!sub) return nil<String>();

    Fixnum* beg = try_as<Fixnum>(sub->at(state, 0));
    Fixnum* fin = try_as<Fixnum>(sub->at(state, 1));

    native_int b = beg->to_native();
    native_int f = fin->to_native();
    native_int max = source_->byte_size();

    if(!beg || !fin ||
        f > max ||
        b < 0) {
      return nil<String>();
    }

    const char* str = (char*)source_->byte_address();
    native_int sz = f - b;

    if(sz > max) sz = max;

    String* string = String::create(state, str + b, sz);
    string->encoding(state, source_->encoding());
    string->ascii_only(state, source_->ascii_only());

    return string;
  }

  String* MatchData::last_capture(STATE) {
    if(region_->num_fields() == 0) return nil<String>();
    native_int captures = region_->num_fields();
    while(captures--) {
      String* capture = nth_capture(state, captures);
      if(!capture->nil_p()) {
        return capture;
      }
    }
    return nil<String>();
  }

  Object* Regexp::last_match_result(STATE, Fixnum* mode, Fixnum* which,
                                    CallFrame* call_frame)
  {
    Object* current_match = call_frame->last_match(state);

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

  Object* Regexp::last_match(STATE, Arguments& args, CallFrame* call_frame) {
    MatchData* match = try_as<MatchData>(call_frame->last_match(state));
    if(!match) return cNil;

    if(args.total() == 0) return match;
    if(args.total() > 1) return Primitives::failure();

    native_int which = as<Fixnum>(args.get_argument(0))->to_native();

    if(which == 0) {
      return match->matched_string(state);
    } else {
      return match->nth_capture(state, which - 1);
    }
  }

  Object* Regexp::set_last_match(STATE, Object* obj, CallFrame* call_frame) {
    if(!obj->nil_p() && !kind_of<MatchData>(obj)) {
      return Primitives::failure();
    }

    if(CallFrame* parent = call_frame->previous) {
      parent->set_last_match(state, obj);
    }

    return obj;
  }

  Object* Regexp::propagate_last_match(STATE, CallFrame* call_frame) {
    Object* obj = call_frame->last_match(state);
    if(CBOOL(obj)) {
      Regexp::set_last_match(state, obj, call_frame);
    }
    return obj;
  }

  Object* Regexp::set_block_last_match(STATE, CallFrame* call_frame) {
    Object* blk = call_frame->scope->block();
    MatchData* match = try_as<MatchData>(call_frame->last_match(state));
    if(!match) return cNil;

    if(BlockEnvironment* env = try_as<BlockEnvironment>(blk)) {
      env->top_scope()->last_match(state, match);
    } else if(Proc* proc = try_as<Proc>(blk)) {
      proc->block()->top_scope()->last_match(state, match);
    }

    return match;
  }

  void Regexp::Info::mark(Object* obj, ObjectMark& mark) {
    auto_mark(obj, mark);

    Regexp* reg_o = force_as<Regexp>(obj);
    for(int i = 0; i < cCachedOnigDatas; ++i) {
      regex_t* reg = reg_o->onig_data[i];
      if(!reg) continue;

      ByteArray* reg_ba = ByteArray::from_body(reg);
      ByteArray* reg_tmp = force_as<ByteArray>(mark.call(reg_ba));
      if(reg_tmp && reg_tmp != reg_ba) {
        reg_o->onig_data[i] = reinterpret_cast<regex_t*>(reg_tmp->raw_bytes());
        mark.just_set(obj, reg_tmp);
        reg = reg_o->onig_data[i];
      }

      if(reg->p) {
        ByteArray* ba = ByteArray::from_body(reg->p);

        ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
        if(tmp && tmp != ba) {
          reg->p = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
          mark.just_set(obj, tmp);
        }
      }

      if(reg->exact) {
        int exact_size = reg->exact_end - reg->exact;
        ByteArray* ba = ByteArray::from_body(reg->exact);

        ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
        if(tmp && tmp != ba) {
          reg->exact = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
          reg->exact_end = reg->exact + exact_size;
          mark.just_set(obj, tmp);
        }
      }

      if(reg->int_map) {
        ByteArray* ba = ByteArray::from_body(reg->int_map);

        ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
        if(tmp && tmp != ba) {
          reg->int_map = reinterpret_cast<int*>(tmp->raw_bytes());
          mark.just_set(obj, tmp);
        }
      }

      if(reg->int_map_backward) {
        ByteArray* ba = ByteArray::from_body(reg->int_map_backward);

        ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
        if(tmp && tmp != ba) {
          reg->int_map_backward = reinterpret_cast<int*>(tmp->raw_bytes());
          mark.just_set(obj, tmp);
        }
      }

      if(reg->repeat_range) {
        ByteArray* ba = ByteArray::from_body(reg->repeat_range);

        ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
        if(tmp && tmp != ba) {
          reg->repeat_range = reinterpret_cast<OnigRepeatRange*>(tmp->raw_bytes());
          mark.just_set(obj, tmp);
        }
      }
    }
  }
}
