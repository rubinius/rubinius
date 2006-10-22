require 'translation/to_c'
require 'test/unit'
require 'pp'

class TestRsToC < Test::Unit::TestCase
  def setup
    @info = TypeInfo.new("test_")
    @cg = @info.cg
    @rc = RsToC.new(@info, @info.cg)
    TypeInfo::CodeGenerator.reset
  end
  
  def load_types(kind="c")
    Dir["lib/translation/types/#{kind}/*.rb"].each do |path|
      @info.load_file path
    end
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
    output = "({ 9 + 9; })"
    
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
    output = "({ _cg0 = 9 + 9; })"
    
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
    output = "({ _cg0 = (int)(9 + 9); })"
    
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
    output = "({ _cg0 = (int)(9 + 10); })"
    
    got = trans input
    assert_equal output, got
    assert_equal "int _cg0;", @cg.preamble
  end
  
  def test_process_call_with_local_temp
    @info.add_c_type Type.Fixnum, "int"
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.Fixnum
      i.args = []
      i.gen do |g, s, a|
        t = g.new_temp(Type.Fixnum)
        g << t.declare_as(1)
        g << "#{s} + #{t}"
      end
    end
    
    input = [:call, [:lit, 9], :blah, [:array]]
    output = "({ int _cg0 = (int)(1);\n9 + _cg0; })"
    
    got = trans input
    assert_equal output, got
    assert_equal "", @cg.preamble
  end
  
  def test_process_call_with_block
    @info.add_c_type Type.Fixnum, "int"
    @info.add(:blah, Type.Fixnum) do |i|
      i.type = Type.void
      i.args = []
      i.block = true
      i.gen do |g, s, a|
        block = a.pop
        g << "if(FALSE) {\n#{block}\n}"
      end
    end
    
    input = [:iter, 
      [:call, [:lit, 9], :blah, [:array]],
      nil,
      [:block, [:and, [:true], [:false]]]
    ]
    
    output = "({ if(FALSE) {\n((TRUE) && (FALSE));\n}; })"
    
    got = trans input
    assert_equal output, got
    assert_equal "", @cg.preamble
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
    assert_equal "struct _locals1 _locals, *locals = &_locals;\nlocals->blah = blah;", @cg.preamble
  end
  
  def test_process_lvar
    trans [:lasgn, :blah, 2, [:lit, 8]]
    
    out = trans [:lvar, :blah, 2]
    
    assert_equal "locals->blah", out
  end
  
  def test_process_ivar
    out = trans [:ivar, :@blah]
    assert_equal "self->blah", out
  end
  
  def test_process_iasgn
    out = trans [:iasgn, :@blah, [:lit, 8]]
    assert_equal "self->blah = 8", out
  end
  
  def test_process_scope
    input = [:scope, [:block,
      [:lit, 8]
    ], []]
    
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
  
  def test_process_inlined_shows_up_in_compound
    load_types()
    input = [:while, [:call, [:lit, 1], :>, [:array, [:lit, 10]]],
        [:lasgn, :i, 2, [:lit, 11]]
      ]
    
    output = "while(({ 1 > 10; })) {\n  locals->i = 11;\n}"
    out = trans input
    
    assert_equal output, out
  end
  
  def test_fixnum_upto
    @info.add_c_type Type.Fixnum, "int"
    load_types()
    input = [:iter, 
      [:call, [:lit, 5], :upto, [:array, [:lit, 10]]],
      nil,
      [:call, [:lit, 1], :+, [:array, [:lit, 1]]]
    ]
    
    out = trans input
    output = "({ int _cg0 = (int)(5);\nwhile(_cg0 <= 10) {\n({ 1 + 1; });\n_cg0++;\n}; })"
    assert_equal output, out
  end
  
  def test_fixnum_downto
    @info.add_c_type Type.Fixnum, "int"
    load_types()
    input = [:iter, 
      [:call, [:lit, 10], :downto, [:array, [:lit, 5]]],
      nil,
      [:call, [:lit, 1], :+, [:array, [:lit, 1]]]
    ]
    
    out = trans input
    output = "({ int _cg0 = (int)(10);\nwhile(_cg0 >= 5) {\n({ 1 + 1; });\n_cg0--;\n}; })"
    assert_equal output, out
  end
  
  def test_fixnum_times
    @info.add_c_type Type.Fixnum, "int"
    load_types()
    input = [:iter, 
      [:call, [:lit, 10], :times, [:array]],
      nil,
      [:call, [:lit, 1], :+, [:array, [:lit, 1]]]
    ]
    
    out = trans input
    output = "({ int _cg0 = (int)(0);\nwhile(_cg0 < 10) {\n({ 1 + 1; });\n_cg0++;\n}; })"
    assert_equal output, out
  end
  
  
end