#include "oniguruma.h"
#include "shotgun.h"
#include "tuple.h"
#include "string.h"
#include "symbol.h"
#include "hash.h"

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

#define REG(k) (*DATA_STRUCT(k, regex_t**))

void regexp_cleanup(STATE, OBJECT regexp) {
  onig_free(REG(regexp_get_data(regexp)));
}

void regexp_init(STATE) {
  onig_init();
  state_add_cleanup(state, state->global->regexp, regexp_cleanup);
}

char *regexp_version(STATE) {
  return (char*)onig_version();
}

struct _gather_data {
  STATE;
  OBJECT tup;
};

static int _gather_names(const UChar *name, const UChar *name_end,
    int ngroup_num, int *group_nums, regex_t *reg, struct _gather_data *gd) {
  
  int gn;
  STATE;
  OBJECT tup;
  
  state = gd->state;
  tup = gd->tup;
  
  gn = group_nums[0];
  hash_set(state, tup, symbol_from_cstr(state, (char*)name), I2N(gn - 1));
  return 0;
}

OnigEncoding get_enc_from_kcode(int kcode)
{
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


int get_kcode_from_enc(OnigEncoding enc)
{
  int r;

  r = KCODE_ASCII;
  if (enc == ONIG_ENCODING_ASCII)  r = KCODE_NONE;
  if (enc == ONIG_ENCODING_EUC_JP) r = KCODE_EUC;
  if (enc == ONIG_ENCODING_SJIS)   r = KCODE_SJIS;
  if (enc == ONIG_ENCODING_UTF8)   r = KCODE_UTF8;
  return r;
}

OBJECT regexp_new(STATE, OBJECT pattern, OBJECT options) {
  regex_t **reg;
  const UChar *pat;
  const UChar *end;
  OBJECT o_regdata, o_reg, o_names;
  OnigErrorInfo err_info;
  OnigOptionType opts;
  OnigEncoding enc;
  int err, num_names, kcode;
  
  pat = (UChar*)string_byte_address(state, pattern);
  end = pat + FIXNUM_TO_INT(string_get_bytes(pattern));

  /* Ug. What I hate about the onig API is that there is no way
     to define how to allocate the reg, onig_new does it for you.
     regex_t is a typedef for a pointer of the internal type.
     So for the time being a regexp object will just store the
     pointer to the real regex structure. */
     
  NEW_STRUCT(o_regdata, reg, BASIC_CLASS(regexpdata), regex_t*);

  opts  = FIXNUM_TO_INT(options);
  kcode = opts & KCODE_MASK;
  enc   = get_enc_from_kcode(kcode);
  opts &= OPTION_MASK;
  
  err = onig_new(reg, pat, end, 
      opts, enc, ONIG_SYNTAX_RUBY, &err_info); 
    
  /* FIXME: error detection! */
  if(err != ONIG_NORMAL) {
    return Qnil;
  }
  
  o_reg = regexp_allocate(state);
  regexp_set_source(o_reg, pattern);
  regexp_set_data(o_reg, o_regdata);
  num_names = onig_number_of_names(*reg);
  if(num_names == 0) {
    regexp_set_names(o_reg, Qnil);
  } else {
    struct _gather_data gd;
    gd.state = state;
    o_names = hash_new(state);
    gd.tup = o_names;
    onig_foreach_name(*reg, (int (*)(const OnigUChar*, const OnigUChar*,int,int*,OnigRegex,void*))_gather_names, (void*)&gd);
    regexp_set_names(o_reg, o_names);
  }
  return o_reg;
}

OBJECT regexp_options(STATE, OBJECT regexp)
{
  OnigEncoding   enc;
  OnigOptionType option;
  regex_t*       reg;

  reg    = REG(regexp_get_data(regexp));
  option = onig_get_options(reg);
  enc    = onig_get_encoding(reg);

  return I2N((int)(option & OPTION_MASK) | get_kcode_from_enc(enc));
}

OBJECT _md_region_to_tuple(STATE, OnigRegion *region, int max) {
  int i;
  OBJECT tup, sub;
  tup = tuple_new(state, region->num_regs - 1);
  for(i = 1; i < region->num_regs; i++) {
    sub = tuple_new2(state, 2, I2N(region->beg[i]), I2N(region->end[i]));
    tuple_put(state, tup, i - 1, sub);
  }
  return tup;
}

OBJECT regexp_match(STATE, OBJECT regexp, OBJECT string) {
  int err, max;
  const UChar *str, *end, *start, *range;
  OnigRegion *region;
  regex_t *reg;
  OBJECT md;
  
  region = onig_region_new();
  
  max = FIXNUM_TO_INT(string_get_bytes(string));
  str = (UChar*)string_byte_address(state, string);
  end = str + max;
  start = str;
  range = end;
  
  reg = REG(regexp_get_data(regexp));
  
  err = onig_search(reg, str, end, start, range, region, ONIG_OPTION_NONE);
  
  if(err == ONIG_MISMATCH) {
    onig_region_free(region, 1);
    return Qnil;
  }
  
  md = matchdata_allocate(state);
  matchdata_set_source(md, string);
  matchdata_set_regexp(md, regexp);
  matchdata_set_full(md, tuple_new2(state, 2, I2N(region->beg[0]), I2N(region->end[0])));
  matchdata_set_region(md, _md_region_to_tuple(state, region, max));
  onig_region_free(region, 1);
  return md;
}
