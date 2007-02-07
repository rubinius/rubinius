void regexp_init(STATE);
char *regexp_version(STATE);
OBJECT regexp_new(STATE, OBJECT pattern, OBJECT options);
void regexp_cleanup(STATE, OBJECT regexp);
OBJECT regexp_match(STATE, OBJECT regexp, OBJECT string);
OBJECT regexp_options(STATE, OBJECT regexp);
