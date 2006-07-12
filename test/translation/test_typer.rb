require 'translation/typer'
require 'test/unit'

class TestTyper < Test::Unit::TestCase
  def setup
    @fun = FunctionTable.new
    @ty = RsTyper.new(@fun)
  end
  
  def trans(x)
    @ty.process x
  end
  
  def test_true
    out = trans [:true]
    assert_equal Type.bool, out.type
  end
  
  def test_false
    out = trans [:false]
    assert_equal Type.bool, out.type
  end
  
  def test_array
    out = trans [:array]
    assert_equal Type.Array, out.type
  end
  
  def test_string
    out = trans [:str, "blah"]
    assert_equal Type.String, out.type
  end
  
  def test_call
    func = FunctionType.new "__test", Type.void
    @fun.add_function Type.unknown, :show_this, func
    
    out = trans [:call, [:self], :show_this, [:array]]
    assert_equal Type.void, out.type
  end
  
  def test_call_mismatch
        
    func = FunctionType.new "__blah", Type.void
    @fun.add_function Type.unknown, :show_this, func

    assert_nothing_raised do
      trans [:call, [:self], :show_this, [:array]]
    end
        
    assert_equal t(), func.arg_types
    
    assert_raises(RuntimeError) do
      trans [:call, [:self], :show_this, [:array, [:true]]]
    end
  end
  
  # Tests that a call can type'd before the function it calls is.
  # When the function is learned and typed, all call sites that used
  # the function has it's type update to the proper return type.
  def test_learn_function_late
    out = trans [:call, [:self], :show_this, [:array]]
    ft = @fun.find(Type.unknown, :show_this)
    assert ft.ret_type.unknown?
    assert out.type.unknown?
    
    # Make sure they're really the same object.
    assert_equal ft.ret_type.object_id, out.type.object_id
    
    func = FunctionType.new "__show_this", Type.String
    @fun.add_function Type.unknown, :show_this, func
    
    assert_equal Type.String, ft.ret_type
    assert_equal Type.String, out.type
  end
  
  def test_lasgn
    
    lt = @ty.local_vars[:blah]
    assert !lt
    
    out = trans [:lasgn, :blah, 2, [:lit, 8]]
    assert_equal Type.Fixnum, out.type
    
    lt = @ty.local_vars[:blah]
    assert_equal Type.Fixnum, lt.type
  end
  
  def test_lasgn_mismatch
    trans [:lasgn, :blah, 2, [:lit, 8]]
    
    assert_nothing_raised do
      trans [:lasgn, :blah, 2, [:lit, 8]]
    end
    
    assert_raises(RuntimeError) do
      trans [:lasgn, :blah, 2, [:lit, "blah"]]
    end
  end
  
  def test_lvar
    assert_raises(RuntimeError) do
      trans [:lvar, :blah, 2]
    end
    
    @ty.add_local_var :blah, 2, Type.Fixnum
    
    out = trans [:lvar, :blah, 2]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_iasgn
    out = trans [:iasgn, :@blah, [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_ivar
    @ty.ivars[:@blah] = Type.Fixnum
    
    out = trans [:ivar, :@blah]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_scope
    out = trans [:scope, [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_return
    out = trans [:return, [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_if
    out = trans [:if, [:true], [:lit, 8], nil]
    assert_equal Type.Fixnum, out.type
    
    out = trans [:if, [:true], [:lit, 8], [:lit, 9]]
    assert_equal Type.Fixnum, out.type
    
    out = trans [:if, [:true], [:lit, 8], [:true]]
    assert_equal [Type.Fixnum, Type.bool], out.type
  end
  
  def test_while
    out = trans [:while, [:true], [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_until
    out = trans [:until, [:true], [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
end