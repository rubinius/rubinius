require 'rubygems'
require 'bytecode/assembler'
require 'test/unit'
require 'test/unit/show_code'

class TestBytecodeAssembler < Test::Unit::TestCase
  
  def setup
    @asm = Bytecode::Assembler.new([])
    @enc = Bytecode::InstructionEncoder.new
    @cpu = CPU.new
  end
  
  def teardown
    @cpu.delete
  end
  
  def assemble(str)
    out = @asm.assemble str
    str = out.inject(str) { |acc, i| acc << @enc.encode(*i) }
    assert !str.empty?
    return out
  end
  
  def reset
    @asm.reset
  end
  
  def test_set_label
    out = assemble "start:\n"
    
    assert_equal [], out
    assert_equal 0, @asm.labels[:start].location
  end
  
  def test_goto_label_before
    out = assemble "noop\nstart:\ngoto start\n"
    expected = [:noop, [:goto, 1]]
    assert_equal expected, out
  end
  
  def test_goto_label_after
    out = assemble "noop\ngoto end\nend: noop\n"
    expected = [:noop, [:goto, 6], :noop]
    assert_equal expected, out
  end
  
  def test_address_of_label
    out = assemble "noop\nstart: noop\npush &start\n"
    expected = [:noop, :noop, [:push_int, 1]]
    assert_equal expected, out
  end
  
  def test_address_of_label_post
    out = assemble "noop\npush &start\nstart: noop\n"
    expected = [:noop, [:push_int, 6], :noop]
    assert_equal expected, out
  end
  
  def test_push_number
    out = assemble "push 88\n"
    expected = [[:push_int, 88]]
    assert_equal expected, out
  end
  
  def test_push_true
    out = assemble "push true\n"
    expected = [:push_true]
    assert_equal expected, out
  end
  
  def test_push_false
    out = assemble "push false\n"
    expected = [:push_false]
    assert_equal expected, out
  end
  
  def test_push_true
    out = assemble "push nil\n"
    expected = [:push_nil]
    assert_equal expected, out
  end

  def test_push_self
    out = assemble "push self\n"
    expected = [:push_self]
    assert_equal expected, out
  end
  
  def test_push_symbol
    out = assemble "push :blah\n"
    expected = [[:push_literal, 0]]
    assert_equal expected, out
  end
  
  def test_push_const
    out = assemble "push Blah\n"
    expected = [[:push_const, 0]]
    assert_equal [:Blah], @asm.literals
    assert_equal expected, out
  end
  
  def test_find_const
    out = assemble "find EEK\n"
    expected = [[:find_const, 0]]
    assert_equal [:EEK], @asm.literals
    assert_equal expected, out
  end
  
  def test_set_const
    out = assemble "set A\n"
    expected = [[:set_const, 0]]
    assert_equal [:A], @asm.literals
    assert_equal expected, out
  end
  
  def test_set_const_at
    out = assemble "set B::A\n"
    expected = [[:push_const, 0], [:set_const_at, 1]]
    assert_equal [:B, :A], @asm.literals
    assert_equal expected, out
  end
  
  def test_set_const_at2
    out = assemble "set +A\n"
    expected = [[:set_const_at, 0]]
    assert_equal [:A], @asm.literals
    assert_equal expected, out
  end
  
  def test_attach_method
    assert_assembled "attach_method __blah__\n", [[:attach_method, 0]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_add_method
    assert_assembled "add_method __blah__\n", [[:add_method, 0]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send_method
    assert_assembled "send_method __blah__\n", [[:send_method, 0]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send_stack
    assert_assembled "send_stack __blah__ 1\n", [[:send_stack, 0, 1]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send
    assert_assembled "send __blah__\n", [[:send_method, 0]]
    assert_equal [:__blah__], @asm.literals    
  end
  
  def test_send_with_args
    assert_assembled "send __blah__ 1\n", [[:send_stack, 0, 1]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send_with_block
    assert_assembled "&send __blah__ 1\n", [[:send_stack_with_block, 0, 1]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send_with_arg_register
    assert_assembled "send __blah__ +\n", [[:send_with_arg_register, 0]]
    assert_equal [:__blah__], @asm.literals
  end
  
  def test_send_prim
    assert_assembled "send_primitive create_block\n", [[:send_primitive, 11]]
  end
  
  def assert_assembled(str, output)
    out = assemble str
    assert_equal output, out
  end
  
  def test_pop
    out = assemble "pop\n"
    assert_equal [:pop], out
  end
  
  def test_swap
    assert_assembled "swap", [:swap_stack]
    reset
    assert_assembled "swap_stack", [:swap_stack]
  end
  
  def test_dup
    assert_assembled "dup", [:dup_top]
  end
  
  def test_set_lvar
    assert_assembled "set a", [[:set_local, 2]]
  end
  
  def test_set_lvar_with_index
    assert_assembled "set a:10", [[:set_local, 10]]
  end
  
  def test_push_local
    assert_assembled "push a\n", [[:push_local, 2]]
  end
  
  def test_push_local_with_index
    assert_assembled "push a:10\n", [[:push_local, 10]]
  end
  
  def test_set_ivar_of_object
    assert_assembled "set a.blah", [[:push_local, 2], [:set_ivar, 0]]
    assert_equal [:@blah], @asm.literals
  end
  
  def test_push_ivar_of_object
    assert_assembled "push a.blah", [[:push_local, 2], [:push_ivar, 0]]
    assert_equal [:@blah], @asm.literals
  end
  
  def test_push_ivar
    assert_assembled "push @blah", [[:push_ivar, 0]]
    assert_equal [:@blah], @asm.literals
  end
  
  def test_set_ivar
    assert_assembled "set @blah", [[:set_ivar, 0]]
    assert_equal [:@blah], @asm.literals
  end
  
  def test_set_field_for_self
    assert_assembled "set self[9]", [:push_self, [:push_int, 9], :store_field]
  end
  
  def test_set_field_for_object
    assert_assembled "set a[0]", [[:push_local, 2], [:push_int, 0], :store_field]
  end
  
  def test_fetch_field_for_self
    assert_assembled "push self[0]", [:push_self, [:push_int, 0], :fetch_field]
  end
  
  def test_fetch_field_for_object
    assert_assembled "push a[0]", [[:push_local, 2], [:push_int, 0], :fetch_field]
  end
  
  def test_source_line_generation
    assert_assembled "noop\n#line 8\nnoop\nnoop\n", [:noop, :noop, :noop]
    assert_equal [[0,1,0],[1,3,8]], @asm.source_lines
  end
  
  def test_arguments_generation
    code = <<-CODE
#arg name
#arg age
noop
    CODE
    
    assert_assembled code, [:noop]
    assert_equal [:name, :age], @asm.arguments
  end
  
  def test_exception_handlers
    code = <<-CODE
noop
#exc_start e1
dup
#exceptions e1
pop
noop
#exc_end e1
dup
    CODE
    assert_assembled code, [:noop, :dup_top, [:goto, 9], :pop, :noop, :dup_top]
    assert_equal( {:e1 => [1,2,7]}, @asm.exceptions)
  end
  
  def test_layered_exception_handlers
    code = <<-CODE
noop
#exc_start e1
dup
#exc_start e2
push 9
#exceptions e2
push 10
#exc_end e2
push 11
#exceptions e1
#exc_start e3
push 12
#exceptions e3
push 14
#exc_end e3
#exc_end e1
push 13
    CODE
    
    expected = [:noop, :dup_top, [:push_int, 9], [:goto, 17], 
      [:push_int, 10], [:push_int, 11], [:goto, 42], [:push_int, 12],
      [:goto, 42], [:push_int, 14], [:push_int, 13]]
    assert_assembled code, expected
    expected_exc = {
      :e2 => [2, 7, 12],
      :e1 => [1, 22, 27],
      :e3 => [27, 32, 37]
    }
    assert_equal expected_exc, @asm.exceptions
    sorted = [[1, 1, 27], [2, 7, 12], [2, 22, 27], [27, 32, 37]]
    assert_equal sorted, @asm.sorted_exceptions
  end
  
  def test_incorrect_exception_layout
    code = <<-CODE
noop
#exc_start e1
dup
#exc_start e2
noop
#exceptions e1
push 1
#exceptions e2
push 2
#exc_end e2
push 3
#exc_end e1
    CODE
    
    assert_raise(Bytecode::Assembler::ExceptionLayoutError) do
      out = assemble code
    end
  end
  
  def test_incorrect_exception_layout2
    code = <<-CODE
noop
#exc_start e1
dup
#exc_start e2
noop
#exceptions e2
push 1
#exceptions e1
push 2
#exc_end e2
push 3
#exc_end e1
    CODE

    assert_raise(Bytecode::Assembler::ExceptionLayoutError) do
      out = assemble code
    end
  end
  
  def test_incorrect_exception_layout3
    code = <<-CODE
#exc_start e1
noop
#exceptions e1
#exc_end e2
    CODE
    
    assert_raise(Bytecode::Assembler::ExceptionLayoutError) do
      assemble code
    end
  end
  
  def test_exceptions_only_used_once
    code = <<-CODE
#exc_start e1
noop
#exceptions e1
noop
#exceptions e1
#exc_end e1
    CODE
    
    assert_raise(Bytecode::Assembler::ExceptionLayoutError) do
      assemble code
    end
  end
  
  def test_exc_end_only_used_once
    code = <<-CODE
#exc_start e1
noop
#exceptions e1
noop
#exc_end e1
#exc_end e1
    CODE
    
    assert_raise(Bytecode::Assembler::ExceptionLayoutError) do
      assemble code
    end
  end
  
  def test_primitive_command
    assert @asm.primitive.nil?
    code = "#primitive 3\nnoop"
    assert_assembled code, [:noop]
    assert_equal 3, @asm.primitive
  end
  
  def test_exceptions_as_tuple
    @cpu.bootstrap
    
    code = <<-CODE
noop
#exc_start e1
dup
#exceptions e1
noop
#exc_end e1
    CODE
    
    assemble code
    tup = @asm.exceptions_as_tuple
    assert_equal 1, tup.fields
    0.upto(tup.fields - 1) do |idx|
      ent = tup.at(idx)
      assert_equal 3, ent.fields
      0.upto(2) do |j|
        assert ent.at(j).fixnum?
      end
    end
  end
  
  def test_lines_as_tuple
    @cpu.bootstrap
    
    code = <<-CODE
#line 1
noop
noop
#line 2
dup
    CODE
    
    assemble code
    tup = @asm.lines_as_tuple
    assert_equal 2, tup.fields
    0.upto(tup.fields - 1) do |idx|
      ent = tup.at(idx)
      assert_equal 3, ent.fields
      0.upto(2) do |j|
        assert ent.at(j).fixnum?
      end
    end
  end
  
  def test_arguments_as_tuple
    @cpu.bootstrap
    
    code = <<-CODE
#arg name
#arg age
noop
    CODE
    
    assemble code
    tup = @asm.arguments_as_tuple
    assert_equal 2, tup.fields
    0.upto(tup.fields - 1) do |idx|
      ent = tup.at(idx)
      assert ent.symbol?
    end
    
  end
  
  def test_literals_as_tuple
    @cpu.bootstrap

    code = <<-CODE
push @name
push @age
    CODE

    assemble code
    tup = @asm.literals_as_tuple
    assert_equal 2, tup.fields
    0.upto(tup.fields - 1) do |idx|
      ent = tup.at(idx)
      assert ent.symbol?
    end
  end
  
  def test_into_method
    @cpu.bootstrap
    
    code = <<-CODE
#primitive 10
push @blah
    CODE
    
    out = assemble code
    cm = @asm.into_method
    assert_equal CPU::Global.cmethod.address, cm.rclass.address
    assert_equal 10, cm.primitive.to_int
    assert_rkind_of CPU::Global.tuple, cm.literals
    assert_rkind_of CPU::Global.tuple, cm.arguments
    unless cm.exceptions.nil?
      assert_rkind_of CPU::Global.tuple, cm.exceptions
    end
    assert_rkind_of CPU::Global.tuple, cm.lines
    assert_rkind_of CPU::Global.bytearray, cm.bytecodes
    bc = cm.bytecodes
    bc.as :bytearray
    str = bc.as_string
    assert_equal @asm.bytecodes.strip, str.strip
  end
  
  def assert_rkind_of(cls, obj)
    assert_equal cls.address, obj.rclass.address
  end
  
  def test_open_class
    code = "open_class EEK"
    out = assemble code
    assert_equal [:EEK], @asm.literals
    assert_equal [[:open_class, 0]], out
  end
  
  def test_open_class_under
    code = "open_class_under EEK"
    out = assemble code
    assert_equal [:EEK], @asm.literals
    assert_equal [[:open_class_under, 0]], out
  end
  
  def test_open_module
    code = "open_module EEK"
    out = assemble code
    assert_equal [:EEK], @asm.literals
    assert_equal [[:open_module, 0]], out
  end
  
  def test_open_module_under
    code = "open_module_under EEK"
    out = assemble code
    assert_equal [:EEK], @asm.literals
    assert_equal [[:open_module_under, 0]], out
  end
  
  def test_check_argcount
    code = "check_argcount 2 2"
    out = assemble code
    assert_equal [[:check_argcount, 2, 2]], out
  end
  
end