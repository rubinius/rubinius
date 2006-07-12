require 'translation/to_c'
require 'test/unit'

class TestRsToC < Test::Unit::TestCase
  def setup
    @rc = RsToC.new
    @fun = @rc.functions
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
    ft = FunctionType.new "__blah", Type.int
    @fun.add_function Type.unknown, :blah, ft
    input = [:call, [:self], :blah, [:array]]
    output = "__blah(self, NO_BLOCK)"
    
    got = trans input
    assert_equal output, got
    
    ft.reset_args!
    
    input = [:call, [:self], :blah, [:array, [:true], [:false]]]
    output = "__blah(self, NO_BLOCK, TRUE, FALSE)"
    
    got = trans input
    assert_equal output, got
  end
  
  def test_process_iter
    
    ft = FunctionType.new "__show_this", Type.int
    @fun.add_function Type.unknown, :show_this, ft
    
    input = [:iter,
      [:call, [:self], :show_this, [:array]],
      nil,
      [:block, [:true]]
    ]
    
    output = "BOOL _rstoc_1() { return TRUE; }\n"
    output << "__show_this(self, _rstoc_1)"
    
    got = trans input
    assert_equal output, got
  end
  
  def test_process_lasgn
    out = trans [:lasgn, :blah, 2, [:lit, 8]]
    assert_equal "blah = 8", out
  end
  
  def test_process_lvar
    trans [:lasgn, :blah, 2, [:lit, 8]]
    
    out = trans [:lvar, :blah, 2]
    
    assert_equal "blah", out
  end
  
  def test_process_ivar
    out = trans [:ivar, :blah]
    assert_equal "self->blah", out
  end
  
  def test_process_iasgn
    out = trans [:iasgn, :blah, [:lit, 8]]
    assert_equal "self->blah = 8", out
    assert_equal Type.Fixnum, @rc.ivars[:blah]
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