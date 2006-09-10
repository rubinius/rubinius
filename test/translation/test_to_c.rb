require 'translation/to_c'
require 'test/unit'

class TestRsToC < Test::Unit::TestCase
  def setup
    @info = TypeInfo.new("test_")
    @cg = @info.cg
    @rc = RsToC.new(@info, @info.cg)
    TypeInfo::CodeGenerator.reset
  end
  
  def trans(x)
    @rc.process x.deep_clone
  end
  
  def test_true
    assert_equal "TRUE", trans([:true])
  end
  
  def test_false
    assert_equal "FALSE", trans([:false])
  end
  
  def test_nil
    assert_equal "NIL", trans([:nil])
  end
  
  def test_self
    assert_equal "self", trans([:self])
  end
  
  def test_process_call
    ft = Function.new :blah, nil, nil, Type.Fixnum
    @info.add_klass :Test, :Object
    @info.add_function Type.Test, ft
    
    @rc.typer.self_type << Type.Test
    
    input = [:call, [:self], :blah, [:array]]
    output = "test_Test_blah(self, NO_BLOCK)"
    
    got = trans input
    assert_equal output, got
    
    ft.reset_args!
    
    input = [:call, [:self], :blah, [:array, [:true], [:false]]]
    output = "test_Test_blah(self, NO_BLOCK, TRUE, FALSE)"
    
    got = trans input
    assert_equal output, got
  end
  
  def test_process_call_into_super
    
    ft = Function.new :blah, nil, nil, Type.Fixnum
    @info.add_klass :Container, :Object
    @info.add_function Type.Container, ft
    
    @info.add_klass :Box, :Container
        
    @rc.typer.self_type << Type.Box
    
    input = [:call, [:self], :blah, [:array]]
    output = "test_Container_blah((struct test_Container*)self, NO_BLOCK)"
    
    got = trans input
    assert_equal output, got
  end
  
  def test_process_call_into_inlined_external
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.Fixnum
      i.gen do |g, s|
        "#{s} + #{s}"
      end
    end
    
    input = [:call, [:lit, 9], :blah, [:array]]
    output = "9 + 9;"
    
    got = trans input
    assert_equal output, got
  end
  
  def test_process_call_with_temp
    @info.add_c_type Type.Fixnum, "int"
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.Fixnum
      i.gen do |g, s|
        t = g.temp(Type.Fixnum)
        "#{t} = #{s} + #{s}"
      end
    end
    
    input = [:call, [:lit, 9], :blah, [:array]]
    output = "_cg0 = 9 + 9;"
    
    got = trans input
    assert_equal output, got
    assert_equal "int _cg0;", @cg.preamble
  end
  
  def test_process_call_with_temp_cast
    @info.add_c_type Type.Fixnum, "int"
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.Fixnum
      i.gen do |g, s|
        t = g.temp(Type.Fixnum)
        t.cast_as "#{s} + #{s}"
      end
    end
    
    input = [:call, [:lit, 9], :blah, [:array]]
    output = "_cg0 = (int)(9 + 9);"
    
    got = trans input
    assert_equal output, got
    assert_equal "int _cg0;", @cg.preamble
  end
  
  def test_process_call_with_args
    @info.add_c_type Type.Fixnum, "int"
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.Fixnum
      i.args = [Type.Fixnum]
      i.gen do |g, s, a|
        t = g.temp(Type.Fixnum)
        t.cast_as "#{s} + #{a.shift}"
      end
    end
    
    input = [:call, [:lit, 9], :blah, [:array, [:lit, 10]]]
    output = "_cg0 = (int)(9 + 10);"
    
    got = trans input
    assert_equal output, got
    assert_equal "int _cg0;", @cg.preamble
  end
  
  def test_process_iter
    
    ft = Function.new :show_this, nil, nil, Type.Fixnum
    @info.add_klass :Container, :Object
    @info.add_function Type.Container, ft
    
    @rc.typer.self_type << Type.Container
    
    input = [:iter,
      [:call, [:self], :show_this, [:array]],
      nil,
      [:block, [:true]]
    ]
    
    @info.add_c_type Type.bool, "BOOL"
    
    output = "test_Container_show_this(self, BLOCK(_rstoc_1))"
    
    got = trans input
    assert_equal output, got
    
    output = "BOOL _rstoc_1(struct _locals1 *locals) { return TRUE; }"
    assert_equal output, @cg.blocks
  end
  
  def test_process_lasgn
    @info.add_c_type Type.Fixnum, "int"
    
    out = trans [:lasgn, :blah, 2, [:lit, 8]]
    assert_equal "locals->blah = 8", out
    
    assert_equal "struct _locals1 {\n  int blah;\n};", @cg.local_struct
    assert_equal "struct _locals1 _locals, *locals = &_locals;", @cg.preamble
  end
  
  def test_process_lvar
    trans [:lasgn, :blah, 2, [:lit, 8]]
    
    out = trans [:lvar, :blah, 2]
    
    assert_equal "locals->blah", out
  end
  
  def test_process_ivar
    out = trans [:ivar, :blah]
    assert_equal "self->blah", out
  end
  
  def test_process_iasgn
    out = trans [:iasgn, :blah, [:lit, 8]]
    assert_equal "self->blah = 8", out
  end
  
  def test_process_scope
    input = [:scope, [:block, 
      [:lit, 8]
    ]]
    
    out = trans input
    assert_equal "8;", out
  end
  
  def test_process_return
    input = [:return, [:lit, 8]]
    out = trans input
    assert_equal "return 8", out
    assert_equal Type.Fixnum, @rc.return_type
  end
  
  def test_return_type
    class << @rc
      attr_accessor :proc
    end
    
    @rc.proc.scope_type = Type.Fixnum
    @rc.proc.ret_types << Type.String
    
    assert_raises(RuntimeError) do
      @rc.return_type
    end
    
    @rc.proc.ret_types = [Type.Fixnum]
    
    out = @rc.return_type
    
    assert_equal Type.Fixnum, out
  end
  
  def test_process_if
    input = [:if, [:true], [:lit, 9], nil]
    output = "if(TRUE) {\n  9;\n} else {\n  \n}"
    
    out = trans input
    assert_equal output, out
  end
  
  def test_process_while
    input = [:while, [:true], [:lit, 9]]
    output = "while(TRUE) {\n  9;\n}"
    
    out = trans input
    assert_equal output, out
  end
  
  def test_process_until
    input = [:until, [:true], [:lit, 9]]
    output = "until(TRUE) {\n  9;\n}"
    
    out = trans input
    assert_equal output, out
  end
end