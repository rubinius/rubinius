require File.expand_path(File.dirname(__FILE__) + '/../test_helper')
require 'translation/typer'
require 'translation/extractors'

class TestTyper < Test::Unit::TestCase
  def setup
    @ti = TypeInfo.new
    @ty = RsTyper.new(@ti)
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
  
  def test_self
    out = trans [:self]
    assert out.type.unknown?
    
    out = trans [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defn, :me, [:args], [:scope, [:block, [:self]]]]
    ]]]
    
    assert_equal Type.Box, out.last.last.last.last.last.last.type
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
    func = Function.new :show_this, [], [], Type.void
    @ti.add_klass :Test, :Object
    @ti.add_function Type.Test, func
    @ty.self_type << Type.Test
    
    out = trans [:call, [:self], :show_this, [:array]]
    assert_equal Type.void, out.type
  end
  
  def test_new_creates_type
    @ty.consts[:RObject] = Type.Class
    out = trans [:call, [:const, :RObject], :new, [:array, [:lit, 88]]]
    assert_equal Type.RObject, out.type
  end
  
  def test_call_mismatch
    
    func = Function.new :show_this, [], [], Type.void
    @ti.add_klass :Test, :Object
    @ti.add_function Type.Test, func
    @ty.self_type << Type.Test

    assert_nothing_raised do
      trans [:call, [:self], :show_this, [:array]]
    end
        
    assert_equal [], func.args.to_a
    
    assert_raises(RuntimeError) do
      trans [:call, [:self], :show_this, [:array, [:true]]]
    end
  end
  
  # Tests that a call can type'd before the function it calls is.
  # When the function is learned and typed, all call sites that used
  # the function has it's type update to the proper return type.
  def test_learn_function_late
    @ti.add_klass :Test, :Object
    @ty.self_type << Type.Test
    out = trans [:call, [:self], :show_this, [:array]]
    ft = @ti.find(Type.Test, :show_this)
    assert ft.type.unknown?
    assert out.type.unknown?
    
    # Make sure they're really the same object.
    assert_equal ft.type.object_id, out.type.object_id
    
    func = Function.new :show_this, [], [], Type.String
    @ti.add_function Type.Test, func
    
    assert_equal Type.String, func.type
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
  
  
  def test_const
    @ty.consts[:Blah] = Type.Fixnum
    out = trans [:const, :Blah]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_iasgn
    out = trans [:iasgn, :@blah, [:lit, 8]]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_ivar
    @ty.ivars["blah"] = Type.Fixnum
    
    out = trans [:ivar, :@blah]
    assert_equal Type.Fixnum, out.type
  end
  
  def test_ivar_late
    out = trans [:ivar, :@blah]
    assert out.type.unknown?
    out2 = trans [:iasgn, :@blah, [:lit, 8]]
    assert_equal Type.Fixnum, out2.type
    assert_equal out.type, out2.type
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
  
  def test_case
    cs = [:case, [:true], [
      [:when, [:array, [:lit, 1]], [:lit, 9]],
      [:when, [:array, [:lit, 2]], [:true]]
    ]]
    
    out = trans cs
    assert_equal [Type.Fixnum, Type.bool], out.type
    
    cs = [:case, [:true], [
      [:when, [:array, [:lit, 1]], [:lit, 9]],
      [:when, [:array, [:lit, 2]], [:lit, 10]]
    ]]
    
    out = trans cs
    assert_equal Type.Fixnum, out.type
  end
  
  def test_class
    sx = [:class, [:colon2, :Box], nil, [:scope, [:block,
      
    ]]]
    
    out = trans sx
    assert_equal Type.void, out.type
  end
  
  def test_module
    sx = [:module, [:colon2, :A], [:scope, [:block, [:true], [:false]]]]
    out = trans sx
    assert_equal Type.void, out.type
  end
  
  def test_defn
    sx = [:defn, :get, [:args], [:scope, [:block, [:true]]]]
    out = trans sx
    assert_equal Type.void, out.type
  end
  
  def test_ivar_per_class
    cl1 = [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defn, :get, [:args], [:scope, [:block,
        [:ivar, :@age]
      ]]],
      [:defn, :go, [:args], [:scope, [:block, 
        [:iasgn, :@age, [:lit, 1]]
      ]]]
    ]]]
    
    cl2 = [:class, [:colon2, :Box2], nil, [:scope, [:block,
      [:defn, :get2, [:args], [:scope, [:block,
        [:ivar, :@age]
      ]]],
      
      [:defn, :go2, [:args], [:scope, [:block,
        [:iasgn, :@age, [:str, "blah"]]
      ]]]
    ]]]
    
    sx = [:scope, [:block, cl1, cl2]]
    
    out = trans sx
    
    box = @ti.classes[:Box]
    box2 = @ti.classes[:Box2]
    
    assert box
    assert box2
    assert_equal [:get, :go], @ti.classes[:Box].defined_methods.keys.sort { |a,b|
      a.to_s <=> b.to_s
    }
    assert_equal [:get2, :go2], @ti.classes[:Box2].defined_methods.keys.sort { |a,b|
      a.to_s <=> b.to_s
    }
    
    m1 = box.defined_methods[:get]
    assert_equal Type.Fixnum, m1.type
    assert_equal [], m1.args
    
    m2 = box.defined_methods[:go]
    assert_equal Type.Fixnum, m2.type
    assert_equal [], m2.args
    
    m3 = box2.defined_methods[:get2]
    assert_equal Type.String, m3.type
    assert_equal [], m3.args
    
    m4 = box2.defined_methods[:go2]
    assert_equal Type.String, m4.type
    assert_equal [], m4.args
    
    b1 = box.defined_methods[:get].body
    b2 = box2.defined_methods[:get2].body
    
    b3 = box.defined_methods[:go].body
    b4 = box2.defined_methods[:go2].body
        
    assert_equal Type.Fixnum, b1.type
    assert_equal Type.String, b2.type
    
    # Wow, it's like descending into hell!
    # d1 = out[1][1][3][1][1][2]
    # d2 = out[1][2][3][1][1][2]
    
    # assert_equal Type.Fixnum, d1.type
    # assert_equal Type.String, d2.type
    
    assert_equal [:block, [:ivar, :@age]], b1.to_a
    assert_equal [:block, [:ivar, :@age]], b2.to_a
    
    s1 = [:block, [:iasgn, :@age, [:lit, 1]]]
    s2 = [:block, [:iasgn, :@age, [:str, "blah"]]]
    
    assert_equal Type.Fixnum, b3.type
    assert_equal Type.String, b4.type
    
    assert_equal s1, b3.to_a
    assert_equal s2, b4.to_a
  end
  
  def test_subclass
    cl1 = [:class, [:colon2, :Box], [:const, :Container], [:scope, [:block,

    ]]]
    
    out = trans cl1
    assert_equal :Container, @ti.classes[:Box].superklass
  end
  
  def test_arguments_are_seen
    sx = [:defn, :get, [:args, [:a, :b, :c], [], [], nil], [:scope, [:block,
      [:lvar, :a, 2]
    ]]]
    
    out = trans sx
  end
  
  def test_return_with_nil
    sx = [:return]
    out = trans sx
  end
  
  def test_yield
    sx = [:yield]
    out = trans sx
  end
  
  def test_ivar_uses_best_type
    cl1 = [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:iasgn, :@age, [:lit, 1]]
      ]]],
    
      [:defn, :get, [:args, [:a], [], [], nil], [:scope, [:block,
        [:iasgn, :@age, [:lvar, :a, 2]]
      ]]],
    ]]]
    
    out = trans cl1
    
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, box.defined_methods[:go].type
    assert_equal Type.Fixnum, box.defined_methods[:get].type
  end
  
  def test_ivar_when_defined_late
    cl1 = [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:ivar, :@age]
      ]]],
    
      [:defn, :get, [:args, [:a], [], [], nil], [:scope, [:block,
        [:iasgn, :@age, [:lit, 2]]
      ]]],
    ]]]
    
    out = trans cl1
    
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, box.defined_methods[:get].type
    assert_equal Type.Fixnum, box.defined_methods[:go].type
  end
  
  require 'pp'
  
  def test_defined_method_late
    cl1 = [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:call, [:self], :get, [:array]]
      ]]],
    
      [:defn, :get, [:args, [], [], [], nil], [:scope, [:block,
        [:lit, 2]
      ]]],
    ]]]
    
    out = trans cl1
    
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, box.defined_methods[:get].type
    assert_equal Type.Fixnum, box.defined_methods[:go].type
  end

  def test_super
    
    cl1 = [:class, [:colon2, :Container], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:lit, 10]
      ]]]
    ]]]

    cl2 = [:class, [:colon2, :Box], [:const, :Container], [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:super, [:array]]
      ]]]
    ]]]
    
    sx = [:scope, [:block, cl1, cl2]]
    
    out = trans sx
    con = @ti.classes[:Container]
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, con.defined_methods[:go].type
    assert_equal Type.Fixnum, box.defined_methods[:go].type
    
    
  end
  
  def test_super_class_defined_2nd
    
    cl1 = [:class, [:colon2, :Container], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:lit, 10]
      ]]]
    ]]]

    cl2 = [:class, [:colon2, :Box], [:const, :Container], [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:super, [:array]]
      ]]]
    ]]]
    
    sx = [:scope, [:block, cl2, cl1]]
    
    out = trans sx

    con = @ti.classes[:Container]
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, con.defined_methods[:go].type
    assert_equal Type.Fixnum, box.defined_methods[:go].type
  end
  
  def test_ivar_from_super
    
    cl1 = [:class, [:colon2, :Container], nil, [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:iasgn, :@name, [:lit, 100]]
      ]]]
    ]]]

    cl2 = [:class, [:colon2, :Box], [:const, :Container], [:scope, [:block,
      [:defn, :go, [:args], [:scope, [:block, 
        [:ivar, :@name]
      ]]]
    ]]]
    
    sx = [:scope, [:block, cl1, cl2]]
    
    out = trans sx

    con = @ti.classes[:Container]
    box = @ti.classes[:Box]
    assert_equal Type.Fixnum, con.defined_methods[:go].type
    assert_equal Type.Fixnum, box.defined_methods[:go].type
  end
  
  def test_defs_on_self
    cl1 = [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:defs, [:self], :blah, [:args], [:scope, [:block,
        [:lit, 10]
      ]]]
    ]]]
    
    out = trans cl1

    con = @ti.classes[:BoxMeta]
    assert con
    assert_equal Type.Fixnum, con.defined_methods[:blah].type
  end
  
  def test_not
    sx = [:not, [:lit, 10]]
    
    out = trans sx
    
    assert_equal Type.bool, out.type
  end
  
  def test_and
    sx = [:and, [:true], [:false]]
    
    out = trans sx
    assert_equal Type.bool, out.type
  end
  
  def test_attrasgn
    sx = [:attrasgn, [:self], :go=, [:array, [:lit, 10]]]
    
    out = trans sx
    assert out.type.unknown?
  end
  
  def test_iter_with_arg
    sx = [:iter, 
      [:call, [:lit, 0], :upto, [:array, [:lit, 10]]],
      [:lasgn, :i, 2], 
      [:block, [:lvar, :i, 2]]
    ]
    
    assert_nothing_raised do
      out = trans sx
    end    
  end
  
  def test_iter_and_yield_propagate
    iter = [:iter, 
      [:call, [:self], :go, [:array, [:lit, 10]]],
      [:lasgn, :i, 2], 
      [:block, [:lvar, :i, 2]]
    ]
    
    sx = [:class, [:colon2, :Container], nil, [:scope, [:block,
      [:defn, :go, [:args, [:n], [], [], nil], [:scope, [:block, 
        [:yield, [:array, [:lit, 100]]]
      ]]],
      
      [:defn, :doit, [:args], [:scope, [:block,
        iter
      ]]]
    ]]]
    
    out = nil
    assert_nothing_raised do
      out = trans sx
    end
    
    con = @ti.classes[:Container]
    assert_equal Type.Fixnum, con.defined_methods[:go].type
    assert_equal Type.Fixnum, con.defined_methods[:doit].type    
  end
  
  def test_multi_type_defn
    
    me = [:defn, :go, [:args], [:scope, [:block,
      [:if, [:true], [:lit, 9], [:ivar, :running]]
    ]]]
    
    sx = [:class, [:colon2, :Box], nil, [:scope, [:block, 
      me
    ]]]
    
    out = trans sx
    
    con = @ti.classes[:Box]
    assert_equal Type.Fixnum, con.defined_methods[:go].type    
  end
  
  def test_dstr
    sx = [:dstr, "hello mr. ", [:lit, 10]]
    
    out = trans sx
    
    assert_equal Type.String, out.type
  end
  
  def test_evstr
    sx = [:evstr, [:lit, 1]]
    
    out = trans sx
    
    assert_equal Type.String, out.type
  end
  
  def test_using_type_hints
    sx = [:scope, [:block,
      [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:comment, " T:String => String"],
      [:defn, :go, [:args, [:a], [], [], nil], [:scope, [:block,
        [:lvar, :a, 2]
      ]]]
      ]]]
    ]]
    
    out = trans sx
    
    pp out
    
    con = @ti.classes[:Box]
    assert_equal Type.String, con.defined_methods[:go].type    
    
    # pp out
  end
  
  def test_using_tag_hints
    sx = [:scope, [:block,
      [:class, [:colon2, :Box], nil, [:scope, [:block,
      [:comment, "hint:rocks"],
      [:defn, :go, [:args, [:a], [], [], nil], [:scope, [:block,
        [:ivar, :@name]
      ]]]
      ]]]
    ]]
    
    out = trans sx
    
    con = @ti.classes[:Box]
    assert_equal ['rocks'], con.defined_methods[:go].tags
    
    # pp out
  end
  
  def test_arg_types
    sx = [:defn, :go, [:args, [:a], [], [], nil],
      [:scope, [:block,
        [:lasgn, :a, 2, [:lit, 10]]
      ]]
    ]
    
    out = trans sx
    pp out
  end
  
end

