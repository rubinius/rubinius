#include "oniguruma.h" // Must be first.

#include "builtin/regexp.hpp"
#include "builtin/class.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"
#include "builtin/bytearray.hpp"
#include "builtin/block_environment.hpp"
#include "builtin/proc.hpp"

#include "vm.hpp"
#include "vm/object_utils.hpp"
#include "objectmemory.hpp"
#include "call_frame.hpp"
#include "arguments.hpp"

#include "gc/gc.hpp"

#define OPTION_IGNORECASE         ONIG_OPTION_IGNORECASE
#define OPTION_EXTENDED           ONIG_OPTION_EXTEND
#define OPTION_MULTILINE          ONIG_OPTION_MULTILINE
#define OPTION_DONT_CAPTURE_GROUP ONIG_OPTION_DONT_CAPTURE_GROUP
#define OPTION_CAPTURE_GROUP      ONIG_OPTION_CAPTURE_GROUP
#define OPTION_MASK               (OPTION_IGNORECASE|OPTION_EXTENDED|OPTION_MULTILINE|ONIG_OPTION_DONT_CAPTURE_GROUP|ONIG_OPTION_CAPTURE_GROUP)

#define KCODE_ASCII       0
#define KCODE_NONE        16
#define KCODE_EUC         32
#define KCODE_SJIS        48
#define KCODE_UTF8        64
#define KCODE_MASK        (KCODE_EUC|KCODE_SJIS|KCODE_UTF8)

namespace rubinius {

  void Regexp::init(STATE) {
    onig_init();
    GO(regexp).set(state->new_class("Regexp", G(object), 0));
    G(regexp)->set_object_type(state, RegexpType);

    GO(matchdata).set(state->new_class("MatchData", G(object), 0));
    G(matchdata)->set_object_type(state, MatchDataType);
  }

  char *Regexp::version(STATE) {
    return (char*)onig_version();
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
        r = ONIG_ENCODING_SJIS;
        break;
      case KCODE_UTF8:
        r = ONIG_ENCODING_UTF8;
        break;
    }
    return r;
  }

  static OnigEncoding current_encoding(STATE) {
    switch(state->shared.kcode_page()) {
    default:
    case kcode::eAscii:
      return ONIG_ENCODING_ASCII;
    case kcode::eEUC:
      return ONIG_ENCODING_EUC_JP;
    case kcode::eSJIS:
      return ONIG_ENCODING_SJIS;
    case kcode::eUTF8:
      return ONIG_ENCODING_UTF8;
    }
  }

  int get_kcode_from_enc(OnigEncoding enc) {
    int r;

    r = KCODE_ASCII;
    if (enc == ONIG_ENCODING_ASCII)  r = KCODE_NONE;
    if (enc == ONIG_ENCODING_EUC_JP) r = KCODE_EUC;
    if (enc == ONIG_ENCODING_SJIS)   r = KCODE_SJIS;
    if (enc == ONIG_ENCODING_UTF8)   r = KCODE_UTF8;
    return r;
  }

  struct _gather_data {
    STATE;
    LookupTable* tbl;
  };

  static int _gather_names(const UChar *name, const UChar *name_end,
      int ngroup_num, int *group_nums, regex_t *reg, struct _gather_data *gd) {

    int gn;
    STATE;
    LookupTable* tbl = gd->tbl;

    state = gd->state;

    gn = group_nums[0];
    tbl->store(state, state->symbol((char*)name), Fixnum::from(gn - 1));
    return 0;
  }

  /*
   * Only initialize the object, not oniguruma.  This allows copying of the
   * regular expression via Regexp#initialize_copy
   */
  Regexp* Regexp::create(STATE) {
    Regexp* o_reg = state->new_object<Regexp>(G(regexp));

    o_reg->onig_data = NULL;
    o_reg->forced_encoding_ = false;

    return o_reg;
  }

  void Regexp::make_managed(STATE) {
    Regexp* obj = this;
    regex_t* reg = onig_data;

    assert(reg->chain == 0);

    ByteArray* reg_ba = ByteArray::create(state, sizeof(regex_t));
    memcpy(reg_ba->raw_bytes(), reg, sizeof(regex_t));

    regex_t* old_reg = reg;
    reg = reinterpret_cast<regex_t*>(reg_ba->raw_bytes());

    obj->onig_data = reg;
    write_barrier(state, reg_ba);

    if(reg->p) {
      ByteArray* pattern = ByteArray::create(state, reg->alloc);
      memcpy(pattern->raw_bytes(), reg->p, reg->alloc);

      reg->p = reinterpret_cast<unsigned char*>(pattern->raw_bytes());

      obj->write_barrier(state, pattern);
    }

    if(reg->exact) {
      int exact_size = reg->exact_end - reg->exact;
      ByteArray* exact = ByteArray::create(state, exact_size);
      memcpy(exact->raw_bytes(), reg->exact, exact_size);

      reg->exact = reinterpret_cast<unsigned char*>(exact->raw_bytes());
      reg->exact_end = reg->exact + exact_size;

      obj->write_barrier(state, exact);
    }

    int int_map_size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;

    if(reg->int_map) {
      ByteArray* intmap = ByteArray::create(state, int_map_size);
      memcpy(intmap->raw_bytes(), reg->int_map, int_map_size);

      reg->int_map = reinterpret_cast<int*>(intmap->raw_bytes());

      obj->write_barrier(state, intmap);
    }

    if(reg->int_map_backward) {
      ByteArray* intmap_back = ByteArray::create(state, int_map_size);
      memcpy(intmap_back->raw_bytes(), reg->int_map_backward, int_map_size);

      reg->int_map_backward = reinterpret_cast<int*>(intmap_back->raw_bytes());

      obj->write_barrier(state, intmap_back);
    }

    if(reg->repeat_range) {
      int rrange_size = sizeof(OnigRepeatRange) * reg->repeat_range_alloc;
      ByteArray* rrange = ByteArray::create(state, rrange_size);
      memcpy(rrange->raw_bytes(), reg->repeat_range, rrange_size);

      reg->repeat_range = reinterpret_cast<OnigRepeatRange*>(rrange->raw_bytes());

      obj->write_barrier(state, rrange);
    }

    onig_free(old_reg);
  }

  void Regexp::maybe_recompile(STATE) {
    const UChar *pat;
    const UChar *end;
    OnigEncoding enc;
    OnigErrorInfo err_info;
    int err;

    if(forced_encoding_) return;

    enc = current_encoding(state);
    if(enc == onig_data->enc) return;

    pat = (UChar*)source()->c_str();
    end = pat + source()->size();

    int options = onig_data->options;
    OnigEncoding orig_enc = onig_data->enc;

    err = onig_new(&this->onig_data, pat, end, options,
                   enc, ONIG_SYNTAX_RUBY, &err_info);

    // If it doesn't work out, then abort and reset the encoding back
    // and say that it's forced.
    if(err != ONIG_NORMAL) {

      err = onig_new(&this->onig_data, pat, end, options,
                     orig_enc, ONIG_SYNTAX_RUBY, &err_info);

      // Ok, wtf. Well, no way to proceed now.
      if(err != ONIG_NORMAL) {
        OnigUChar buf[1024];
        onig_error_code_to_str((UChar*)buf, err, &err_info);
        std::cout << "Fatal ONIG error: " << buf << "\n";
        assert(err == ONIG_NORMAL);
      }

      forced_encoding_ = true;
    }

    make_managed(state);
  }

  /*
   * This is a primitive so #initialize_copy can work.
   */
  Regexp* Regexp::initialize(STATE, String* pattern, Fixnum* options) {
    const UChar *pat;
    const UChar *end;
    OnigErrorInfo err_info;
    OnigOptionType opts;
    OnigEncoding enc;
    int err, num_names, kcode;

    pat = (UChar*)pattern->c_str();
    end = pat + pattern->size();

    opts  = options->to_native();
    kcode = opts & KCODE_MASK;
    opts &= OPTION_MASK;

    if(kcode == 0) {
      enc = current_encoding(state);
    } else {
      // Don't attempt to fix the encoding later, it's been specified by the
      // user.
      enc = get_enc_from_kcode(kcode);
      forced_encoding_ = true;
    }

    err = onig_new(&this->onig_data, pat, end, opts, enc, ONIG_SYNTAX_RUBY, &err_info);

    if(err != ONIG_NORMAL) {
      UChar onig_err_buf[ONIG_MAX_ERROR_MESSAGE_LEN];
      char err_buf[1024];
      onig_error_code_to_str(onig_err_buf, err, &err_info);
      snprintf(err_buf, 1024, "%s: %s", onig_err_buf, pat);

      Exception::regexp_error(state, err_buf);
      return 0;
    }

    this->source(state, pattern);

    num_names = onig_number_of_names(this->onig_data);

    if(num_names == 0) {
      this->names(state, nil<LookupTable>());
    } else {
      struct _gather_data gd;
      gd.state = state;
      LookupTable* tbl = LookupTable::create(state);
      gd.tbl = tbl;
      onig_foreach_name(this->onig_data, (int (*)(const OnigUChar*, const OnigUChar*,int,int*,OnigRegex,void*))_gather_names, (void*)&gd);
      this->names(state, tbl);
    }

    make_managed(state);

    return this;
  }

  // 'self' is passed in automatically by the primitive glue
  Regexp* Regexp::allocate(STATE, Object* self) {
    Regexp* re = Regexp::create(state);
    re->onig_data = 0;
    re->klass(state, (Class*)self);
    return re;
  }

  Fixnum* Regexp::options(STATE) {
    regex_t*       reg;

    reg    = onig_data;

    int result = ((int)onig_get_options(reg) & OPTION_MASK);

    if(forced_encoding_) {
      result |= get_kcode_from_enc(onig_get_encoding(reg));
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

  static Object* get_match_data(STATE, OnigRegion *region, String* string, Regexp* regexp, int pos) {
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

  Object* Regexp::match_region(STATE, String* string, Fixnum* start,
                               Fixnum* end, Object* forward)
  {
    int beg, max;
    const UChar *str;
    OnigRegion *region;
    Object* md;

    maybe_recompile(state);

    region = onig_region_new();

    max = string->size();
    str = (UChar*)string->c_str();

    int* back_match = onig_data->int_map_backward;

    if(!RTEST(forward)) {
      beg = onig_search(onig_data, str, str + max,
                        str + end->to_native(),
                        str + start->to_native(),
                        region, ONIG_OPTION_NONE);
    } else {
      beg = onig_search(onig_data, str, str + max,
                        str + start->to_native(),
                        str + end->to_native(),
                        region, ONIG_OPTION_NONE);
    }

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(onig_data->int_map_backward != back_match) {
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create(state, size);
      memcpy(ba->raw_bytes(), onig_data->int_map_backward, size);

      // Dispose of the old one.
      free(onig_data->int_map_backward);

      onig_data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }


    if(beg == ONIG_MISMATCH) {
      onig_region_free(region, 1);
      return Qnil;
    }

    md = get_match_data(state, region, string, this, 0);
    onig_region_free(region, 1);
    return md;
  }

  Object* Regexp::match_start(STATE, String* string, Fixnum* start) {
    int beg, max;
    const UChar *str;
    const UChar *fin;
    OnigRegion *region;
    Object* md = Qnil;

    maybe_recompile(state);
    region = onig_region_new();

    max = string->size();
    native_int pos = start->to_native();

    str = (UChar*)string->c_str();
    fin = str + max;

    str += pos;

    int* back_match = onig_data->int_map_backward;

    beg = onig_match(onig_data, str, fin, str, region,
                     ONIG_OPTION_NONE);

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(onig_data->int_map_backward != back_match) {
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create(state, size);
      memcpy(ba->raw_bytes(), onig_data->int_map_backward, size);

      // Dispose of the old one.
      free(onig_data->int_map_backward);

      onig_data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }

    if(beg != ONIG_MISMATCH) {
      md = get_match_data(state, region, string, this, pos);
    }

    onig_region_free(region, 1);
    return md;
  }

  Object* Regexp::search_from(STATE, String* string, Fixnum* start) {
    int beg, max;
    const UChar *str;
    const UChar *fin;
    OnigRegion *region;
    Object* md = Qnil;

    maybe_recompile(state);
    region = onig_region_new();

    max = string->size();
    native_int pos = start->to_native();

    str = (UChar*)string->c_str();
    fin = str + max;

    str += pos;

    int* back_match = onig_data->int_map_backward;

    beg = onig_search(onig_data, str, fin, str, fin,
                      region, ONIG_OPTION_NONE);

    // Seems like onig must setup int_map_backward lazily, so we have to watch
    // for it to appear here.
    if(onig_data->int_map_backward != back_match) {
      native_int size = sizeof(int) * ONIG_CHAR_TABLE_SIZE;
      ByteArray* ba = ByteArray::create(state, size);
      memcpy(ba->raw_bytes(), onig_data->int_map_backward, size);

      // Dispose of the old one.
      free(onig_data->int_map_backward);

      onig_data->int_map_backward = reinterpret_cast<int*>(ba->raw_bytes());

      write_barrier(state, ba);
    }

    if(beg != ONIG_MISMATCH) {
      md = get_match_data(state, region, string, this, pos);
    }

    onig_region_free(region, 1);
    return md;
  }

  String* MatchData::matched_string(STATE) {
    Fixnum* beg = try_as<Fixnum>(full_->at(state, 0));
    Fixnum* fin = try_as<Fixnum>(full_->at(state, 1));

    if(!beg || !fin ||
        fin->to_native() > source_->size() ||
        beg->to_native() < 0) {
      return String::create(state, 0, 0);
    }

    const char* str = source_->c_str();
    native_int sz = fin->to_native() - beg->to_native();

    return String::create(state, str + beg->to_native(), sz);
  }

  String* MatchData::pre_matched(STATE) {
    Fixnum* beg = try_as<Fixnum>(full_->at(state, 0));

    if(!beg || beg->to_native() <= 0) {
      return String::create(state, 0, 0);
    }

    const char* str = source_->c_str();
    native_int sz = beg->to_native();

    return String::create(state, str, sz);
  }

  String* MatchData::post_matched(STATE) {
    Fixnum* fin = try_as<Fixnum>(full_->at(state, 1));

    if(!fin || fin->to_native() >= source_->size()) {
      return String::create(state, 0, 0);
    }

    const char* str = source_->c_str();
    native_int sz = (native_int)source_->size() - fin->to_native();

    return String::create(state, str + fin->to_native(), sz);
  }

  Object* MatchData::nth_capture(STATE, native_int which) {
    if(region_->num_fields() <= which) return Qnil;

    Tuple* sub = try_as<Tuple>(region_->at(state, which));
    if(!sub) return Qnil;

    Fixnum* beg = try_as<Fixnum>(sub->at(state, 0));
    Fixnum* fin = try_as<Fixnum>(sub->at(state, 1));

    if(!beg || !fin ||
        fin->to_native() > source_->size() ||
        beg->to_native() < 0) {
      return Qnil;
    }

    const char* str = source_->c_str();
    native_int sz = fin->to_native() - beg->to_native();

    return String::create(state, str + beg->to_native(), sz);
  }

  Object* MatchData::last_capture(STATE) {
    if(region_->num_fields() == 0) return Qnil;
    return nth_capture(state, region_->num_fields() - 1);
  }

  Object* Regexp::last_match_result(STATE, Fixnum* mode, Fixnum* which,
                                    CallFrame* call_frame)
  {
    Object* current_match = call_frame->scope->last_match(state);

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
    return Qnil;
  }

  Object* Regexp::last_match(STATE, Arguments& args, CallFrame* call_frame) {
    MatchData* match = try_as<MatchData>(call_frame->scope->last_match(state));
    if(!match) return Qnil;

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
      parent->scope->set_last_match(state, obj);
    }

    return obj;
  }

  Object* Regexp::propagate_last_match(STATE, CallFrame* call_frame) {
    Object* obj = call_frame->scope->last_match(state);
    if(RTEST(obj)) {
      Regexp::set_last_match(state, obj, call_frame);
    }
    return obj;
  }

  Object* Regexp::set_block_last_match(STATE, CallFrame* call_frame) {
    Object* blk = call_frame->scope->block();
    MatchData* match = try_as<MatchData>(call_frame->scope->last_match(state));
    if(!match) return Qnil;

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
    regex_t* reg = reg_o->onig_data;

    if(!reg) return;

    ByteArray* reg_ba = ByteArray::from_body(reg);

    if(ByteArray* reg_tmp = force_as<ByteArray>(mark.call(reg_ba))) {
      reg_o->onig_data = reinterpret_cast<regex_t*>(reg_tmp->raw_bytes());
      mark.just_set(obj, reg_tmp);

      reg_ba = reg_tmp;
      reg = reg_o->onig_data;
    }

    if(reg->p) {
      ByteArray* ba = ByteArray::from_body(reg->p);

      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp) {
        reg->p = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }

    if(reg->exact) {
      int exact_size = reg->exact_end - reg->exact;
      ByteArray* ba = ByteArray::from_body(reg->exact);

      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp) {
        reg->exact = reinterpret_cast<unsigned char*>(tmp->raw_bytes());
        reg->exact_end = reg->exact + exact_size;
        mark.just_set(obj, tmp);
      }
    }

    if(reg->int_map) {
      ByteArray* ba = ByteArray::from_body(reg->int_map);

      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp) {
        reg->int_map = reinterpret_cast<int*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }

    if(reg->int_map_backward) {
      ByteArray* ba = ByteArray::from_body(reg->int_map_backward);

      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp) {
        reg->int_map_backward = reinterpret_cast<int*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }

    if(reg->repeat_range) {
      ByteArray* ba = ByteArray::from_body(reg->repeat_range);

      ByteArray* tmp = force_as<ByteArray>(mark.call(ba));
      if(tmp) {
        reg->repeat_range = reinterpret_cast<OnigRepeatRange*>(tmp->raw_bytes());
        mark.just_set(obj, tmp);
      }
    }
  }

  void Regexp::Info::visit(Object* obj, ObjectVisitor& visit) {
    auto_visit(obj, visit);

    Regexp* reg_o = force_as<Regexp>(obj);
    regex_t* reg = reg_o->onig_data;

    if(!reg) return;

    ByteArray* reg_ba = ByteArray::from_body(reg);
    visit.call(reg_ba);

    if(reg->p) {
      ByteArray* ba = ByteArray::from_body(reg->p);
      visit.call(ba);
    }

    if(reg->exact) {
      ByteArray* ba = ByteArray::from_body(reg->exact);
      visit.call(ba);
    }

    if(reg->int_map) {
      ByteArray* ba = ByteArray::from_body(reg->int_map);
      visit.call(ba);
    }

    if(reg->int_map_backward) {
      ByteArray* ba = ByteArray::from_body(reg->int_map_backward);
      visit.call(ba);
    }

    if(reg->repeat_range) {
      ByteArray* ba = ByteArray::from_body(reg->repeat_range);
      visit.call(ba);
    }
  }
}
