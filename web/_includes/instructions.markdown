<h3><a class="instruction" name="noop">noop()</a></h3>

   The classic no-op operator. It performs no actions and does not modify the
   stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#pop">pop</a></li>
</ul>
<h3><a class="instruction" name="push_nil">push_nil()</a></h3>

   The special object `nil` is pushed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>nil</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_true">push_true()</a></h3>

   The special value `true` is pushed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>true</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_false">push_false()</a></h3>

   The special object `false` is pushed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>false</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_int">push_int(number)</a></h3>

   Pushes the value of the integer literal onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>number</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Certain common cases (i.e. -1, 0, 1, and 2) are optimised to avoid the
   decoding of the argument.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#meta_push_0">meta_push_0</a></li>
<li><a href="#meta_push_1">meta_push_1</a></li>
<li><a href="#meta_push_2">meta_push_2</a></li>
<li><a href="#meta_push_neg_1">meta_push_neg_1</a></li>
</ul>
<h3><a class="instruction" name="push_self">push_self()</a></h3>

   The current `self` object is pushed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>self</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="set_literal">set_literal(literal)</a></h3>

   Used to set the value of a literal. The stack top is set to the literal
   indicated by the operand.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Unlike other literals such as strings and numbers, creating a Regexp
   literal (i.e. via the /regex/ syntax) is a two step process to create the
   literal slot for the Regexp, create a literal for the string between the
   '/' delimiters and create a new Regexp object passing it the string.  Only
   then can the literal value be set, using the set_literal opcode.

<h3><a class="instruction" name="push_literal">push_literal(literal)</a></h3>

   The value identified by the operand _literal_ in the current state
   literals tuple is retrieved and placed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>literal</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   The literals tuple is part of the machine state, and holds all literal
   objects defined or used within a particular scope.

<h3><a class="instruction" name="goto">goto(location)</a></h3>

   Unconditionally moves the instruction pointer to the position specified by
   _location_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   All goto instructions use absolute addressing. This is absolute movement
   rather than a relative one, so the operand must specify the ip starting
   from 0 to move to.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#goto_if_true">goto_if_true</a></li>
<li><a href="#goto_if_false">goto_if_false</a></li>
</ul>
<h3><a class="instruction" name="goto_if_false">goto_if_false(location)</a></h3>

   Remove the top value on the stack, and if `nil` or `false`, set the
   instruction pointer to the value specified by _location_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#goto">goto</a></li>
<li><a href="#goto_if_true">goto_if_true</a></li>
</ul>
<h3><a class="instruction" name="goto_if_true">goto_if_true(location)</a></h3>

   Remove the top value on the stack, and if not `nil` or `false`, set the
   instruction pointer to the value specified by _location_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#goto">goto</a></li>
<li><a href="#goto_if_false">goto_if_false</a></li>
</ul>
<h3><a class="instruction" name="ret">ret()</a></h3>

   Return a value to the direct caller

   Pops the top value from the stack, and returns to the direct caller of the
   current invocation.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   In a method, the `return` keyword uses this instruction. In a block
   though, `return` uses the raise_return instruction and `next` uses this
   instruction.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#raise_return">raise_return</a></li>
<li><a href="#raise_exc">raise_exc</a></li>
</ul>
<h3><a class="instruction" name="swap_stack">swap_stack()</a></h3>

   Swaps the top two values on the stack, so that the second value becomes
   the first, and the first value becomes the second.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>s0</td><td>s1</td></tr>
<tr><td>s1</td><td>s0</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="dup_top">dup_top()</a></h3>

   Read a value from the top of the stack and push it on the stack again
   without removing the original value.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>s0</td><td>s0</td></tr>
<tr><td>...</td><td>s0</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="dup_many">dup_many(count)</a></h3>

   Duplicate multiple values on the stack

   Read _count_ values from the stack and push them onto the stack again
   in order without removing the original values.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   value1
</td><td>   value1
</td></tr>
<tr><td>   value2
</td><td>   value2
</td></tr>
<tr><td>   ...
</td><td>   value1
</td></tr>
<tr><td></td><td>   value2
</td></tr>
<tr><td></td><td>   ...
</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="pop">pop()</a></h3>

   Removes the top value from the stack, discarding it.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

#### Notes
   Pop is typically used when the return value of another opcode is not
   required.

<h3><a class="instruction" name="pop_many">pop_many(count)</a></h3>

   Removes _count_ values from the stack and discard them.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   value1
</td><td>   ...
</td></tr>
<tr><td>   value2
</td><td></td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="rotate">rotate(count)</a></h3>

   Reverses the order on the stack of the top _count_ values.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   obj1
</td><td>   obj3
</td></tr>
<tr><td>   obj2
</td><td>   obj2
</td></tr>
<tr><td>   obj3
</td><td>   obj1
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="move_down">move_down(positions)</a></h3>

   The top value on the stack is moved down by the specified number of
   _positions_, with all values above that position shuffling up by one.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   obj1
</td><td>   obj2
</td></tr>
<tr><td>   obj2
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td>   objn
</td></tr>
<tr><td>   objn
</td><td>   obj1
</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="set_local">set_local(local)</a></h3>

   Read the top of the stack and set the local variable identified by operand
   _local_ to it. The stack is not modified by this instruction.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_local">push_local</a></li>
</ul>
<h3><a class="instruction" name="push_local">push_local(local)</a></h3>

   Retrieves the current value of the local variable referenced by operand
   _local_ and push it onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#set_local">set_local</a></li>
</ul>
<h3><a class="instruction" name="push_local_depth">push_local_depth(depth, index)</a></h3>

   Pushes the value of a local from an enclosing scope onto the stack

   Retrieves the value of a local variable. Operand _depth_ indicates how many
   upward enclosing scopes to walk up and then operand _index_ indicates which
   local in that context to read. The value is then pushed on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Example
     k = 0
     foo.each do |i|
       bar.each do |j|
         # i is a local variable from enclosing scope at depth 1
         # k is a local variable from enclosing scope at depth 2
         i = i + j + k
       end
     end


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#set_local_depth">set_local_depth</a></li>
</ul>
<h3><a class="instruction" name="set_local_depth">set_local_depth(depth, index)</a></h3>

   Updates the value of a local variable contained in an enclosing scope

   Read a value from the top of thes stack and use it to update a local
   variable in an enclosing scope. The _depth_ and _index_ operands
   indentify the specific local the same as in `push_local_depth`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Example
     foo.each do |i|
       bar.each do |j|
         i = i + j  # i is a local variable from enclosing scope at depth 1
       end
     end


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_local_depth">push_local_depth</a></li>
</ul>
<h3><a class="instruction" name="passed_arg">passed_arg(index)</a></h3>

   Checks if the argument specified by the operand _index_ was passed to
   the current invocation. If so, push true, otherwise false.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>boolean</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Arguments are specified via a zero-index, so the first argument is 0.

<h3><a class="instruction" name="push_current_exception">push_current_exception()</a></h3>

   Pushes the current exception onto the stack, so that it can be used for
   some purpose, such as checking the exception type, setting an exception
   variable in a rescue clause, etc.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>exception</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Example
     begin
       foo = BAR        # BAR is not defined
     rescue NameError   # push_exception used to check type of exception (via ===)
       puts "No BAR"
     end


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#raise_exc">raise_exc</a></li>
</ul>
<h3><a class="instruction" name="clear_exception">clear_exception()</a></h3>

   Clears any exceptions from the current thread.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_exception_state">push_exception_state()</a></h3>

   Package up the current exception state into an object and push it. This
   is used to preserve the exception state around code that might mutate it.
   For instance, when handling an ensure while an exception is being raised


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>exc_state</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="restore_exception_state">restore_exception_state()</a></h3>

   Pops a value off the stack and set the threads exception state from it.
   This instruction is only to be used with a value pushed on the stack
   by the push_exception_state instruction.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>exception</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_exception_state">push_exception_state</a></li>
</ul>
<h3><a class="instruction" name="raise_exc">raise_exc()</a></h3>

   Raises an exception

   Pops a value off the stack and make it the current exception.
   If the value is not an instance of Exception, a TypeError is raised.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="setup_unwind">setup_unwind(ip, type)</a></h3>

   Register an unwind handler

   Registers what to happen when an exception wants to unwind through the
   current invocation. Operand _ip_ specifies where to set the instruction
   pointer if used. Operand _type_ is either 0 for if the value should be
   used in rescue style (not run when unwinding because of a return caused by
   `raise_return`) or 1 for ensure style (always used). The registrations are
   nested within the current invocation and are automatically removed from
   the registry when they are used. The `pop_unwind` instruction can be used
   to remove an unused registration.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   The registration also contains the value of the stack depth when
   created. If the registration is used, then the stack depth is
   restored to the value contained in the registration


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#pop_unwind">pop_unwind</a></li>
</ul>
<h3><a class="instruction" name="pop_unwind">pop_unwind()</a></h3>

   Remove the next unused unwind registration from the current invocation.
   This instruction is paired with `setup_unwind` to remove registrations
   when control exits a section of code that registered a handler but didn't
   use it. For example, exiting a begin that had a rescue expression.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#setup_unwind">setup_unwind</a></li>
</ul>
<h3><a class="instruction" name="raise_return">raise_return()</a></h3>

   Cause the toplevel enclosing scope to return

   Only used in a block, pop a value from the stack and raise a special
   internal exception and begin unwinding the stack. The toplevel method
   scope will rescue the exception and return the value.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#ret">ret</a></li>
</ul>
<h3><a class="instruction" name="ensure_return">ensure_return()</a></h3>

   Return from a scope but run ensures first

   A one use instruction, used only in a method toplevel within a begin
   that has an ensure. Use the same internal exception as `raise_return`
   which will coax the ensure registration to run.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#ret">ret</a></li>
<li><a href="#raise_return">raise_return</a></li>
</ul>
<h3><a class="instruction" name="raise_break">raise_break()</a></h3>

   Cause the method that yielded the current block to return. Used to
   implement the `break` keyword in a block.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="reraise">reraise()</a></h3>

   Continue unwinding the stack with the current exception. Verify that there
   is a current exception, then begin the unwinding process again.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="make_array">make_array(count)</a></h3>

   Create an array and populate with values on the stack

   Creates a new array, populating its contents by remove the number of
   values specidied by operand _count_ and putting them into the array in the
   order they were on the stack. The resulting array is pushed onto the
   stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   valueN
</td><td>   [value1, value2, ..., valueN]
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   value2
</td><td></td></tr>
<tr><td>   value1
</td><td></td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="cast_array">cast_array()</a></h3>

   Removes the object on the top of the stack, and:

   1. If the input is a tuple, a new array object is created based on the
      tuple data.
   1. If the input is an array, it is unmodified.

   If the input is any other type, call `Array.coerce_into_array(value)`.
   If the return value of the method call is an `Array`, make it the result.
   Otherwise make the result an 1 element `Array` contain the original value.

   The resulting array is then pushed back onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>array</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="shift_array">shift_array()</a></h3>

   Pops an array off the top of the stack. If the array is empty, it is
   pushed back onto the stack, followed by `nil`.

   Otherwise, the array is shifted, then pushed back onto the stack,
   followed by the object that was shifted from the front of the array.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   [value1, value2, ..., valueN]
</td><td>   value1
</td></tr>
<tr><td>   ...
</td><td>   [value2, ..., valueN]
</td></tr>
<tr><td></td><td>   ...
</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="set_ivar">set_ivar(index)</a></h3>

   Pops a value off the stack, and uses it to set the value of the instance
   variable identifies by the literal specified by operand _index_.  The
   value popped off the stack is then pushed back on again.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_ivar">push_ivar(index)</a></h3>

   Pushes the instance variable identified by _index_ onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_const">push_const(literal)</a></h3>

   Locates the constant indicated by the operand _literal_ from the current
   context, and pushes it onto the stack. If the constant cannot be found in
   the current context, nothing is pushed onto the stack, and a NameError
   exception is raised.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>constant</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Example
     engine = RUBY_ENGINE # RUBY_ENGINE is a constant defined by Rubinius

<h3><a class="instruction" name="set_const">set_const(index)</a></h3>

   Pops an object off the stack, and uses value to set a constant named
   by the literal _index_. The value is pushed back onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="set_const_at">set_const_at(index)</a></h3>

   Pop a value from the literals table specified by the operand _index_ and
   use it as the value of a constant named inside a Module object popped from
   the stack.  The _value_ is pushed back on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   value
</td><td>   value
</td></tr>
<tr><td>   module
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="find_const">find_const(index)</a></h3>

   Pops _module_ off the stack, and searches within its namespace for the
   constant named by the literal specified by the operand _index_. If found,
   it is pushed onto the stack; otherwise, nothing is pushed onto the stack,
   and a `NameError` exception is raised.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>module</td><td>constant</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Example
     str = "abc"
     enum = Enumerable::Enumerator(str, :each_byte)

<h3><a class="instruction" name="push_cpath_top">push_cpath_top()</a></h3>

   Pushes the top-level global object that represents the top-level namespace
   for constants. Used to find constants relative to the toplevel. In Ruby,
   this is the class `Object`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>constant</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_const_fast">push_const_fast(literal, association)</a></h3>

   Pushes a constant onto the stack. Caches the lookup to provide faster
   future lookup. This instruction is normally emitted only by the Generator.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>constant</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Example
     engine = RUBY_ENGINE # RUBY_ENGINE is a constant defined by Rubinius


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_const">push_const</a></li>
</ul>
<h3><a class="instruction" name="set_call_flags">set_call_flags(flags)</a></h3>

   The call flags on the current execution context are set to the opcode
   argument _flags_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Currently this only has one use, which is that send_stack_with_splat
   checks if flags is set to CALL_FLAG_CONCAT which indicase that
   the splat represents arguments at the beginning rather than the end.

<h3><a class="instruction" name="allow_private">allow_private()</a></h3>

   Indicate that the next send is allowed to see `private` methods.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="send_method">send_method(literal)</a></h3>

   Pops a _receiver_ object off the top of the stack and sends it the
   message specified by the operand _literal_ with zero arguments.

   When the method returns, the return value is pushed on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>receiver</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This form of send is for methods that take no arguments.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack">send_stack</a></li>
</ul>
<h3><a class="instruction" name="send_stack">send_stack(literal, count)</a></h3>

   Sends a message with arguments on the stack

   Pops the _receiver_ of the message off the stack and sends the message
   specified by the operand _literal_ with _count_ arguments. The arguments
   are removed from the stack also.

   When the method returns, the return value is pushed on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   value
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   receiver
</td><td></td></tr>
</tbody>
</table>

#### Notes
   This opcode does not pass a block to the receiver; see
   `send_stack_with_block` for the equivalent op code used when a block is to
   be passed.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack_with_block">send_stack_with_block</a></li>
</ul>
<h3><a class="instruction" name="send_stack_with_block">send_stack_with_block(literal, count)</a></h3>

   Sends a message with arguments and a block on the stack

   Pops the _receiver_ of the message off the stack and sends the message
   specified by the operand _literal_ with _count_ arguments. The arguments
   are removed from the stack also. A value that represents the block to pass
   on is popped off the stack after the normal arguments.

   When the method returns, the return value will be on top of the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   block
</td><td>   retval
</td></tr>
<tr><td>   argN
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   receiver
</td><td></td></tr>
</tbody>
</table>

#### Notes
   This opcode passes a block to the receiver; see `send_stack` for the
   equivalent op code used when no block is to be passed.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack">send_stack</a></li>
</ul>
<h3><a class="instruction" name="send_stack_with_splat">send_stack_with_splat(literal, count)</a></h3>



<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>block</td><td>value</td></tr>
<tr><td>array</td><td>...</td></tr>
<tr><td>receiver</td><td></td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="send_super_stack_with_block">send_super_stack_with_block(literal, count)</a></h3>

   Call a method on the superclass with a block

   The same as `send_stack_with_block`, but reciever is the current self
   instead of being read from the stack, and the method to call is looked up
   starting with the reciever superclass.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   block
</td><td>   retval
</td></tr>
<tr><td>   argN
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="send_super_stack_with_splat">send_super_stack_with_splat(literal, count)</a></h3>

   Call a method on the superclass, passing args plus a block.

   The same as `send_stack_with_block`, but receiver is the current `self`
   instead of being read from the stack, and the method to call is looked up
   starting with the receiver superclass.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   block
</td><td>   retval
</td></tr>
<tr><td>   argN
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_block">push_block()</a></h3>

   Pushes the current block onto the stack. The value is not wrapped in a
   `Proc` if it is a `BlockEnvironment`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>block</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_proc">push_proc</a></li>
</ul>
<h3><a class="instruction" name="passed_blockarg">passed_blockarg(count)</a></h3>

   Check if exactly _count_ arguments were passed to the current invocation.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>boolean</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   _This instruction is deprecated and no longer used._

<h3><a class="instruction" name="create_block">create_block(literal)</a></h3>

   Read a CompiledMethod specified by the operand +literal+ and create a
   `BlockEnvironment`.  Push the new `BlockEnvironment` object on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>block</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="cast_for_single_block_arg">cast_for_single_block_arg()</a></h3>

   Converts the value on the top of the stack into an argument for a block
   taking one argument.

   The value on the top of the stack is popped, and:

   If it has no fields, the result is `nil`.

   If the value contains a single field, the result is the value in the
   first field.

   Otherwise, package up all the arguments in an `Array` as the result.

   The result is then pushed onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>argument</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This is a single use instruction, only used to simplify how to handle a
   block that accepts one argument.

<h3><a class="instruction" name="cast_for_multi_block_arg">cast_for_multi_block_arg()</a></h3>

   Converts a block argument single-valued tuple into multiple arguments if
   the arg is an array.

   If the Proc invoked from was in lambda mode, and one argument is passed:
     * and it's an Array, push it.
     * and it responds to `#to_ary`, try and convert it and push it.
     * otherwise wrap it in a one element Array and push it.

   Otherwise:
     Package up the arguments into an `Array` and push it onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   value1
</td><td>   array[value1,..] | value1
</td></tr>
<tr><td>   value2
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>

#### Example
     [[1,2,3]].each do |i,j,k|
       # do something
     end


#### Notes
   This is a single use instruction, only used to simplify how to handle a
   block that accepts 2 or more arguments. The semantics for this instruction
   change depending on if the current block invocation is from a Proc with
   lambda semantics or not.

<h3><a class="instruction" name="cast_for_splat_block_arg">cast_for_splat_block_arg()</a></h3>

   Take all arguments passed to the current invocation and package
   them into an `Array`, which is then pushed on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>arguments</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="yield_stack">yield_stack(count)</a></h3>

   Invoke the current block, passing _count_ arguments to it.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   value
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack">send_stack</a></li>
</ul>
<h3><a class="instruction" name="yield_splat">yield_splat(count)</a></h3>

   Invoke the current block, passing _count_ arguments to it in
   addition to the values in the `Array` _array_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   array
</td><td>   value
</td></tr>
<tr><td>   argN
</td><td>   ...
</td></tr>
<tr><td>   ...
</td><td></td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack_with_splat">send_stack_with_splat</a></li>
</ul>
<h3><a class="instruction" name="string_append">string_append()</a></h3>

   Pops two strings off the stack, appends the second to the first, and
   then pushes the combined string back onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>prefix</td><td>string</td></tr>
<tr><td>suffix</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

#### Notes
   The original string is modified by the append.

<h3><a class="instruction" name="string_build">string_build(count)</a></h3>

   Build a new string using many substrings

   Remove _count_ elements from the stack and interpret each as a `String`.
   Build a new string which is all the removed elements concated together in
   the order they were on the stack.

   Push the resulting string.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   stringN
</td><td>   string1string2..stringN
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   string2
</td><td></td></tr>
<tr><td>   string1
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="string_dup">string_dup()</a></h3>

   Consume the string on the stack, replacing it with a duplicate. Mutating
   operations on the original string will not affect the duplicate, and
   vice-versa.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>string</td><td>string</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_scope">push_scope()</a></h3>

   Pushes the current `StaticScope` object on the stack. Many operations are
   defered to the current scope. This operation retrieves the current scope
   so methods can be called on it.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>scope</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="add_scope">add_scope()</a></h3>

   Create a new `StaticScope` object for the given Module on the stack.
   This scope is chained off the current scope of the method.

   This also sets the scope of the current `CompiledMethod` to the new
   `StaticScope`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>module</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_variables">push_variables()</a></h3>

   Push the `VariableScope` for the current method/block invocation on the
   stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>scope</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="check_interrupts">check_interrupts()</a></h3>

   Perform required occasional checks that must be done.  This instruction is
   used by loops to allow them to be interrupted externally, and thus also
   cause the current method to heat up.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="yield_debugger">yield_debugger()</a></h3>

   Pauses virtual machine execution and yields control to the debugger on the
   debug channel. If no debugger is registered, an error is raised.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   _This instruction is deprecated and should not be used._

<h3><a class="instruction" name="is_nil">is_nil()</a></h3>

   Pop the _value_ from the stack, and push `true` or `false` depending on
   whether the consumed value was the special value `nil`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>boolean</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="check_serial">check_serial(literal, serial)</a></h3>

   Checks if the specified method serial number matches an expected value.

   Pops the _receiver_ object from the stack and checks if it responds to the
   message specified by the operand _literal_ and the target method has
   serial number _serial_. If so, push `true`, else push `false`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>receiver</td><td>boolean</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This opcode is typically used to determine at runtime whether an
   optimisation can be performed. At compile time, two code paths are
   generated: a slow, but guaranteed correct path, and a fast path that uses
   certain optimisations. The serial number check is then performed at
   runtime to determine which code path is executed.

   For example, a method such as `Fixnum#times` can be optimised at compile
   time, but we can't know until runtime whether or not the `Fixnum#times`
   method has been overridden. The serial number check is used to determine
   each time the code is executed, whether or not the standard `Fixnum#times`
   has been overridden. It leverages the serial number field on a
   `CompiledMethod`, is initialised to either 0 (for kernel land methods) or
   1 (for user land methods).

<h3><a class="instruction" name="check_serial_private">check_serial_private(literal, serial)</a></h3>

   Checks if the specified method's serial number matches an expected value.
   Considers `private` methods too.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>receiver</td><td>boolean</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#check_serial">check_serial</a></li>
</ul>
<h3><a class="instruction" name="push_my_field">push_my_field(index)</a></h3>

   Pushes the value of the specified field in the current object onto the
   stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Fields are similar to instance variables, but have dedicated storage
   allocated. They are primarily used on core or bootstap classes.
   This instruction should not be used directly. The VM will specialize
   push_ivar instructions into this.

<h3><a class="instruction" name="store_my_field">store_my_field(index)</a></h3>

   Stores the value at the top of the stack into the field specified by
   _index_ on `self`.

   The stack is left unmodified.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This instruction should not be used directly. The VM will specialize
   push_ivar instructions into this.

<h3><a class="instruction" name="kind_of">kind_of()</a></h3>

   Evaluate if _object_ is an instance of _class_ or of an ancestor of
   _class_. If so, push `true`, else push `false`.

   The equivalent of `object.kind_of?(klass)` in Ruby.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>object</td><td>boolean</td></tr>
<tr><td>class</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#instance_of">instance_of</a></li>
</ul>
<h3><a class="instruction" name="instance_of">instance_of()</a></h3>

   Evaluate if _object_ is an instance of _class_. If so, push `true`, else
   push `false`.

   The equivalent of `object.instance_of?(klass)` in Ruby.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>object</td><td>boolean</td></tr>
<tr><td>class</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#kind_of">kind_of</a></li>
</ul>
<h3><a class="instruction" name="meta_push_neg_1">meta_push_neg_1()</a></h3>

   Push `-1` (negative 1) onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This is an optimisation applied by the compiler.

<h3><a class="instruction" name="meta_push_0">meta_push_0()</a></h3>

   Push `0` (zero) onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This is an optimisation applied by the compiler.

<h3><a class="instruction" name="meta_push_1">meta_push_1()</a></h3>

   Push `1` (one) onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This is an optimisation applied by the compiler.

<h3><a class="instruction" name="meta_push_2">meta_push_2()</a></h3>

   Push `2` (two) onto the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This is an optimisation applied by the compiler.

<h3><a class="instruction" name="meta_send_op_plus">meta_send_op_plus(literal)</a></h3>

   Implementation of `#+` optimised for `Fixnum`.

   Pops _value1_ and _value2_ off the stack, and pushes the _sum_ (_value1_
   `+` _value2_).  If both values are Fixnums, the addition is done directly
   via the `fixnum_add` primitive. Otherwise, the `#+` method is called on
   _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>sum</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_send_op_minus">meta_send_op_minus(literal)</a></h3>

   Implementation of `#-` optimised for `Fixnum`.

   Pops _value1_ and _value2_ off the stack, and pushes the _difference_ (_value1_
   `-` _value2_).  If both values are Fixnums, the subtraction is done directly
   via the `fixnum_sub` primitive. Otherwise, the `#-` method is called on
   _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>difference</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_send_op_equal">meta_send_op_equal(literal)</a></h3>

   Implementation of `#==` optimised for `Fixnum` and `Symbol`.

   Pops _value1_ and _value2_ off the stack and pushes the logical result
   of (_value1_ `==` _value2_). If _value1_ and _value2_ are both Fixnums or
   both Symbols, the comparison is done directly. Otherwise, the `#==` method
   is called on _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>boolean</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_send_op_lt">meta_send_op_lt(literal)</a></h3>

   Implementation of `#<` optimised for `Fixnum`.

   Pops _value1_ and _value2_ off the stack, and pushes the logical result
   of (_value1_ `<` _value2_). If _value1_ and _value2_ are both Fixnums, the
   comparison is done directly. Otherwise, the `#<` method is called on
   _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>boolean</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_send_op_gt">meta_send_op_gt(literal)</a></h3>

   Implementation of `#>` optimised for `Fixnum`.

   Pops _value1_ and _value2_ off the stack, and pushes the logical result
   of (_value1_ `>` _value2_). If _value1_ and _value2_ are both Fixnums, the
   comparison is done directly. Otherwise, the `#>` method is called on
   _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>boolean</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_send_op_tequal">meta_send_op_tequal(literal)</a></h3>

   Implementation of `#===` (triple equal) optimised for `Fixnum` and
   `Symbol`.

   Pops _value1_ and _value2_ off the stack, and pushes the logical result
   of (_value1_ `===` _value2_). If _value1_ and _value2_ are both Fixnums or
   both Symbols, the comparison is done directly. Otherwise, the `#===` method
   is called on _value1_, passing _value2_ as the argument.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value1</td><td>boolean</td></tr>
<tr><td>value2</td><td>...</td></tr>
<tr><td>...</td><td></td></tr>
</tbody>
</table>

#### Notes
   Exactly like equal, except calls `#===` if it can't handle it directly.

<h3><a class="instruction" name="meta_send_call">meta_send_call(literal, count)</a></h3>

   Simplified call instruction used for non-dynamic `yield` calls and for
   simple calls with static arguments.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   retval
</td></tr>
<tr><td>   ...
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   receiver
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_my_offset">push_my_offset(index)</a></h3>

   Pushes a value read directly from within the body of an object.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>offset</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   This instruction must never be used directly. The VM will specialize
   `push_my_field` instructions into this.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_my_field">push_my_field</a></li>
</ul>
<h3><a class="instruction" name="zsuper">zsuper(literal)</a></h3>

   Call a superclass method on the current, passing the arguments
   passed to the current invocation.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   value
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   ...
</td><td></td></tr>
</tbody>
</table>

#### Notes
   This is a specialization of `send_super_with_stack` that is necessary for
   Ruby semantics regarding how to read the original arguments.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_super_with_stack">send_super_with_stack</a></li>
</ul>
<h3><a class="instruction" name="push_block_arg">push_block_arg()</a></h3>

   Push the block passed as an argument to the current invocation.
   This differs from `push_block` in that in is not the block for the
   currrent scope because of how the current block is seen within
   an existing block.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>block</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_block">push_block</a></li>
</ul>
<h3><a class="instruction" name="push_undef">push_undef()</a></h3>

   Push the special undefined value on the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_stack_local">push_stack_local(which)</a></h3>

   Push the stack local identified by operand _which_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Stack locals differ from normal locals in that they are not viewable by
   closures.

<h3><a class="instruction" name="set_stack_local">set_stack_local(which)</a></h3>

   Set the stack local identified by operand _which_ using the value on the
   top of the stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Stack locals differ from normal locals in that they are not viewable by
   closures.

<h3><a class="instruction" name="push_has_block">push_has_block()</a></h3>

   Push `true` or `false` based on whether there is a current block.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Used to implement `block_given?` without having to directly expose
   the block object itself. This simplifies JIT inlining.

<h3><a class="instruction" name="push_proc">push_proc()</a></h3>

   Wrap the current block in a `Proc` and push it onto the stack.  If there
   is no current block, push `nil`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>value</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Used to implement `&block` in a method signature.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_block">push_block</a></li>
</ul>
<h3><a class="instruction" name="check_frozen">check_frozen()</a></h3>

   Check if the value on the top of the stack is frozen.  If so, raise a
   `TypeError` indicating so.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>value</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   An optimzation to deal with check for frozen.

<h3><a class="instruction" name="cast_multi_value">cast_multi_value()</a></h3>

   Convert a value into an Array

   Pop _value_. If it is an `Array`, push it back on the stack.  Otherwise,
   attempt to convert it to an `Array` using `#to_ary` and push the result.
   If the value can not be converted to an array, it is wrapped in a one
   element `Array`.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>value</td><td>array</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="invoke_primitive">invoke_primitive(literal, count)</a></h3>

   Directly invoke a primitive by name.

   Pop _count_ values off the stack and pass them directly to the primitive
   operation named by the operand _literal_.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   value
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_rubinius">push_rubinius()</a></h3>

   Pushes the top-level global `Rubinius` constant onto the stack.  Generally
   this is done to call a utility method.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>constant</td></tr>
<tr><td></td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="call_custom">call_custom(literal, count)</a></h3>

   Invoke a method via the call custom protocol.

   Pop the _receiver_ and _count_ values off the stack and begin the call
   custom invocation protocol with them as arguments.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>   argN
</td><td>   value
</td></tr>
<tr><td>   ...
</td><td>   ...
</td></tr>
<tr><td>   arg2
</td><td></td></tr>
<tr><td>   arg1
</td><td></td></tr>
<tr><td>   reciever
</td><td></td></tr>
</tbody>
</table>
<h3><a class="instruction" name="meta_to_s">meta_to_s(literal)</a></h3>

   Pop a value off the stack and if it's not a `String`, call a method
   indicated by _literal_ on it.  Push the resulting object back on the
   stack.


<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>object</td><td>string</td></tr>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes
   Normally literal is `:to_s`, but this instruction leaves it up to the user
   to indicate for flexibility.

