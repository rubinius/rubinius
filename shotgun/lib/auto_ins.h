int _int, j, k;
OBJECT t1, t2, t3;
switch(op) {
   case 0: {

   break;
    }
   case 1: {
stack_push(Qnil);
   break;
    }
   case 2: {
stack_push(Qtrue);
   break;
    }
   case 3: {
stack_push(Qfalse);
   break;
    }
   case 4: {
    _lit = stack_pop();
    stack_push(cpu_new_blank(state, FIXNUM_TO_INT(_lit)));
   break;
    }
   case 5: {
    _lit = stack_pop();
    t1 = stack_pop();
    HEADER(_lit)->klass = t1;
    stack_push(_lit);
   break;
    }
   case 8: {
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    SET_FIELD(t3, FIXNUM_TO_INT(t1), t2);
    stack_push(t3);
   break;
    }
   case 9: {
    next_int;
    stack_push(I2N(_int));
   break;
    }
   case 10: {
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(NTH_FIELD(t2, FIXNUM_TO_INT(t1)));
   break;
    }
   case 11: {
    next_int;
    cpu_run_primitive(state, c, _int);
   break;
    }
   case 12: {
stack_push(c->active_context);
   break;
    }
   case 13: {
next_int; stack_push(tuple_at(state, c->literals, _int));
   break;
    }
   case 14: {
stack_push(c->self);
   break;
    }
   case 15: {
next_int; c->ip = _int;
   break;
    }
   case 16: {
    next_int;
    t1 = stack_pop();
    if(!RTEST(t1)) {
      c->ip = _int;
    }
   break;
    }
   case 17: {
    next_int;
    t1 = stack_pop();
    if(RTEST(t1)) {
      c->ip = _int;
    }
   break;
    }
   case 18: {
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(t1);
    stack_push(t2);
   break;
    }
   case 19: {
next_int; tuple_put(state, c->locals, _int, stack_pop());
   break;
    }
   case 20: {
next_int; stack_push(tuple_at(state, c->locals, _int));
   break;
    }
   case 21: {
stack_push(c->exception);
   break;
    }
   case 22: {
    next_int;
    t1 = array_new(state, _int);
    j = _int - 1;
    for(; j >= 0; j--) {
      t2 = stack_pop();
      array_set(state, t1, j, t2);
    }
    stack_push(t1);
   break;
    }
   case 23: {
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    t2 = stack_pop();
    object_set_ivar(state, c->self, t1, t2);
    stack_push(t2);
   break;
    }
   case 24: {
    next_literal;
    stack_push(object_get_ivar(state, c->self, _lit));
   break;
    }
   case 25: {
    next_int;
    t1 = stack_pop();
    if(t1 != Qundef) {
      c->ip = _int;
    }
   break;
    }
   case 26: {
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_get(state, c, t1));
   break;
    }
   case 27: {
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_set(state, c, t1, stack_pop()));
   break;
    }
   case 28: {
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    t2 = stack_pop();
    t3 = stack_pop();
    cpu_const_set_at(state, c, t1, t2, t3);
   break;
    }
   case 29: {
    t1 = stack_pop();
    next_int;
    t2 = tuple_at(state, c->literals, _int);
    stack_push(cpu_find_const(state, c, t1, t2));
   break;
    }
   case 30: {
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    cpu_attach_method(state, c, t1, _lit, t2);
    stack_push(t2);
   break;
    }
   case 31: {
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    cpu_add_method(state, c, t1, _lit, t2);
    stack_push(t2);
   break;
    }
   case 32: {
    stack_push(cpu_open_class(state, c, c->enclosing_class, stack_pop()));
   break;
    }
   case 33: {
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(cpu_open_class(state, c, t2, t1));
   break;
    }
   case 34: {
    stack_push(cpu_open_module(state, c, c->enclosing_class));
   break;
    }
   case 35: {
    stack_push(cpu_open_module(state, c, stack_pop()));
   break;
    }
   case 36: {
    t1 = stack_pop();
    if(NUM_FIELDS(t1) == 0) {
      stack_push(t1);
      stack_push(Qnil);
    } else {
      j = NUM_FIELDS(t1) - 1;
      t3 = tuple_at(state, t1, 0);
      t2 = tuple_new(state, j);
      object_copy_fields_from(state, tup, t2, 1, sz);
      stack_push(t2);
      stack_push(t3);
    }
   break;
    }
   case 37: {
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(t1, state->global->array)) {
      t1 = array_to_tuple(t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(t1, state->global->tuple)) {
      t2 = tuple_new(state, 1);
      tuple_put(state, t2, 0, t1);
      t1 = t2;
    }
    stack_push(t1);
   break;
    }
   case 38: {
    next_int;
    j = FIXNUM_TO_INT(methctx_get_argcount(c->active_context)) - _int;
    t1 = array_new(state, j);
    for(k = 0; k < j; k++) {
      array_set(state, t1, k, stack_pop());
    }
    stack_push(t1);
   break;
    }
   case 39: {
stack_push(stack_top());
   break;
    }
   case 40: {
stack_pop();
   break;
    }
   case 41: {
    cpu_return_to_sender(state, c, TRUE);
   break;
    }
   case 42: {
    next_int;
    j = _int;
    next_int;
    t1 = stack_pop();
    cpu_unified_send(state, c, t1, j, _int, Qnil);
   break;
    }
   case 43: {
    next_int;
    j = _int;
    next_int;
    cpu_unified_send(state, c, stack_pop(), j, _int, Qnil);
   break;
    }
   case 44: {
    t1 = stack_pop();
    t2 = stack_pop();
    next_int;
    j = _int;
    next_int;
    cpu_unified_send(state, c, t1, j, _int, t2);
   break;
    }
   case 45: {
stack_push(c->block);
   break;
    }
   case 46: {
c->exception = Qnil;
   break;
    }
   case 47: {
cpu_return_to_sender(state, c, FALES);
   break;
    }
   case 48: {
    t1 = c->active_context;
    c->active_context = methctx_get_sender(t1);
    cpu_return_to_sender(state, c, TRUE);
   break;
    }
   case 49: {
    t1 = stack_pop();
    j = array_total(state, t1) - 1;
    for(; j >= 0; j--) {
      stack_push(array_get(state, t1, j));
    }
   break;
    }
   case 50: {
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(t1, state->global->tuple)) {
      t1 = array_from_tuple(t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(t1, state->global->array)) {
      t2 = array_new(state, 1);
      array_set(state, t2, 0, t1);
      t1 = t2;
    }
    stack_push(t1);
   break;
    }
   case 51: {
    next_int;
    t1 = hash_new(state);
    j = _int - 1;
    while(j >= 0) {
      t2 = stack_pop();
      t3 = stack_pop();
      hash_set(state, t1, t2, t3);
      j -= 2;
    }
    
    stack_push(t1);
   break;
    }
   case 52: {
    t1 = stack_pop();
    cpu_raise_exception(t1);
   break;
    }
   case 53: {
    t1 = stack_pop();
    cpu_set_encloser_path(state, c, t1);
   break;
    }
   case 54: {
    stack_push(c->enclosing_class)
   break;
    }
   case 55: {
    t1 = stack_pop();
    t2 = stack_pop();
    j = FIXNUM_TO_INT(stack_pop());
    cpu_goto_method(state, c, t1, t2, j, cmethod_get_name(meth));
   break;
    }
   case 56: {
stack_push(state->global->object);
   break;
    }
   case 57: {
    next_int;
    j = _int;
    next_int;
    
    if(c->argcount < j) {
      cpu_raise_arg_error(state, c, c->argcount, j);
    } else if(_int > 0 && c->argcount > _int) {
      cpu_raise_arg_error(state, c, c->argcount, _int);
    }
   break;
    }
   case 58: {
    next_int;
    if(_int < c->argcount) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
   break;
    }
   case 59: {
    t1 = stack_pop();
    t2 = stack_pop();
    string_append(state, t1, t2);
    stack_push(t1);
   break;
    }
   case 60: {
    t1 = stack_pop();
    stack_push(string_dup(state, t1));
   break;
    }
   case 61: {
    t1 = stack_pop();
    c->args = FIXNUM_TO_INT(t1);
   break;
    }
   case 62: {
    stack_push(I2N(c->args));
   break;
    }
   case 63: {
    t1 = stack_pop();
    t2 = stack_pop();
    next_int;
    cpu_unified_send(state, c, t1, _int, c->args, t2);
   break;
    }
   case 64: {
    next_int;
    c->args = _int;
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(t1, state->global->tuple)) {
      t1 = array_from_tuple(t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(t1, state->global->array)) {
      t2 = array_new(state, 1);
      array_set(state, t2, 0, t1);
      t1 = t2;
    }
    c->args += array_total(state, t1);
   break;
    }
}
