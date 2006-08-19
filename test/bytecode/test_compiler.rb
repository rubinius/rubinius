require 'rubygems'
require 'bytecode/assembler'
require 'test/unit'
require 'test/unit/show_code'
require 'bytecode/compiler'
require 'pp'

class TestBytecodeMethodDescription < Test::Unit::TestCase
  def test_encode_literals
    cpu = CPU.new
    cpu.bootstrap
    md = Bytecode::MethodDescription.new(:blah)
    md.add_literal :eek
    md.add_literal :ok
    out = md.encode_literals
    
    assert_equal CPU::Global.tuple.address, out.rclass.address
    assert_equal 2, out.fields
    assert_equal ":eek", out.at(0).as(:symbol).as_string
    assert_equal ":ok", out.at(1).as(:symbol).as_string
  end
end

class TestBytecodeCompiler < Test::Unit::TestCase
  def setup
    @comp = Bytecode::Compiler.new
  end
  
  def show_input(x)
    comp = CompositeSexpProcessor.new
    # Convert the block args to the new rules...
    comp << RsLocalScoper.new
    # normalize the rest of the sexp...
    comp << RsNormalizer.new
    
    pp comp.process(x)
  end
    
  def test_compile_as_method
    sx = [:and, [:true], [:false]]
    out = @comp.compile_as_method(sx, :blah)
    assert_kind_of Bytecode::MethodDescription, out
    assert_equal :blah, out.name
    assert_kind_of String, out.assembly
  end
  
  def compile(sx)
    @meth = @comp.compile_as_method(sx, :test)
  end
  
  def test_process_true
    sx = [:true]
    compile sx
    assert_equal "push true\nret\n", @meth.assembly
  end
  
  def test_process_false
    compile [:false]
    assert_equal "push false\nret\n", @meth.assembly
  end
  
  def test_process_nil
    compile [:nil]
    assert_equal "push nil\nret\n", @meth.assembly
  end
  
  def test_process_or
    compile [:or, [:true], [:false]]
    
    exc = "push true\ngit lbl1\npush false\nlbl1:\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_not
    compile [:not, [:true]]
    exc = "push true\ngit lbl1\npush true\ngoto lbl2\nlbl1:\npush false\nlbl2:\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_lit_for_num
    compile [:lit, 9]
    exc = "push 9\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_lit_for_sym
    compile [:lit, :blah]
    exc = "push :blah\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_if
    sx = [:if, [:true], [:lit, 9], [:lit, 10]]
    exc = "push true\ngif lbl1\npush 9\ngoto lbl2\nlbl1:\npush 10\nlbl2:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_if_no_else
    sx = [:if, [:true], [:lit, 9]]
    exc = "push true\ngif lbl1\npush 9\nlbl1:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_if_no_then
    sx = [:if, [:true], nil, [:lit, 10]]
    exc = "push true\ngit lbl1\npush 10\nlbl1:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_block
    sx = [:block, [:true], [:lit, 11]]
    exc = "push true\npush 11\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_scope
    sx = [:scope, [:true]]
    exc = "push true\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_while
    sx = [:while, [:true], [:lit, 10]]
    exc = "lbl1:\npush true\ngif lbl2\npush 10\ngoto lbl1\nlbl2:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_until
    sx = [:until, [:true], [:lit, 10]]
    exc = "lbl1:\npush true\ngit lbl2\npush 10\ngoto lbl1\nlbl2:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_str
    sx = [:str, "blah"]
    exc = "push_literal 0\n"
  end
  
  def test_process_lasgn
    sx = [:lasgn, :x, 8, [:false]]
    exc = "push false\nset x:2\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_lvar
    sx = [:lvar, :x, 8]
    exc = "push x:2\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_array
    sx = [:array, [:lit, 99]]
    exc = "push 99\nmake_array 1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_zarray
    sx = [:zarray]
    exc = "make_array 0\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_to_ary
    sx = [:to_ary, [:lit, 8]]
    exc = "push 8\ncast_array\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_rescue_simple
    sx = [:rescue, [:true], [:resbody, nil, [:lit, 2], nil]]
    exc =  "#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl3\npush 2\ngoto lbl2\nlbl3:\npush_exception\nraise\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_rescue_two_resbodies
    sx = [:rescue, [:true], [:resbody, nil, [:lit, 2], [:resbody, nil, [:lit, 3]]]]
    exc =  "#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl4\npush 2\ngoto lbl2\n"
    exc << "lbl4:\npush_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl3\npush 3\ngoto lbl2\n"
    exc << "lbl3:\npush_exception\nraise\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_rescue_mulitple_classes
    sx = [:rescue, [:true], [:resbody, 
      [:array, [:const, :Blah], [:const, :Bleh]], [:lit, 4]]]
    compile sx
    exc =  "#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush Blah\nsend === 1\n"
    exc << "git lbl4\npush_exception\npush Bleh\nsend === 1\n"
    exc << "gif lbl3\n"
    exc << "lbl4:\npush 4\ngoto lbl2\n"
    exc << "lbl3:\npush_exception\nraise\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_argscat
    sx = [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]
    exc = "push 99\ncast_array\npush_array\npush 2\npush 1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_ivar
    sx = [:ivar, :@blah]
    exc = "push @blah\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_iasgn
    sx = [:iasgn, :@blah, [:lit, 99]]
    exc = "push 99\nset @blah\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_hash
    sx = [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    exc = "push 4\npush 3\npush 2\npush 1\nmake_hash 4\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_colon2
    sx = [:colon2, [:const, :B], :A]
    compile sx
    assert_equal "push B\nfind A\nret\n", @meth.assembly
  end
  
  def test_colon3
    sx = [:colon3, :A]
    compile sx
    assert_equal "push_cpath_top\nfind A\nret\n", @meth.assembly
  end
  
  def test_const
    sx = [:const, :A]
    compile sx
    assert_equal "push A\nret\n", @meth.assembly
  end
  
  def test_process_class
    sx = [:class, [:colon2, :Blah], nil, [:scope, [:true]]]
    exc =  "push nil\nopen_class Blah\ndup\npush_literal 0\nswap\n"
    exc << "attach __class_init__\nsend __class_init__\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_class_with_super
    sx = [:class, [:colon2, :Blah], [:colon2, [:const, :A], :B], [:scope, [:true]]]
    exc =  "push A\nfind B\n"
    exc << "open_class Blah\ndup\npush_literal 0\nswap\n"
    exc << "attach __class_init__\nsend __class_init__\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly    
  end
  
  def test_process_class_at_cpath
    sx = [:class, [:colon2, [:const, :A], :Blah], nil, [:scope, [:true]]]
    exc =  "push A\npush nil\nopen_class_under Blah\ndup\npush_literal 0\nswap\n"
    exc << "attach __class_init__\nsend __class_init__\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_module
    sx = [:module, [:colon2, :A], [:scope, [:true]]]
    exc =  "open_module A\ndup\npush_literal 0\nswap\n"
    exc << "attach __module_init__\nsend __module_init__\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_module_at_cpath
    sx = [:module, [:colon2, [:const, :B], :A], [:scope, [:true]]]
    exc =  "push B\nopen_module_under A\ndup\npush_literal 0\nswap\n"
    exc << "attach __module_init__\nsend __module_init__\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_return
    sx = [:return, [:lit, 8]]
    exc =  "push 8\nret\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_ensure
    sx = [:ensure, [:lit, 10], [:lit, 11]]
    exc =  "#exc_start exc1\npush 10\n"
    exc << "#exceptions exc1\npush 11\n"
    exc << "push_exception\ngif lbl2\npush_exception\nraise\n"
    exc << "lbl2:\n#exc_end exc1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_defn
    sx = [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    
    compile sx
    exc = "push_self\npush_literal 0\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 = "check_argcount 2 2\nset a\nset b\npush true\nret\n"
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defn_with_splat
    sx = [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], [:c, 4], nil],
        [:true]],
      [:a, :b]
    ]]
    
    compile sx
    exc = "push_self\npush_literal 0\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 =  "check_argcount 2 0\nset a\nset b\nmake_rest 2\nset c\n"
    exc2 << "push true\nret\n"
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defn_with_default
    sx = [:defn, :blah,
     [:scope,
      [:block,
       [:args, [:a], [:b], nil, [:block, [:lasgn, :b, 3, [:lit, 9]]]],
       [:false],
       [:true]],
      [:a, :b]]]
      
    exc = "push_self\npush_literal 0\nadd_method blah\nret\n"
    exc2 =  "check_argcount 1 2\n"
    exc2 << "set a\npassed_arg 1\ngit lbl1\npush 9\n"
    exc2 << "lbl1:\nset b\npush false\npush true\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defn_with_block_arg
    sx = [:defn, :blah,
     [:scope,
      [:block,
       [:args],
       [:block_arg, :b, 2],
       [:true]],
      [:b]]]
      
    exc = "push_self\npush_literal 0\nadd_method blah\nret\n"
    exc2 =  "check_argcount 0 0\n"
    exc2 << "push_block\nset b:2\npush true\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_masgn_no_splat
    sx = [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
      nil,
      [:to_ary, [:lit, 8]]]
    
    exc =  "push 8\ncast_tuple\n"
    exc << "unshift_tuple\nset a:2\n"
    exc << "unshift_tuple\nset b:3\n"
    exc << "unshift_tuple\nset c:4\n"
    exc << "ret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_masgn_with_splat
    sx = [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
      [:lasgn, :c, 4],
      [:to_ary, [:lit, 8]]]
    
    exc =  "push 8\ncast_tuple\n"
    exc << "unshift_tuple\nset a:2\n"
    exc << "unshift_tuple\nset b:3\n"
    exc << "cast_array\nset c:4\n"
    exc << "ret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_masgn_with_array_as_source
    sx = [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     nil,
     [:array, [:lit, 99], [:lit, 8]]]
     
    exc =  "push 99\nset a:2\npush 8\nset b:3\nret\n"

    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_masgn_with_array_as_source_too_many_lhs
    sx = [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
     nil,
     [:array, [:lit, 99], [:lit, 8]]]
     
    assert_raises(RuntimeError) do
     compile sx
    end
  end
  
  def test_process_masgn_with_array_as_source_too_many_rhs
    sx = [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     nil,
     [:array, [:lit, 99], [:lit, 8], [:lit, 93]]]
     
    assert_raises(RuntimeError) do
     compile sx
    end
  end
  
  def test_process_masgn_with_array_as_source_splat_is_stupid
    sx = [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     [:lasgn, :c, 4],
     [:array, [:lit, 99], [:lit, 8]]]
     
    assert_raises(RuntimeError) do
     compile sx
    end
  end
  
  def test_process_call
    sx = [:call, [:self], :b, [:array, [:lit, 1], [:lit, 2]]]
    
    exc = "push 2\npush 1\npush self\nsend b 2\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_block_pass
    sx = [:block_pass, [:lit, 10], [:fcall, :d, [:array, [:lit, 9]]]]
    exc = "push 9\npush 10\npush self\n&send d 1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_iter
    sx = [:iter, 
      [:fcall, :m],
      [:dasgn_curr, :a],
      [:block, [:true], [:fcall, :p, [:array, [:lit, 2]]]]
    ]
    
    exc =  "push &lbl1\n"
    exc << "push_context\nsend_primitive create_block\n"
    exc << "goto lbl2\nset a:2\npush true\npush 2\n"
    exc << "push self\nsend p 1\n"
    exc << "lbl1: soft_return\nlbl2:\n"
    exc << "push self\n&send m 0\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
end