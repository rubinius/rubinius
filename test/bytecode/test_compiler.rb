require 'rubygems'
require 'bytecode/assembler'
require 'test/unit'

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
    
    exc = "push true\ndup\ngit lbl1\npop\npush false\nlbl1:\nret\n"
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
  
  def test_process_lit_for_regexp
    compile [:lit, /hello/]
    exc = "push false\npush_literal 0\npush Regexp\nsend new 2\nret\n"
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
    exc = "lbl1:\npush true\ngif lbl2\nlbl3:\npush 10\ngoto lbl1\nlbl2:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_until
    sx = [:until, [:true], [:lit, 10]]
    exc = "lbl1:\npush true\ngit lbl2\nlbl3:\npush 10\ngoto lbl1\nlbl2:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_str
    sx = [:str, "blah"]
    exc = "push_literal 0\nstring_dup\n"
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
    exc =  "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl3\npush 2\ngoto lbl2\nlbl3:\npush_exception\nraise_exc\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_rescue_two_resbodies
    sx = [:rescue, [:true], [:resbody, nil, [:lit, 2], [:resbody, nil, [:lit, 3]]]]
    exc =  "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl5\npush 2\ngoto lbl2\n"
    exc << "lbl5:\npush_exception\npush RuntimeError\nsend === 1\n"
    exc << "gif lbl3\npush 3\ngoto lbl2\n"
    exc << "lbl3:\npush_exception\nraise_exc\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_rescue_mulitple_classes
    sx = [:rescue, [:true], [:resbody, 
      [:array, [:const, :Blah], [:const, :Bleh]], [:lit, 4]]]
    compile sx
    exc =  "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n"
    exc << "push_exception\npush Blah\nsend === 1\n"
    exc << "git lbl5\npush_exception\npush Bleh\nsend === 1\n"
    exc << "gif lbl3\n"
    exc << "lbl5:\npush 4\ngoto lbl2\n"
    exc << "lbl3:\npush_exception\nraise_exc\n"
    exc << "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
    assert_equal exc, @meth.assembly
  end
  
  def test_process_argscat
    sx = [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]
    exc = "push 99\ncast_array_for_args 2\npush_array\npush 2\npush 1\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_call_with_argscat
    sx = [:call, [:self], :b, [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]]
    
    exc =  "push 99\ncast_array_for_args 2\npush_array\n"
    exc << "push 2\npush 1\npush nil\npush self\nsend b +\nret\n"
        
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_yield
    sx = [:yield, [:array, [:lit, 1], [:lit, 2]]]
    compile sx
    
    exc = "push 2\npush 1\npush_block\nsend call 2\nret\n"
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
  
  def test_ivar_as_index
    sx = [:class, [:colon2, :B], nil, 
      [:scope, [:block,
        [:fcall, :ivar_as_index, [:array, 
          [:ihash, [:lit, :name], [:lit, 1]]
        ]],
        
        [:defn, :blah, [:args, [], [], nil, nil], [:scope, [:block,
          [:ivar, :@name],
          [:iasgn, :@name, [:lit, 11]]
        ]]]
      ]]
    ]
    
    compile sx

    # Check the __class_init__
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    exc =  "push self\nset_encloser\npush_literal 0\n"
    exc << "push_self\nadd_method blah\nret\n"
    
    assert_equal exc, m.assembly
    
    m2 = m.literals.first
    exc2 =  "check_argcount 0 0\npush_my_field 1\npush 11\nstore_my_field 1\nret\n"
    assert_equal exc2, m2.assembly
    
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
  
  def test_cdecl
    sx = [:cdecl, :Blah, [:lit, 8], nil]
    compile sx
    assert_equal "push 8\nset Blah\nret\n", @meth.assembly
  end
  
  def test_cdecl_with_path
    sx = [:cdecl, nil, [:lit, 8], [:colon2, [:const, :A], :Blah]]
    compile sx
    assert_equal "push A\npush 8\nset +Blah\nret\n", @meth.assembly
  end
  
  def test_process_class
    sx = [:class, [:colon2, :Blah], nil, [:scope, [:true]]]
    exc =  "push nil\nopen_class Blah\ndup\npush_literal 0\nswap\n"
    exc << "attach __class_init__\npop\nsend __class_init__\n"
    exc << "pop\npush_encloser\nret\n"
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
    exc << "attach __class_init__\npop\nsend __class_init__\n"
    exc << "pop\npush_encloser\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly    
  end
  
  def test_process_class_at_cpath
    sx = [:class, [:colon2, [:const, :A], :Blah], nil, [:scope, [:true]]]
    exc =  "push A\npush nil\nopen_class_under Blah\ndup\npush_literal 0\nswap\n"
    exc << "attach __class_init__\npop\nsend __class_init__\n"
    exc << "pop\npush_encloser\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_module
    sx = [:module, [:colon2, :A], [:scope, [:true]]]
    exc =  "open_module A\ndup\npush_literal 0\nswap\n"
    exc << "attach __module_init__\npop\nsend __module_init__\n"
    exc << "pop\npush_encloser\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
    m = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, m
    assert_equal "push self\nset_encloser\npush true\nret\n", m.assembly
  end
  
  def test_process_module_at_cpath
    sx = [:module, [:colon2, [:const, :B], :A], [:scope, [:true]]]
    exc =  "push B\nopen_module_under A\ndup\npush_literal 0\nswap\n"
    exc << "attach __module_init__\npop\nsend __module_init__\n"
    exc << "pop\npush_encloser\nret\n"
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
    exc << "push_exception\ngif lbl2\npush_exception\nraise_exc\n"
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
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 = "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\nret\n"
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defn_with_splat
    sx = [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], [:c, 4], nil],
        [:true]],
      [:a, :b]
    ]]
    
    compile sx
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 =  "check_argcount 2 0\nset a:2\npop\nset b:3\npop\nmake_rest 2\nset c\npop\n"
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
      
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    exc2 =  "check_argcount 1 2\n"
    exc2 << "set a:2\npop\npassed_arg 1\ngit set1\npush 9\n"
    exc2 << "set1:\nset b:3\npop\npush false\npush true\nret\n"
    
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
      
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    exc2 =  "check_argcount 0 0\n"
    exc2 << "push_block\npush Proc\nsend from_environment 1\nset b:2"
    exc2 << "\npop\npush true\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defn_with_primitive
    sx = [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:call, [:const, :Ruby], :primitive, [:array, [:lit, :at]]],
        [:true]],
      [:a, :b]
    ]]
    
    compile sx
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 = "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\nret\n"
    assert_equal exc2, dfn.assembly
    assert_equal :at, dfn.primitive
  end
  
  def test_process_defn_with_inline_bytecode
    sx = [:defn, :blah, [:scope, [:block,
      [:args, [:a, :b], [], nil, nil],
      [:true],
      [:call, [:const, :Ruby], :asm, [:array, [:str, "push self"]]]],
        [:a, :b]
    ]]
    
    compile sx
    exc = "push_literal 0\npush_self\nadd_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 = "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\npush self\nret\n"
    assert_equal exc2, dfn.assembly
  end
  
  def test_process_defs
    sx = [:defs, [:const, :Object], :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    
    compile sx
    exc = "push_literal 0\npush Object\nattach_method blah\nret\n"
    assert_equal exc, @meth.assembly
    dfn = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, dfn
    exc2 = "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\nret\n"
    assert_equal exc2, dfn.assembly
  end
    
  def test_process_masgn_no_splat
    sx = [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
      nil,
      [:to_ary, [:lit, 8]]]
    
    exc =  "push 8\ncast_tuple\n"
    exc << "unshift_tuple\nset a:2\npop\n"
    exc << "unshift_tuple\nset b:3\npop\n"
    exc << "unshift_tuple\nset c:4\npop\n"
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
    exc << "unshift_tuple\nset a:2\npop\n"
    exc << "unshift_tuple\nset b:3\npop\n"
    exc << "cast_array\nset c:4\npop\n"
    exc << "ret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_masgn_with_array_as_source
    sx = [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     nil,
     [:array, [:lit, 99], [:lit, 8]]]
     
    exc =  "push 99\nset a:2\npop\npush 8\nset b:3\npop\nret\n"

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
    exc = "push 9\npush 10\npush self\n&send d 1\nlbl1:\nret\n"
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_iter
    sx = [:iter, 
      [:fcall, :m],
      [:dasgn_curr, :a],
      [:block, [:true], [:fcall, :p, [:array, [:lit, 2]]]]
    ]
    
    exc =  "push &lbl1\npush &lbl2\n"
    exc << "push_context\nsend_primitive create_block\n"
    exc << "goto lbl3\nset a:2\npush true\npush 2\n"
    exc << "push self\nsend p 1\n"
    exc << "lbl2: soft_return\nlbl3:\n"
    exc << "push self\n&send m 0\nlbl1:\nret\n"
    
    compile sx
    assert_equal exc, @meth.assembly
  end
  
  def test_process_str
    sx = [:str, "blah"]
    compile sx
    assert_equal "push_literal 0\nstring_dup\nret\n", @meth.assembly
  end
  
  def test_process_dstr
    sx = [:dstr, "blah ", [:evstr, [:lit, 1]], [:str, " more "], [:evstr, [:lit, 2]]]
    compile sx
    
    exc =  "push 2\nsend to_s\npush_literal 0\nstring_dup\npush 1\n"
    exc << "send to_s\npush_literal 1\n"
    exc << "string_dup\nstring_append\nstring_append\n"
    exc << "string_append\nret\n"
    
    assert_equal exc, @meth.assembly
  end
  
  def test_process_newline
    sx = [:newline, 7, "blah.rb", [:true]]
    compile sx
    exc = "\#line 7\npush true\nret\n"
    assert_equal exc, @meth.assembly
    assert_equal "blah.rb", @meth.file
  end
  
  def test_correct_lvars1
    
    m1 = [:defn, :blah, [:scope, [:block, 
      [:args, [], [:fuck], nil, nil], [:true]], []]]
    m2 = [:defn, :to_s,
     [:scope,
      [:block,
       [:args, [], [:base], nil, [:block, [:lasgn, :base, 2, [:lit, 10]]]],
       [:fcall, :based_to_s, [:array, [:lvar, :base, 2]]]], [:base]]]
    
    sx = [:class, [:colon2, :Fixnum], nil,
     [:scope, [:block,
      m1,
      m2,
      ], []]]
      
    @meth = @comp.compile_as_script(sx, :test)
    
    cls = @meth.literals.first
    assert_kind_of Bytecode::MethodDescription, cls
    dfn = cls.literals.last
    exc =  "check_argcount 0 1\npassed_arg 0\ngit set1\npush 10\nset1:\nset base:2\n"
    exc << "pop\npush base:2\npush self\nsend based_to_s 1\nret\n"
    
    assert_equal exc, dfn.assembly
  end
  
  def test_case_simple
    sx = [:case, [:lit, 1], [[:when, [:array, [:const, :String]], [:lit, 9]]]]
    
    compile sx
    
    exc =  "push 1\ndup\npush String\nsend === 1\ngif lbl1\n"
    exc << "push 9\nlbl1:\nswap\npop\nret\n"
    
    assert_equal exc, @meth.assembly
  end
  
  def test_case_many_when
    sx = [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah]], [:lit, 3423]]
            ]]
    
    compile sx
    
    exc =  "push 1\ndup\npush String\nsend === 1\ngit lbl3\n"
    exc << "dup\npush Fixnum\nsend === 1\ngif lbl1\n"
    exc << "lbl3:\npush 9\ngoto lbl2\nlbl1:\n"
    exc << "dup\npush Blah\nsend === 1\ngif lbl2\n"
    exc << "push 3423\nlbl2:\nswap\npop\nret\n"
    
    assert_equal exc, @meth.assembly
  end
  
  def test_case_else
    sx = [:case, [:lit, 1], [[:when, [:array, [:const, :String]], 
      [:lit, 9]]], [:lit, 10]]
      
    compile sx
    
    exc =  "push 1\ndup\npush String\nsend === 1\ngif lbl1\n"
    exc << "push 9\ngoto lbl2\nlbl1:\npush 10\nlbl2:\nswap\npop\nret\n"
    
    assert_equal exc, @meth.assembly
  end
  
  def test_case_many_with_else
    sx = [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah], [:const, :Go]], [:lit, 3423]]
            ], [:lit, 632]]
    
    compile sx
    
    exc =  "push 1\ndup\npush String\nsend === 1\ngit lbl4\n"
    exc << "dup\npush Fixnum\nsend === 1\ngif lbl1\n"
    exc << "lbl4:\npush 9\ngoto lbl3\nlbl1:\n"
    exc << "dup\npush Blah\nsend === 1\ngit lbl5\n"
    exc << "dup\npush Go\nsend === 1\ngif lbl2\n"
    exc << "lbl5:\npush 3423\ngoto lbl3\nlbl2:\npush 632\n"
    exc << "lbl3:\nswap\npop\nret\n"
    
    assert_equal exc, @meth.assembly
  end
  
  def test_dot2
    sx = [:dot2, [:lit, 100], [:lit, 1]]
    compile sx
    
    exc = "push 1\npush 100\npush Range\nsend new 2"
  end
  
  def test_dot3
    sx = [:dot3, [:lit, 100], [:lit, 1]]
    compile sx
    
    exc = "push false\npush 1\npush 100\npush Range\nsend new 3"
    
  end
end
