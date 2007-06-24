require File.dirname(__FILE__) + '/../spec_helper'

require 'bytecode/assembler'
require 'bytecode/compiler'

describe Bytecode::Compiler do
  before do
    @compiler = Bytecode::Compiler.new
  end
  
  def compile(sexp)
    @method = @compiler.compile_as_method(sexp, :test)
  end
  
  it "should compile the sexpression to a method" do
    compile [:and, [:true], [:false]]
    @method.kind_of?(Bytecode::MethodDescription).should == true
    @method.name.should == :test
    @method.assembly.kind_of?(String).should == true
  end
  
  it "should compile true" do
    compile [:true]
    @method.assembly.should == "push true\nret\n"
  end
  
  it "should compile false" do
    compile [:false]
    @method.assembly.should == "push false\nret\n"
  end
  
  it "should compile nil" do
    compile [:nil]
    @method.assembly.should == "push nil\nret\n"
  end
  
  it "should compile or" do
    compile [:or, [:true], [:false]]
    @method.assembly.should == 
      "push true\ndup\ngit lbl1\npop\npush false\nlbl1:\nret\n"
  end
  
  it "should compile not" do
    compile [:not, [:true]]
    @method.assembly.should == 
      "push true\ngit lbl1\npush true\ngoto lbl2\nlbl1:\npush false\nlbl2:\nret\n"
  end
  
  it "should compile a number literal" do
    compile [:lit, 9]
    @method.assembly.should == "push 9\nret\n"
  end
  
  it "should compile a symbol literal" do
    compile [:lit, :blah]
    @method.assembly.should == "push_literal 0\nret\n"
  end
  
  it "should compile a regexp literal" do
    compile [:lit, /hello/]
    @method.assembly.should == 
      "push 16\npush_literal 0\npush Regexp\nsend new 2\nret\n"
  end
  
  it "should compile if" do
    compile [:if, [:true], [:lit, 9], [:lit, 10]]
    @method.assembly.should == 
      "push true\ngif lbl1\npush 9\ngoto lbl2\nlbl1:\npush 10\nlbl2:\nret\n"
  end
  
  it "should compile if with no else" do
    compile [:if, [:true], [:lit, 9]]
    @method.assembly.should == 
      "push true\ngif lbl1\npush 9\ngoto lbl2\nlbl1:\npush nil\nlbl2:\nret\n"
  end
  
  it "should compile if with no then" do
    compile [:if, [:true], nil, [:lit, 10]]
    @method.assembly.should == 
      "push true\ngit lbl1\npush 10\ngoto lbl2\nlbl1:\npush nil\nlbl2:\nret\n"
  end

  it "should compile a block" do
    compile [:block, [:true], [:lit, 11]]
    @method.assembly.should == "push true\npop\npush 11\nret\n"
  end
  
  it "should compile scope" do
    compile [:scope, [:true]]
    @method.assembly.should == "push true\nret\n"
  end
  
  it "should compile while" do
    compile [:while, [:true], [:lit, 10]]
    @method.assembly.should == 
       "lbl1:\npush true\ngif lbl2\nlbl3:\npush 10\npop\ngoto lbl1\nlbl2:\npush nil\nret\n"
  end
  
  it "should compile until" do
    compile [:until, [:true], [:lit, 10]]
    @method.assembly.should == 
      "lbl1:\npush true\ngit lbl2\nlbl3:\npush 10\npop\ngoto lbl1\nlbl2:\npush nil\nret\n"
  end
  
  it "should compile lasgn" do
    compile [:lasgn, :x, 8, [:false]]
    @method.assembly.should == "push false\nset x:2\nret\n"
  end
  
  it "should compile lvar" do
    compile [:lvar, :x, 8]
    @method.assembly.should == "push x:2\nret\n"
  end
  
  it "should compile an array literal" do
    compile [:array, [:lit, 99]]
    @method.assembly.should == "push 99\nmake_array 1\nret\n"
  end
  
  it "should compile zarray" do
    compile [:zarray]
    @method.assembly.should == "make_array 0\nret\n"
  end
  
  it "should compile to_ary" do
    compile [:to_ary, [:lit, 8]]
    @method.assembly.should == "push 8\ncast_array\nret\n"
  end
  
  it "should compile a simple rescue" do
    compile [:rescue, [:true], [:resbody, nil, [:lit, 2], nil]]
    @method.assembly.should == 
      "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n" \
      "push_exception\npush StandardError\nsend === 1\n" \
      "gif lbl3\npush 2\ngoto lbl2\nlbl3:\npush_exception\nraise_exc\n" \
      "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
  end
  
  it "should compile rescue with two resbodies" do
    compile [:rescue, [:true], [:resbody, nil, [:lit, 2], [:resbody, nil, [:lit, 3]]]]
    @method.assembly.should == 
      "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n" \
      "push_exception\npush StandardError\nsend === 1\n" \
      "gif lbl5\npush 2\ngoto lbl2\n" \
      "lbl5:\npush_exception\npush StandardError\nsend === 1\n" \
      "gif lbl3\npush 3\ngoto lbl2\n" \
      "lbl3:\npush_exception\nraise_exc\n" \
      "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
  end
  
  it "should compile rescue with multiple classes" do
    compile [:rescue, [:true], [:resbody, 
      [:array, [:const, :Blah], [:const, :Bleh]], [:lit, 4]]]
    @method.assembly.should == 
      "lbl4:\n#exc_start exc1\npush true\ngoto lbl2\n#exceptions exc1\n" \
      "push_exception\npush Blah\nsend === 1\n" \
      "git lbl5\npush_exception\npush Bleh\nsend === 1\n" \
      "gif lbl3\n" \
      "lbl5:\npush 4\ngoto lbl2\n" \
      "lbl3:\npush_exception\nraise_exc\n" \
      "lbl2:\nclear_exception\n#exc_end exc1\nret\n"
  end
  
  it "should compile argscat" do
    compile [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]
    @method.assembly.should == "push 99\ncast_array_for_args 2\npush_array\npush 2\npush 1\nret\n"
  end
  
  it "should compile call with argscat" do
    compile [:call, [:self], :b, [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]]
    @method.assembly.should == 
      "push 99\ncast_array_for_args 2\npush_array\npush 2\npush 1\nget_args\npush nil\n" \
      "swap\npush self\nswap\nset_args\nsend b +\nret\n"
  end

  it "should compile call with argscat and newline" do
    compile [:call, [:const, :Hash], :[], [:newline, 1, "(eval)", [:argscat, [:array, [:lit, 1], [:lit, 2]]]]]
    @method.assembly.should == "cast_array_for_args 2\npush_array\npush 2\npush 1\nget_args\npush nil\nswap\npush Hash\nswap\nset_args\nsend [] +\nret\n"
  end
  
  it "should compile yield" do
    compile [:yield, [:array, [:lit, 1], [:lit, 2]], false]
    @method.assembly.should == 
      "push 2\npush 1\npush_block\nsend call 2\nret\n"
  end

  it "should compile yield with a splat" do
    compile [:yield, [:splat, [:array, [:lit, 5], [:lit, 6]]], true]
    @method.assembly.should == "push 5\npush 6\nmake_array 2\ncast_array_for_args 0\npush_array\nget_args\npush_block\nsend call +\nret\n"
  end
  
  it "should compile ivar" do
    compile [:ivar, :@blah]
    @method.assembly.should == "push @blah\nret\n"
  end
  
  it "should compile iasgn" do
    compile [:iasgn, :@blah, [:lit, 99]]
    @method.assembly.should == "push 99\nset @blah\nret\n"
  end
  
  it "should compile ivar_as_index" do
    compile [:class, [:colon2, :B], nil, 
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
    # Check the __class_init__
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush nil\npop\npush_literal 0\n" \
      "push_self\nadd_method blah\nret\n"
    m.literals.first.assembly.should == 
       "check_argcount 0 0\npush_my_field 1\npop\npush 11\nstore_my_field 1\nret\n"
  end
  
  it "should compile a hash literal" do
    compile [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    @method.assembly.should == "push 4\npush 3\npush 2\npush 1\nmake_hash 4\nret\n"
  end
  
  it "should compile colon2" do
    compile [:colon2, [:const, :B], :A]
    @method.assembly.should == "push B\nfind A\nret\n"
  end
  
  it "should compile colon3" do
    compile [:colon3, :A]
    @method.assembly.should == "push_cpath_top\nfind A\nret\n"
  end
  
  it "should compile const" do
    compile [:const, :A]
    @method.assembly.should == "push A\nret\n"
  end
  
  it "should compile cdecl" do
    compile [:cdecl, :Blah, [:lit, 8], nil]
    @method.assembly.should == "push 8\nset Blah\nret\n"
  end
  
  it "should compile cdecl with path" do
    compile [:cdecl, nil, [:lit, 8], [:colon2, [:const, :A], :Blah]]
    @method.assembly.should == "push A\npush 8\nset +Blah\nret\n"
  end
  
  it "should compile class" do
    compile [:class, [:colon2, :Blah], nil, [:scope, [:true]]]
    @method.assembly.should == 
      "push nil\nopen_class Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\n" \
      "pop\npush_encloser\npush nil\nret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nret\n"
  end
  
  it "should compile class with sugar" do
    compile [:class, [:colon2, :Blah], [:colon2, [:const, :A], :B], [:scope, [:true]]]
    @method.assembly.should == "push A\nfind B\n" \
      "open_class Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\n" \
      "pop\npush_encloser\npush nil\nret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nret\n"
  end
  
  it "should compile class at cpath" do
    compile [:class, [:colon2, [:const, :A], :Blah], nil, [:scope, [:true]]]
    @method.assembly.should == 
      "push A\npush nil\nopen_class_under Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\n" \
      "pop\npush_encloser\npush nil\nret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nret\n"
  end
  
  it "should compile module" do
    compile [:module, [:colon2, :A], [:scope, [:true]]]
    @method.assembly.should == 
      "open_module A\ndup\npush_literal 0\nswap\n" \
      "attach __module_init__\npop\nsend __module_init__\n" \
      "pop\npush_encloser\npush nil\nret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nret\n"
  end
  
  it "should compile module at cpath" do
    compile [:module, [:colon2, [:const, :B], :A], [:scope, [:true]]]
    @method.assembly.should == 
      "push B\nopen_module_under A\ndup\npush_literal 0\nswap\n" \
      "attach __module_init__\npop\nsend __module_init__\n" \
      "pop\npush_encloser\npush nil\nret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nret\n"
  end
  
  it "should compile return" do
    compile [:return, [:lit, 8]]
    @method.assembly.should == "push 8\nret\nret\n"
  end
  
  it "should compile ensure" do
    compile [:ensure, [:lit, 10], [:lit, 11]]
    @method.assembly.should == "#exc_start exc1\npush 10\n#exceptions exc1\npush_exception\n#exc_start exc2\npush 11\npop\ngoto lbl3\n#exceptions exc2\ngit lbl4\npop\nlbl4:\npush_exception\nlbl3:\n#exc_end exc2\ndup\ngif lbl5\nraise_exc\nlbl5:\npop\n#exc_end exc1\nret\n"
  end
  
  it "should compile defn" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == 
      "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\nret\n"
  end
  
  it "should compile defn with splat" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], [:c, 4], nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == 
      "check_argcount 2 0\nset a:2\npop\nset b:3\npop\nmake_rest 2\nset c\npop\n" \
      "push true\nret\n"
  end
  
  it "should compile defn with default" do
    compile [:defn, :blah,
     [:scope,
      [:block,
       [:args, [:a], [:b], nil, [:block, [:lasgn, :b, 3, [:lit, 9]]]],
       [:false],
       [:true]],
      [:a, :b]]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 1 2\n" \
      "set a:2\npop\npassed_arg 1\ngit set1\npush 9\n" \
      "set1:\nset b:3\npop\npush false\npop\npush true\nret\n"
  end
  
  it "should compile defn with block arg" do
    compile [:defn, :blah,
     [:scope,
      [:block,
       [:args],
       [:block_arg, :b, 2],
       [:true]],
      [:b]]]
    @method.assembly.should == 
      "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 0 0\n" \
      "push_block\npush Proc\nsend from_environment 1\nset b:2" \
      "\npop\npush true\nret\n"
  end
  
  it "should compile defn with primitive" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:call, [:const, :Ruby], :primitive, [:array, [:lit, :at]]],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\nret\n"
    defn.primitive.should == :at
  end
  
  it "should compile defn with inline bytecodes" do
    compile [:defn, :blah, [:scope, [:block,
      [:args, [:a, :b], [], nil, nil],
      [:true],
      [:call, [:const, :Ruby], :asm, [:array, [:str, "push self"]]]],
        [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 2 2\nset a:2\npop\nset b:3\npop\npush true\npop\npush self\nret\n"
  end
  
  it "should compile defs" do
    compile [:defs, [:const, :Object], :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush Object\nattach_method blah\nret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    @method.assembly.should == "push_literal 0\npush Object\nattach_method blah\nret\n"
  end
  
  it "should compile masgn with no splat" do
    compile [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
      nil,
      [:to_ary, [:lit, 8]]]
    @method.assembly.should == "push 8\ncast_tuple\n" \
      "unshift_tuple\nset a:2\npop\n" \
      "unshift_tuple\nset b:3\npop\n" \
      "unshift_tuple\nset c:4\npop\npop\npush true\nret\n"
  end
  
  it "should compile masgn with splat" do
    compile [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
      [:lasgn, :c, 4],
      [:to_ary, [:lit, 8]]]
    @method.assembly.should == "push 8\ncast_tuple\n" \
      "unshift_tuple\nset a:2\npop\n" \
      "unshift_tuple\nset b:3\npop\n" \
      "cast_array\nset c:4\npop\npop\npush true\nret\n"
  end
  
  it "should compile masgn with array as the source" do
    compile [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     nil,
     [:array, [:lit, 99], [:lit, 8]]]
     
    @method.assembly.should == "push 8\npush 99\nset a:2\npop\nset b:3\npop\npush true\nret\n"
  end
  
  # TODO - Add correct asm expectation, remove should_raise
  it "should compile masgn with array as the source and too many lhs" do
    should_raise(RuntimeError) {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
       nil,
       [:array, [:lit, 99], [:lit, 8]]]
    }
  end
  
  # TODO - Add correct asm expectation, remove should_raise
  it "should compile masgn with array as the source and too many rhs" do
    should_raise(RuntimeError) {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
       nil,
       [:array, [:lit, 99], [:lit, 8], [:lit, 93]]]
    }
  end
  
  # TODO - Add correct asm expectation, remove should_raise
  it "should compile masgn with array as source splat is stupid" do
    should_raise(RuntimeError) {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
       [:lasgn, :c, 4],
       [:array, [:lit, 99], [:lit, 8]]]
    }
  end
  
  it "should compile call" do
    compile [:call, [:self], :b, [:array, [:lit, 1], [:lit, 2]]]
    @method.assembly.should == "push 2\npush 1\npush self\nsend b 2\nret\n"
  end
  
  it "should compile block pass" do
    compile [:block_pass, [:lit, 10], [:fcall, :d, [:array, [:lit, 9]]]]
    @method.assembly.should == "push 9\npush 10\npush self\n&send d 1\nlbl1:\nret\n"
  end
  
  it "should compile iter" do
    compile [:iter, [:fcall, :m], [:lasgn, :a, 0], [:block, [:dasgn_curr, :a], 
      [:true], [:fcall, :p, [:array, [:lit, 2]] ] ] ]
    @method.assembly.should == "push &lbl1\npush &lbl2\ncreate_block 1\ngoto lbl3\n" \
    "unshift_tuple\nset a:0:0\npop\nlbl4:\npush true\npop\npush 2\n" \
    "push self\nset_call_flags 1\n" \
    "send p 1\nlbl2: soft_return\nlbl3:\npush self\n" \
    "set_call_flags 1\n&send m 0\nlbl1:\nret\n"
  end
  
  it "should compile a string literal" do
    compile [:str, "blah"]
    @method.assembly.should == "push_literal 0\nstring_dup\nret\n"
  end
  
  it "should compile dstr" do
    compile [:dstr, "blah ", [:evstr, [:lit, 1]], [:str, " more "], [:evstr, [:lit, 2]]]
    @method.assembly.should == 
      "push 2\nsend to_s\npush_literal 0\nstring_dup\npush 1\n" \
      "send to_s\npush_literal 1\n" \
      "string_dup\nstring_append\nstring_append\n" \
      "string_append\nret\n"
  end
  
  it "should compile newline" do
    compile [:newline, 7, "blah.rb", [:true]]
    @method.assembly.should == "\#line 7\npush true\nret\n"
    @method.file.should == "blah.rb"
  end
  
  it "should compile correct lvars" do
    m1 = [:defn, :blah, [:scope, [:block, 
      [:args, [], [:fuck], nil, nil], [:true]], []]]
    m2 = [:defn, :to_s,
     [:scope,
      [:block,
       [:args, [], [:base], nil, [:block, [:lasgn, :base, 2, [:lit, 10]]]],
       [:fcall, :based_to_s, [:array, [:lvar, :base, 2]]]], [:base]]]

    compile [:class, [:colon2, :Fixnum], nil,
     [:scope, [:block,
      m1,
      m2,
      ], []]]
      
    cls = @method.literals.first
    cls.kind_of?(Bytecode::MethodDescription).should == true
    defn = cls.literals.last
    defn.assembly.should == 
      "check_argcount 0 1\npassed_arg 0\ngit set1\npush 10\nset1:\nset base:2\n" \
      "pop\npush base:2\npush self\nsend based_to_s 1\nret\n"
  end
  
  it "should compile a simple case" do
    compile [:case, [:lit, 1], [[:when, [:array, [:const, :String]], [:lit, 9]]]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngif lbl1\n" \
      "push 9\nlbl1:\nswap\npop\nret\n"
  end
  
  it "should compile a case with many when" do
    compile [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah]], [:lit, 3423]]
            ]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngit lbl3\n" \
      "dup\npush Fixnum\nsend === 1\ngif lbl1\n" \
      "lbl3:\npush 9\ngoto lbl2\nlbl1:\n" \
      "dup\npush Blah\nsend === 1\ngif lbl2\n" \
      "push 3423\nlbl2:\nswap\npop\nret\n"
  end
  
  it "should compile case with else" do
    compile [:case, [:lit, 1], [[:when, [:array, [:const, :String]], 
      [:lit, 9]]], [:lit, 10]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngif lbl1\n" \
      "push 9\ngoto lbl2\nlbl1:\npush 10\nlbl2:\nswap\npop\nret\n"
  end
  
  it "should compile case with else and multiple when" do
    compile [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah], [:const, :Go]], [:lit, 3423]]
            ], [:lit, 632]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngit lbl4\n" \
      "dup\npush Fixnum\nsend === 1\ngif lbl1\n" \
      "lbl4:\npush 9\ngoto lbl3\nlbl1:\n" \
      "dup\npush Blah\nsend === 1\ngit lbl5\n" \
      "dup\npush Go\nsend === 1\ngif lbl2\n" \
      "lbl5:\npush 3423\ngoto lbl3\nlbl2:\npush 632\n" \
      "lbl3:\nswap\npop\nret\n"
  end
  
  it "should compile dot2" do
    compile [:dot2, [:lit, 100], [:lit, 1]]
    @method.assembly.should == "push 1\npush 100\npush Range\nsend new 2\nret\n"
  end
  
  it "should compile dot3" do
    compile [:dot3, [:lit, 100], [:lit, 1]]
    @method.assembly.should == "push true\npush 1\npush 100\npush Range\nsend new 3\nret\n"
  end
end
