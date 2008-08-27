#include "oniguruma.h" // Must be first.

#include "builtin/regexp.hpp"
#include "builtin/class.hpp"
#include "builtin/integer.hpp"
#include "builtin/lookuptable.hpp"
#include "builtin/string.hpp"
#include "builtin/symbol.hpp"
#include "builtin/tuple.hpp"

#include "vm.hpp"
#include "objectmemory.hpp"

#define OPTION_IGNORECASE ONIG_OPTION_IGNORECASE
#define OPTION_EXTENDED   ONIG_OPTION_EXTEND
#define OPTION_MULTILINE  ONIG_OPTION_MULTILINE
#define OPTION_MASK       (OPTION_IGNORECASE|OPTION_EXTENDED|OPTION_MULTILINE)

#define KCODE_ASCII       0
#define KCODE_NONE        16
#define KCODE_EUC         32
#define KCODE_SJIS        48
#define KCODE_UTF8        64
#define KCODE_MASK        (KCODE_EUC|KCODE_SJIS|KCODE_UTF8)

#define REG(k) (*((regex_t**)((k)->bytes)))

#define BASIC_CLASS(blah) G(blah)
#define NEW_STRUCT(obj, str, kls, kind) \
  obj = (typeof(obj))state->new_struct(kls, sizeof(kind)); \
  str = (kind *)(obj->bytes)
#define DATA_STRUCT(obj, type) ((type)(obj->bytes))

namespace rubinius {

  void Regexp::Info::cleanup(OBJECT regexp) {
    onig_free(as<Regexp>(regexp)->onig_data);
  }

  void Regexp::init(STATE) {
    onig_init();
    GO(regexp).set(state->new_class("Regexp", G(object), 0));
    G(regexp)->set_object_type(RegexpType);

    GO(matchdata).set(state->new_class("MatchData", G(object), 0));
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
    tbl->store(state, state->symbol((char*)name), Integer::from(state, gn - 1));
    return 0;
  }

  Regexp* Regexp::create(STATE, String* pattern, INTEGER options, char *err_buf) {
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
    enc   = get_enc_from_kcode(kcode);
    opts &= OPTION_MASK;

    Regexp* o_reg = (Regexp*)state->om->new_object(G(regexp), Regexp::fields);
    err = onig_new(&o_reg->onig_data, pat, end, opts, enc, ONIG_SYNTAX_RUBY, &err_info); 

    if(err != ONIG_NORMAL) {
      UChar onig_err_buf[ONIG_MAX_ERROR_MESSAGE_LEN];
      onig_error_code_to_str(onig_err_buf, err, &err_info);
      snprintf(err_buf, 1024, "%s: %s", onig_err_buf, pat);
      return (Regexp*)Qnil;
    }

    SET(o_reg, source, pattern);

    num_names = onig_number_of_names(o_reg->onig_data);
    if(num_names == 0) {
      SET(o_reg, names, Qnil);
    } else {
      struct _gather_data gd;
      gd.state = state;
      LookupTable* tbl = LookupTable::create(state);
      gd.tbl = tbl;
      onig_foreach_name(o_reg->onig_data, (int (*)(const OnigUChar*, const OnigUChar*,int,int*,OnigRegex,void*))_gather_names, (void*)&gd);
      SET(o_reg, names, tbl);
    }

    return o_reg;
  }

  // 'self' is passed in automatically by the primitive glue
  Regexp* Regexp::new_expression(STATE, OBJECT self, String* pattern, INTEGER options) {
    Regexp* re = Regexp::create(state, pattern, options);
    SET(re, klass, self);
    return re;
  }

  OBJECT Regexp::options(STATE) {
    OnigEncoding   enc;
    OnigOptionType option;
    regex_t*       reg;

    reg    = onig_data;
    option = onig_get_options(reg);
    enc    = onig_get_encoding(reg);

    return Integer::from(state, ((int)(option & OPTION_MASK) | get_kcode_from_enc(enc)));
  }

  static OBJECT _md_region_to_tuple(STATE, OnigRegion *region, int max) {
    int i;
    Tuple* sub;
    Tuple* tup = Tuple::create(state, region->num_regs - 1);
    for(i = 1; i < region->num_regs; i++) {
      sub = Tuple::create(state, 2);
      sub->put(state, 0, Integer::from(state, region->beg[i]));
      sub->put(state, 1, Integer::from(state, region->end[i]));
      tup->put(state, i - 1, sub);
    }
    return tup;
  }

  static OBJECT get_match_data(STATE, OnigRegion *region, String* string, Regexp* regexp, int max) {
    MatchData* md = (MatchData*)state->om->new_object(G(matchdata), MatchData::fields);
    SET(md, source, string->string_dup(state));
    SET(md, regexp, regexp);
    Tuple* tup = Tuple::create(state, 2);
    tup->put(state, 0, Integer::from(state, region->beg[0]));
    tup->put(state, 1, Integer::from(state, region->end[0]));
    
    SET(md, full, tup);
    SET(md, region, _md_region_to_tuple(state, region, max));
    return md;
  }

  OBJECT Regexp::match_region(STATE, String* string, INTEGER start, INTEGER end, OBJECT forward) {
    int beg, max;
    const UChar *str;
    OnigRegion *region;
    OBJECT md;

    region = onig_region_new();

    max = string->size();
    str = (UChar*)string->c_str();

    if(!RTEST(forward)) {
      beg = onig_search(onig_data, str, str + max, str + end->to_native(), str + start->to_native(), region, ONIG_OPTION_NONE);
    } else {
      beg = onig_search(onig_data, str, str + max, str + start->to_native(), str + end->to_native(), region, ONIG_OPTION_NONE);
    }

    if(beg == ONIG_MISMATCH) {
      onig_region_free(region, 1);
      return Qnil;
    }

    md = get_match_data(state, region, string, this, max);
    onig_region_free(region, 1);
    return md;
  }

  OBJECT Regexp::match_start(STATE, String* string, INTEGER start) {
    int beg, max;
    const UChar *str;
    OnigRegion *region;
    OBJECT md = Qnil;

    region = onig_region_new();

    max = string->size();
    str = (UChar*)string->c_str();

    beg = onig_match(onig_data, str, str + max, str + start->to_native(), region,
                     ONIG_OPTION_NONE);

    if(beg != ONIG_MISMATCH) {
      md = get_match_data(state, region, string, this, max);
    }

    onig_region_free(region, 1);
    return md;
  }
}
