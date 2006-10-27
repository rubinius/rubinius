OBJECT methctx_new(STATE, int cnt);
void methctx_init_registers(STATE, OBJECT self);
void methctx_init_locals(STATE, OBJECT self, int cnt);
OBJECT methctx_s_from_method(STATE, OBJECT meth, OBJECT from);
OBJECT blokctx_s_under_context(STATE, OBJECT ctx);
int blokctx_s_block_context_p(STATE, OBJECT ctx);
int methctx_s_was_referenced_p(STATE, OBJECT obj);
void methctx_reference(STATE, OBJECT self);
void methctx_s_reuse(STATE, OBJECT self, OBJECT meth, OBJECT from);
