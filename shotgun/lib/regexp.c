#include "oniguruma.h"
#include "shotgun.h"
#include "tuple.h"
#include "string.h"

void regexp_init(STATE) {
  onig_init();
}

char *regexp_version(STATE) {
  return (char*)onig_version();
}

#define REG(k) (*DATA_STRUCT(k, regex_t**))

struct _gather_data {
  STATE;
  OBJECT tup;
};

static int _gather_names(const UChar *name, const UChar *name_end,
    int ngroup_num, int *group_nums, regex_t *reg, struct _gather_data *gd) {
  
  int i, gn;
  STATE;
  OBJECT tup;
  
  state = gd->state;
  tup = gd->tup;
  
  for(i = 0; i < ngroup_num; i++) {
    gn = group_nums[i];
    tuple_put(state, tup, i, 
      tuple_new2(state, 2, I2N(gn), string_new(state, (char*)name)));
  }
  return 0;
}

OBJECT regexp_new(STATE, OBJECT pattern, OBJECT options) {
  regex_t **reg;
  const UChar *pat;
  const UChar *end;
  OBJECT o_regdata, o_reg, o_names;
  OnigErrorInfo err_info;
  OnigOptionType opts;
  int err, num_names;
  
  pat = (UChar*)string_byte_address(state, pattern);
  end = pat + FIXNUM_TO_INT(string_get_bytes(pattern));

  /* Ug. What I hate about the onig API is that there is no way
     to define how to allocate the reg, onig_new does it for you.
     regex_t is a typedef for a pointer of the internal type.
     So for the time being a regexp object will just store the
     pointer to the real regex structure. */
     
  NEW_STRUCT(o_regdata, reg, BASIC_CLASS(regexpdata), regex_t*);
  
  opts = ONIG_OPTION_NONE;
  if(options == Qtrue) {
    opts |= ONIG_OPTION_MULTILINE;
  }
  
  err = onig_new(reg, pat, end, 
      opts, ONIG_ENCODING_ASCII, ONIG_SYNTAX_RUBY, &err_info); 
    
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
    o_names = tuple_new(state, num_names);
    gd.tup = o_names;
    onig_foreach_name(*reg, _gather_names, (void*)&gd);
    regexp_set_names(o_reg, o_names);
  }
  return o_reg;
}

void regexp_cleanup(STATE, OBJECT regexp) {
  onig_free(REG(regexp_get_data(regexp)));
}

OBJECT _md_region_to_tuple(STATE, OnigRegion *region, int max) {
  int i, j;
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
