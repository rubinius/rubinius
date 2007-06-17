NODE* syd_compile_string(const char *f, VALUE s, int line, rb_parse_state *parse_state);
NODE* syd_compile_cstr(const char *f, const char *s, int len, int line);
int syd_compile_file_from_state(const char *f, VALUE file, int start, rb_parse_state *parse_state);
NODE *syd_compile_file(const char *f, VALUE file, int start);

void setup_parser();

