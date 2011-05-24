<h3><a class="instruction" name="noop">noop()</a></h3>

   Классический пустой оператор. Не производит никаких действий и не
   модифицирует стек.


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

   Помещает значение целочисленного литерала в стек.


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
   Некоторые частые случаи применения (-1, 0, 1, 2) оптимизированы на предмет
   избегания декодирования аргумента.


<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#meta_push_0">meta_push_0</a></li>
<li><a href="#meta_push_1">meta_push_1</a></li>
<li><a href="#meta_push_2">meta_push_2</a></li>
<li><a href="#meta_push_neg_1">meta_push_neg_1</a></li>
</ul>
<h3><a class="instruction" name="push_self">push_self()</a></h3>




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


<h3><a class="instruction" name="push_literal">push_literal(literal)</a></h3>




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


<h3><a class="instruction" name="goto">goto(location)</a></h3>




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#goto_if_true">goto_if_true</a></li>
<li><a href="#goto_if_false">goto_if_false</a></li>
</ul>
<h3><a class="instruction" name="goto_if_false">goto_if_false(location)</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#raise_return">raise_return</a></li>
<li><a href="#raise_exc">raise_exc</a></li>
</ul>
<h3><a class="instruction" name="swap_stack">swap_stack()</a></h3>




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


<h3><a class="instruction" name="pop_many">pop_many(count)</a></h3>




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


<h3><a class="instruction" name="push_current_exception">push_current_exception()</a></h3>




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




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="push_exception_state">push_exception_state()</a></h3>




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




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#pop_unwind">pop_unwind</a></li>
</ul>
<h3><a class="instruction" name="pop_unwind">pop_unwind()</a></h3>




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




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="make_array">make_array(count)</a></h3>




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




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes


<h3><a class="instruction" name="allow_private">allow_private()</a></h3>




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="send_method">send_method(literal)</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack">send_stack</a></li>
</ul>
<h3><a class="instruction" name="send_stack">send_stack(literal, count)</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_stack_with_block">send_stack_with_block</a></li>
</ul>
<h3><a class="instruction" name="send_stack_with_block">send_stack_with_block(literal, count)</a></h3>




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


<h3><a class="instruction" name="create_block">create_block(literal)</a></h3>




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


<h3><a class="instruction" name="cast_for_multi_block_arg">cast_for_multi_block_arg()</a></h3>




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


<h3><a class="instruction" name="cast_for_splat_block_arg">cast_for_splat_block_arg()</a></h3>




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


<h3><a class="instruction" name="string_build">string_build(count)</a></h3>




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




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>
<h3><a class="instruction" name="yield_debugger">yield_debugger()</a></h3>




<table class="stack_effect">
<thead>
<tr><th>Before</th><th>After</th></tr>
</thead>
<tbody>
<tr><td>...</td><td>...</td></tr>
</tbody>
</table>

#### Notes


<h3><a class="instruction" name="is_nil">is_nil()</a></h3>




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


<h3><a class="instruction" name="check_serial_private">check_serial_private(literal, serial)</a></h3>




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


<h3><a class="instruction" name="store_my_field">store_my_field(index)</a></h3>




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


<h3><a class="instruction" name="kind_of">kind_of()</a></h3>




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


<h3><a class="instruction" name="meta_push_0">meta_push_0()</a></h3>




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


<h3><a class="instruction" name="meta_push_1">meta_push_1()</a></h3>




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


<h3><a class="instruction" name="meta_push_2">meta_push_2()</a></h3>




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


<h3><a class="instruction" name="meta_send_op_plus">meta_send_op_plus(literal)</a></h3>




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


<h3><a class="instruction" name="meta_send_call">meta_send_call(literal, count)</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_my_field">push_my_field</a></li>
</ul>
<h3><a class="instruction" name="zsuper">zsuper(literal)</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#send_super_with_stack">send_super_with_stack</a></li>
</ul>
<h3><a class="instruction" name="push_block_arg">push_block_arg()</a></h3>




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


<h3><a class="instruction" name="set_stack_local">set_stack_local(which)</a></h3>




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


<h3><a class="instruction" name="push_has_block">push_has_block()</a></h3>




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


<h3><a class="instruction" name="push_proc">push_proc()</a></h3>




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



<h4>See Also</h4>
<ul class="insn_cross_ref">
<li><a href="#push_block">push_block</a></li>
</ul>
<h3><a class="instruction" name="check_frozen">check_frozen()</a></h3>




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


<h3><a class="instruction" name="cast_multi_value">cast_multi_value()</a></h3>




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


