require 'bytecode/encoder'

class ShotgunInstructions
  
  def generate_switch(fd, op="op")
    i = 0 
    order = Bytecode::InstructionEncoder::OpCodes;
    fd.puts "switch(#{op}) {"
    order.each do |ins|
      code = send(ins) rescue nil
      if code
        fd.puts "   case #{i}: {"
        fd.puts code
        fd.puts "   break;\n    }"
      else
        STDERR.puts "Problem with opcode: #{ins}"
      end
      i += 1
    end
    fd.puts "default: printf(\"Invalid bytecode: %d\\n\", (int)op); abort();\n"
    fd.puts "}"
    fd.puts
  end
  
  def generate_declarations(fd)
    fd.puts "int _int, j, k;"
    fd.puts "OBJECT _lit, t1, t2, t3, t4;"
  end
  
  def generate_names
    order = Bytecode::InstructionEncoder::OpCodes;
    str = "char *instruction_names[] = {\n"
    order.each do |ins|
      str << "  #{ins.to_s.dump},"      
    end
    str << "};"
  end
  
  def noop
    ""
  end
  
  def push_int
    <<-CODE
    next_int;
    stack_push(I2N(_int));
    CODE
  end
  
  def meta_push_neg_1
    <<-CODE
    stack_push(I2N(-1));
    CODE
  end
  
  def meta_push_0
    <<-CODE
    stack_push(I2N(0));
    CODE
  end
  
  def meta_push_1
    <<-CODE
    stack_push(I2N(1));
    CODE
  end
  
  def meta_push_2
    <<-CODE
    stack_push(I2N(2));
    CODE
  end
  
  def push_nil
    "stack_push(Qnil);"
  end
  
  def push_true
    "stack_push(Qtrue);"
  end
  
  def push_false
    "stack_push(Qfalse);"
  end
  
  def push_context
    <<-CODE
    methctx_reference(state, c->active_context);
    stack_push(c->active_context);
    CODE
  end
  
  def push_literal
    "next_int; stack_push(tuple_at(state, c->literals, _int));"
  end
  
  def push_self
    "stack_push(c->self);"
  end
  
  def push_local
    "next_int; stack_push(tuple_at(state, c->locals, _int));"
  end
  
  def push_local_depth
    <<-CODE
    next_int;
    k = _int;
    next_int;
    t1 = c->active_context;
    for(j = 0; j < k; j++) {
      t2 = blokctx_get_env(t1);
      t1 = blokenv_get_home_block(t2);
    }
    stack_push(tuple_at(state, blokctx_get_locals(t1), _int));
    CODE
  end
  
  
  def push_exception
    "stack_push(c->exception);"
  end
  
  def clear_exception
    <<-CODE
    c->exception = Qnil;
    CODE
  end
  
  def push_block
    "stack_push(c->block);"
  end
  
  def push_ivar
    <<-CODE
    next_literal;
    stack_push(object_get_ivar(state, c->self, _lit));
    CODE
  end
  
  def allocate
    <<-CODE
    _lit = stack_pop();
    stack_push(NEW_OBJECT(Qnil, FIXNUM_TO_INT(_lit)));
    CODE
  end
  
  def set_class
    <<-CODE
    _lit = stack_pop();
    t1 = stack_pop();
    SET_CLASS(_lit, t1);
    stack_push(_lit);
    CODE
  end
  
  def store_field
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    SET_FIELD(t3, FIXNUM_TO_INT(t1), t2);
    stack_push(t3);
    CODE
  end
  
  def fetch_field
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    assert(REFERENCE_P(t2));
    stack_push(NTH_FIELD(t2, FIXNUM_TO_INT(t1)));
    CODE
  end
  
  def push_my_field
    <<-CODE
    next_int;
    assert(REFERENCE_P(c->self));
    stack_push(NTH_FIELD(c->self, _int));
    CODE
  end
  
  def store_my_field
    <<-CODE
    next_int;
    SET_FIELD(c->self, _int, stack_top());
    CODE
  end
  
  # cpu_perform_primitive(STATE, cpu c, int prim, OBJECT mo, int num_args)
  # Make primitives safer by having the opcode be aware of the number of args sent.
  # This way we can remove the dependency of primitives being embedded in methods.
  def send_primitive
    <<-CODE
    next_int;
    j = _int; // primitive index
    next_int;
    k = _int; // num_args

    // Should the OBJECT parameter be removed since a primitive is not necesarily
    // performed on an object? Or should we state that the semantics of a primitive 
    // will always have an object or else it needs to be an opcode... ?
    // If the primitive fails raise an exception
    if( ! cpu_perform_primitive(state, c, j, Qnil, k, Qnil, Qnil) )
    {
      cpu_raise_primitive_failure(state, c, j);
    }
    CODE
  end
  
  def goto
    <<-CODE
    next_int;
    c->ip = _int;
    cpu_cache_ip(c);
    CODE
  end
  
  def goto_if_false
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(!RTEST(t1)) {
      c->ip = _int;
      cpu_cache_ip(c);
    }
    CODE
  end
  
  def goto_if_true
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(RTEST(t1)) {
      c->ip = _int;
      cpu_cache_ip(c);
    }
    CODE
  end
  
  def goto_if_defined
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(t1 != Qundef) {
      c->ip = _int;
      cpu_cache_ip(c);
    }
    CODE
  end
  
  def swap_stack
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(t1);
    stack_push(t2);
    CODE
  end
  
  def dup_top
    <<-CODE
    stack_push(stack_top());
    CODE
  end
  
  def pop
    "stack_pop();"
  end
  
  def set_local
    <<-CODE
    next_int;
    t1 = stack_pop();
    // printf("Set local %d to %s\\n", _int, _inspect(t1));
    tuple_put(state, c->locals, _int, t1);
    stack_push(t1);
    CODE
  end
  
  def set_local_depth
    <<-CODE
    next_int;
    k = _int;
    next_int;
    t3 = stack_pop();
    t1 = c->active_context;
        
    for(j = 0; j < k; j++) {
      t2 = blokctx_get_env(t1);
      t1 = blokenv_get_home_block(t2);
    }
    tuple_put(state, blokctx_get_locals(t1), _int, t3);
    stack_push(t3);
    
    CODE
  end
  
  def make_array
    <<-CODE
    next_int;
    t1 = array_new(state, _int);
    j = _int - 1;
    for(; j >= 0; j--) {
      t2 = stack_pop();
      array_set(state, t1, j, t2);
    }
    
    cpu_perform_hook(state, c, BASIC_CLASS(array), 
                     state->global->sym_from_literal, t1);
    stack_push(t1);
    CODE
  end
  
  def push_array
    <<-CODE
    t1 = stack_pop();
    j = FIXNUM_TO_INT(array_get_total(t1)) - 1;
    for(; j >= 0; j--) {
      stack_push(array_get(state, t1, j));
    }
    CODE
  end
  
  def cast_array
    <<-CODE
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(state, t1, state->global->tuple)) {
      t1 = array_from_tuple(state, t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(state, t1, state->global->array)) {
      t2 = array_new(state, 1);
      array_set(state, t2, 0, t1);
      t1 = t2;
    }
    stack_push(t1);
    CODE
  end
  
  def cast_array_for_args
    <<-CODE
    next_int;
    c->args = _int;
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(state, t1, state->global->tuple)) {
      t1 = array_from_tuple(state, t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(state, t1, state->global->array)) {
      t2 = array_new(state, 1);
      array_set(state, t2, 0, t1);
      t1 = t2;
    }
    stack_push(t1);
    c->args += FIXNUM_TO_INT(array_get_total(t1));
    CODE
  end
  
  def cast_tuple
    <<-CODE
    t1 = stack_pop();
    if(REFERENCE_P(t1) && object_kind_of_p(state, t1, state->global->array)) {
      t1 = array_get_tuple(t1);
    } else if(!REFERENCE_P(t1) || !object_kind_of_p(state, t1, state->global->tuple)) {
      t2 = tuple_new(state, 1);
      tuple_put(state, t2, 0, t1);
      t1 = t2;
    }
    stack_push(t1);
    CODE
  end
  
  def make_hash
    <<-CODE
    next_int;
    t1 = hash_new(state);
    j = _int - 1;
    while(j >= 0) {
      t2 = stack_pop();
      t3 = stack_pop();
      hash_set(state, t1, t2, t3);
      j -= 2;
    }
    
    cpu_perform_hook(state, c, BASIC_CLASS(hash), 
                     state->global->sym_from_literal, t1);
    
    stack_push(t1);
    CODE
  end
  
  def set_ivar
    <<-CODE
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    t2 = stack_pop();
    object_set_ivar(state, c->self, t1, t2);
    stack_push(t2);
    CODE
  end
  
  def push_const
    <<-CODE
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_get(state, c, t1, c->enclosing_class));
    c->cache_index = -1;
    CODE
  end
  
  def find_const
    <<-CODE
    t1 = stack_pop();
    next_int;
    t2 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_get(state, c, t2, t1));
    c->cache_index = -1;
    CODE
  end
  
  def set_const
    <<-CODE
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_set(state, c, t1, stack_pop(), c->enclosing_class));
    CODE
  end
  
  def set_const_at
    <<-CODE
    next_int;
    t1 = tuple_at(state, c->literals, _int);
    t2 = stack_pop();
    t3 = stack_pop();
    cpu_const_set(state, c, t1, t2, t3);
    CODE
  end
  
  def push_cpath_top
    "stack_push(state->global->object);"
  end
  
  def set_encloser
    <<-CODE
    t1 = stack_pop();
    cpu_set_encloser_path(state, c, t1);
    stack_push(t1);
    CODE
  end
  
  def push_encloser
    <<-CODE
    cpu_push_encloser(state, c);
    CODE
  end
  
  def open_class_under
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(cpu_open_class(state, c, t2, t1));
    CODE
  end
  
  def open_class
    <<-CODE
    stack_push(cpu_open_class(state, c, c->enclosing_class, stack_pop()));
    CODE
  end
  
  def open_module_under
    <<-CODE
    stack_push(cpu_open_module(state, c, stack_pop()));
    CODE
  end
  
  def open_module
    <<-CODE
    stack_push(cpu_open_module(state, c, c->enclosing_class));
    CODE
  end

  def open_metaclass
    <<-CODE
    stack_push(object_metaclass(state, stack_pop()));
    CODE
  end
  
  def attach_method
    <<-CODE
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    cpu_attach_method(state, c, t1, _lit, t2);
    stack_push(t2);
    cpu_perform_hook(state, c, t1, state->global->sym_s_method_added, _lit);
    CODE
  end
  
  def add_method
    <<-CODE
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    cpu_add_method(state, c, t1, _lit, t2);
    stack_push(t2);
    cpu_perform_hook(state, c, t1, state->global->sym_method_added, _lit);
    CODE
  end
  
  def activate_method
    <<-CODE
    t1 = stack_pop(); /* recv */
    t2 = stack_pop(); /* self */ 
    j = FIXNUM_TO_INT(stack_pop()); /* sz */
    t3 = stack_pop(); /* locals */
    cpu_goto_method(state, c, t1, t2, j, cmethod_get_name(t2), stack_pop()); /* block */
    if(RTEST(t3)) {
      if(NUM_FIELDS(t3) >= NUM_FIELDS(c->locals)) {
        methctx_set_locals(c->active_context, t3);
        c->locals = t3;
      }
    }
    
    CODE
  end
  
  def send_method
    <<-CODE
    next_int;
    t1 = stack_pop();
    cpu_unified_send(state, c, t1, _int, 0, Qnil);
    CODE
  end
  
  def meta_send_stack_1
    <<-CODE
    next_int;
    cpu_unified_send(state, c, stack_pop(), _int, 1, Qnil);
    CODE
  end
  
  def meta_send_stack_2
    <<-CODE
    next_int;
    cpu_unified_send(state, c, stack_pop(), _int, 2, Qnil);
    CODE
  end
  
  def meta_send_stack_3
    <<-CODE
    next_int;
    cpu_unified_send(state, c, stack_pop(), _int, 3, Qnil);
    CODE
  end
  
  def meta_send_stack_4
    <<-CODE
    next_int;
    cpu_unified_send(state, c, stack_pop(), _int, 4, Qnil);
    CODE
  end
  
  def send_stack
    <<-CODE
    next_int;
    j = _int; // index
    next_int;
    k = _int; // num_args
    cpu_unified_send(state, c, stack_pop(), j, k, Qnil);
    CODE
  end
  
  def send_stack_with_block
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    next_int;
    j = _int;
    next_int;
    cpu_unified_send(state, c, t1, j, _int, t2);
    CODE
  end

  def send_with_arg_register
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    next_int;
    cpu_unified_send(state, c, t1, _int, c->args, t2);
    CODE
  end
  
  def send_super_stack_with_block
    <<-CODE
    t1 = stack_pop();
    next_int;
    j = _int;
    next_int;
    cpu_unified_send_super(state, c, c->self, j, _int, t1);
    CODE
  end
  
  def send_super_with_arg_register
    <<-CODE
    t1 = stack_pop();
    next_int;
    cpu_unified_send_super(state, c, c->self, _int, c->args, t1);
    CODE
  end
  
  def send_off_stack
    <<-CODE
    t3 = stack_pop();
    if(!SYMBOL_P(t3)) {
      if(RISA(t3, string)) {
        t3 = string_to_sym(state, t3);
      } else {
        t2 = stack_pop();
        t1 = stack_pop();
        stack_push(t3);
        cpu_send_method2(state, c, t1, state->global->sym_send, c->args + 1, t2);
        goto sos_done;
      }
    }
    t2 = stack_pop();
    t1 = stack_pop();
    cpu_send_method2(state, c, t1, t3, c->args, t2);
    sos_done:
    CODE
  end
  
  def locate_method
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(cpu_locate_method_on(state, c, t2, t1));
    CODE
  end
  
  def meta_send_op_plus
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(FIXNUM_P(t1) && FIXNUM_P(t2)) {
      stack_push(fixnum_add(state, t1, t2));
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_plus, 1);
    }
    CODE
  end
  
  def meta_send_op_minus
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(FIXNUM_P(t1) && FIXNUM_P(t2)) {
      stack_push(fixnum_sub(state, t1, t2));
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_minus, 1);
    }
    CODE
  end
  
  def meta_send_op_equal
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    // printf("opeqal %s == %s\\n", _inspect(t1), _inspect(t2));
    /* If both are fixnums, or one is a symbol, compare the ops directly. */
    if((FIXNUM_P(t1) && FIXNUM_P(t2)) || SYMBOL_P(t1) || SYMBOL_P(t2)) {
      stack_push((t1 == t2) ? Qtrue : Qfalse);
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_equal, 1);
    }
    CODE
  end
  
  def meta_send_op_nequal
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    /* If both are fixnums, or one is a symbol, compare the ops directly. */
    if((FIXNUM_P(t1) && FIXNUM_P(t2)) || SYMBOL_P(t1) || SYMBOL_P(t2)) {
      stack_push((t1 == t2) ? Qfalse : Qtrue);
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_nequal, 1);
    }
    CODE
  end

  # Exactly like equal, except calls === if it can't handle it directly.
  def meta_send_op_tequal
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    /* If both are fixnums, or one is a symbol, compare the ops directly. */
    if((FIXNUM_P(t1) && FIXNUM_P(t2)) || SYMBOL_P(t1) || SYMBOL_P(t2)) {
      stack_push((t1 == t2) ? Qtrue : Qfalse);
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_tequal, 1);
    }
    CODE
  end
  
  def meta_send_op_lt
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(FIXNUM_P(t1) && FIXNUM_P(t2)) {
      j = FIXNUM_TO_INT(t1);
      k = FIXNUM_TO_INT(t2);
      stack_push((j < k) ? Qtrue : Qfalse);
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_lt, 1);
    }
    CODE
  end
  
  def meta_send_op_gt
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(FIXNUM_P(t1) && FIXNUM_P(t2)) {
      j = FIXNUM_TO_INT(t1);
      k = FIXNUM_TO_INT(t2);
      stack_push((j > k) ? Qtrue : Qfalse);
    } else {
      stack_push(t2);
      cpu_send_method(state, c, t1, state->global->sym_gt, 1);
    }
    CODE
  end
  
  def soft_return
    "cpu_return_to_sender(state, c, FALSE);"
  end
  
  def caller_return
    <<-CODE
    t1 = c->active_context;
    c->active_context = c->sender;
    cpu_return_to_sender(state, c, TRUE);
    CODE
  end
  
  def raise_exc
    <<-CODE
    t1 = stack_pop();
    cpu_raise_exception(state, c, t1);
    CODE
  end
  
  def ret
    <<-CODE
    cpu_return_to_sender(state, c, TRUE);
    CODE
  end
  
  def block_break
    <<-CODE
    cpu_return_to_block_creator(state, c);
    CODE
  end
  
  
  # This actually shifts, not unshifts.
  def unshift_tuple
    <<-CODE
    t1 = stack_pop();
    if(NUM_FIELDS(t1) == 0) {
      stack_push(t1);
      stack_push(Qnil);
    } else {
      j = NUM_FIELDS(t1) - 1;
      t3 = tuple_at(state, t1, 0);
      t2 = tuple_new(state, j);
      object_copy_fields_from(state, t1, t2, 1, j);
      stack_push(t2);
      stack_push(t3);
    }
    CODE
  end
  
  def make_rest
    <<-CODE
    next_int;
    j = c->argcount - _int;
    t1 = array_new(state, j);
    for(k = 0; k < j; k++) {
      array_set(state, t1, k, stack_pop());
    }
    stack_push(t1);
    CODE
  end
  
  def check_argcount
    <<-CODE
    next_int;
    j = _int;
    next_int;
    
    if(c->argcount < (unsigned long int)j) {
      cpu_raise_arg_error(state, c, c->argcount, j);
    } else if(_int > 0 && c->argcount > (unsigned long int)_int) {
      cpu_raise_arg_error(state, c, c->argcount, _int);
    }
    CODE
  end
  
  def passed_arg
    <<-CODE
    next_int;
    if((unsigned long int)_int < c->argcount) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def string_append
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    string_append(state, t1, t2);
    stack_push(t1);
    CODE
  end
  
  def string_dup
    <<-CODE
    t1 = stack_pop();
    stack_push(string_dup(state, t1));
    CODE
  end
  
  def set_args
    <<-CODE
    t1 = stack_pop();
    assert(FIXNUM_P(t1));
    c->args = FIXNUM_TO_INT(t1);
    CODE
  end
  
  def get_args
    <<-CODE
    stack_push(I2N(c->args));
    CODE
  end
  
  def set_call_flags
    <<-CODE
    next_int;
    c->call_flags = _int;
    CODE
  end
  
  def set_cache_index
    <<-CODE
    next_int;
    c->cache_index = _int;
    CODE
  end
  
  def create_block
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    next_int;
    
    t4 = c->active_context;

    t3 =   Qnil;
    if(blokctx_s_block_context_p(state, t4)) {
      t3 = blokenv_get_home(blokctx_get_env(t4));
    } else {
      t3 = t4;
    }
    
    // GUARD(t3 == Qnil) would like to use this...
    if(t3 == Qnil) {
      // can this be put somewhere else?
      printf("Create block failed, %s!!\\n", _inspect(t4));
      abort();
    } else {
      cpu_flush_sp(c);
      cpu_flush_ip(c);
      j = c->ip + 5;
      t2 = blokenv_s_under_context(state, t3, t4, j, t1, t2, _int);
      stack_push(t2);
    }
    CODE
  end
  
  def kind_of
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(ISA(t1, t2)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def instance_of
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    if(object_logical_class(state, t1) == t2) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
end

si = ShotgunInstructions.new
si.generate_declarations(STDOUT)
si.generate_switch(STDOUT)

File.open("instruction_names.gen","w") do |f|
  f.puts si.generate_names
end
