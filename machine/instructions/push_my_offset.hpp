  intptr_t index = next_int;
  Object* val = *reinterpret_cast<Object**>(
      reinterpret_cast<uintptr_t>(call_frame->self()) + index);
  stack_push(val);
