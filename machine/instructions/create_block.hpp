  intptr_t literal = next_int;
  Object* code_or_id = reinterpret_cast<Object*>(literal);
  CompiledCode* code = 0;

  if(!(code = try_as<CompiledCode>(code_or_id))) {
    code = CodeDB::load(state, as<String>(code_or_id));
    // TODO: instructions
    // store_literal(reinterpret_cast<opcode>(code));
  }

  Object* be = BlockEnvironment::under_call_frame(state, code, call_frame->machine_code);
  if(!be) return static_cast<intptr_t>(NULL);

  stack_push(be);
