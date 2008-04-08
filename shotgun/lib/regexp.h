void   regexp_cleanup      (STATE, OBJECT regexp);
void   regexp_init         (STATE);
OBJECT regexp_match        (STATE, OBJECT regexp, OBJECT string);
OBJECT regexp_match_region (STATE, OBJECT regexp, OBJECT string, OBJECT start);
OBJECT regexp_new          (STATE, OBJECT pattern, OBJECT options, char *err_buf);
OBJECT regexp_options      (STATE, OBJECT regexp);
OBJECT regexp_search_region(STATE, OBJECT regexp, OBJECT string, OBJECT start, OBJECT end, OBJECT forward);
char * regexp_version      (STATE);
