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
    fd.puts "}"
    fd.puts
  end
  
  def generate_declarations(fd)
    fd.puts "int _int, j, k;"
    fd.puts "OBJECT _lit, t1, t2, t3;"
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
    HEADER(_lit)->klass = t1;
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
  
  def send_primitive
    <<-CODE
    next_int;
    cpu_perform_primitive(state, c, _int, Qnil, 0);
    CODE
  end
  
  def goto
    "next_int; c->ip = _int;"
  end
  
  def goto_if_false
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(!RTEST(t1)) {
      c->ip = _int;
    }
    CODE
  end
  
  def goto_if_true
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(RTEST(t1)) {
      c->ip = _int;
    }
    CODE
  end
  
  def goto_if_defined
    <<-CODE
    next_int;
    t1 = stack_pop();
    if(t1 != Qundef) {
      c->ip = _int;
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
    CODE
  end
  
  def find_const
    <<-CODE
    t1 = stack_pop();
    next_int;
    t2 = tuple_at(state, c->literals, _int);
    stack_push(cpu_const_get(state, c, t2, t1));
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
    CODE
  end
  
  def add_method
    <<-CODE
    next_literal;
    t1 = stack_pop();
    t2 = stack_pop();
    cpu_add_method(state, c, t1, _lit, t2);
    stack_push(t2);
    CODE
  end
  
  def activate_method
    <<-CODE
    t1 = stack_pop();
    t2 = stack_pop();
    j = FIXNUM_TO_INT(stack_pop());
    cpu_goto_method(state, c, t1, t2, j, cmethod_get_name(t2));
    CODE
  end
  
  def send_method
    <<-CODE
    next_int;
    t1 = stack_pop();
    cpu_unified_send(state, c, t1, _int, 0, Qnil);
    CODE
  end
  
  def send_stack
    <<-CODE
    next_int;
    j = _int;
    next_int;
    cpu_unified_send(state, c, stack_pop(), j, _int, Qnil);
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
  
  def soft_return
    "cpu_return_to_sender(state, c, FALSE);"
  end
  
  def caller_return
    <<-CODE
    t1 = c->active_context;
    c->active_context = methctx_get_sender(t1);
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
    j = FIXNUM_TO_INT(methctx_get_argcount(c->active_context)) - _int;
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
    
    if(c->argcount < j) {
      cpu_raise_arg_error(state, c, c->argcount, j);
    } else if(_int > 0 && c->argcount > _int) {
      cpu_raise_arg_error(state, c, c->argcount, _int);
    }
    CODE
  end
  
  def passed_arg
    <<-CODE
    next_int;
    if(_int < c->argcount) {
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
end

si = ShotgunInstructions.new
si.generate_declarations(STDOUT)
si.generate_switch(STDOUT)

File.open("instruction_names.gen","w") do |f|
  f.puts si.generate_names
end
