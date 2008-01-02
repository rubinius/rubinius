require File.dirname(__FILE__) + '/../spec_helper'

require 'lib/compiler1/bytecode/assembler'
require 'lib/compiler1/bytecode/compiler'

describe Compiler1::Bytecode::Compiler do
  before do
    @compiler = Compiler1::Bytecode::Compiler.new
  end

  def compile(sexp)
    @method = @compiler.compile_as_method(sexp, :test)
  end

  it "compiles the sexpression to a method" do
    compile [:and, [:true], [:false]]
    @method.kind_of?(Bytecode::MethodDescription).should == true
    @method.name.should == :test
    @method.assembly.kind_of?(String).should == true
  end

  it "compiles true" do
    compile [:true]
    @method.assembly.should == "push true\nsret\n"
  end

  it "compiles false" do
    compile [:false]
    @method.assembly.should == "push false\nsret\n"
  end

  it "compiles nil" do
    compile [:nil]
    @method.assembly.should == "push nil\nsret\n"
  end

  it "compiles or" do
    compile [:or, [:true], [:false]]
    @method.assembly.should == 
      "push true\ndup\ngit or_lbl1\npop\npush false\nor_lbl1:\nsret\n"
  end

  it "compiles not" do
    compile [:not, [:true]]
    @method.assembly.should == 
      "push true\ngit not_lbl1\npush true\ngoto not_lbl2\nnot_lbl1:\npush false\nnot_lbl2:\nsret\n"
  end

  it "compiles a number literal" do
    compile [:lit, 9]
    @method.assembly.should == "push 9\nsret\n"
  end

  it "compiles a symbol literal" do
    compile [:lit, :blah]
    @method.assembly.should == "push_literal 0\nsret\n"
  end

  it "compiles a regexp literal" do
    compile [:lit, /hello/]
    @method.assembly.should == 
      "push 16\npush_literal 0\npush Regexp\nsend new 2\nsret\n"
  end

  it "compiles if" do
    compile [:if, [:true], [:lit, 9], [:lit, 10]]
    @method.assembly.should == 
      "push true\ngif if_lbl1\npush 9\ngoto if_lbl2\nif_lbl1:\npush 10\nif_lbl2:\nsret\n"
  end

  it "compiles if with no else" do
    compile [:if, [:true], [:lit, 9]]
    @method.assembly.should == 
      "push true\ngif if_lbl1\npush 9\ngoto if_lbl2\nif_lbl1:\npush nil\nif_lbl2:\nsret\n"
  end

  it "compiles if with no then" do
    compile [:if, [:true], nil, [:lit, 10]]
    @method.assembly.should == 
      "push true\ngit if_lbl1\npush 10\ngoto if_lbl2\nif_lbl1:\npush nil\nif_lbl2:\nsret\n"
  end

  it "compiles a block" do
    compile [:block, [:true], [:lit, 11]]
    @method.assembly.should == "push true\npop\npush 11\nsret\n"
  end

  it "compiles scope" do
    compile [:scope, [:true]]
    @method.assembly.should == "push true\nsret\n"
  end

  it "compiles while" do
    compile [:while, [:true], [:block, [:lit, 10], [:lit, 11]], true]
    @method.assembly.should ==
      "while_lbl1:\npush true\ngif while_lbl2\nredo_lbl4:\npush 10\npop\npush 11\npop\ngoto while_lbl1\nwhile_lbl2:\npush nil\nbreak_lbl3:\nsret\n"
  end
  
  it "compiles post while" do
    compile [:while, [:true], [:block, [:lit, 10], [:lit, 11]], false]
    @method.assembly.should ==
      "while_lbl1:\npush 10\npop\npush 11\npop\nnext_lbl4:\npush true\ngif while_lbl2\ngoto while_lbl1\nwhile_lbl2:\npush nil\nbreak_lbl3:\nsret\n"
  end
  
  it "compiles until" do
    compile [:until, [:true], [:lit, 10]]
    @method.assembly.should == 
      "while_lbl1:\npush 10\npop\nnext_lbl4:\npush true\ngit while_lbl2\ngoto while_lbl1\nwhile_lbl2:\npush nil\nbreak_lbl3:\nsret\n"
  end

  it "compiles lasgn" do
    compile [:lasgn, :x, 8, [:false]]
    @method.assembly.should == "push false\nset_local_fp 1 ; local x\nsret\n"
  end

  it "compiles lvar" do
    compile [:lvar, :x, 8]
    @method.assembly.should == "get_local_fp 1 ; local x\nsret\n"
  end

  it "compiles an array literal" do
    compile [:array, [:lit, 99]]
    @method.assembly.should == "push 99\nmake_array 1\nsret\n"
  end

  it "compiles zarray" do
    compile [:zarray]
    @method.assembly.should == "make_array 0\nsret\n"
  end

  it "compiles to_ary" do
    compile [:to_ary, [:lit, 8]]
    @method.assembly.should == "push 8\ncast_array\nsret\n"
  end

  it "compiles a simple rescue" do
    compile [:rescue, [:true], [:resbody, nil, [:lit, 2], nil]]
    @method.assembly.should == 
      "rescue_lbl4:\n\#exc_start exc1\npush true\ngoto rescue_lbl2\n\#exceptions exc1\n" \
      "push_exception\npush StandardError\nsend === 1\n" \
      "gif rescue_lbl3\npush 2\ngoto rescue_lbl2\nrescue_lbl3:\n" \
      "push_exception\nraise_exc\nrescue_lbl2:\nclear_exception\n\#exc_end exc1\nsret\n"
  end

  it "compiles rescue with two resbodies" do
    compile [:rescue, [:true], [:resbody, nil, [:lit, 2], [:resbody, nil, [:lit, 3]]]]
    @method.assembly.should == 
      "rescue_lbl4:\n\#exc_start exc1\npush true\ngoto rescue_lbl2\n\#exceptions exc1\n" \
      "push_exception\npush StandardError\nsend === 1\n" \
      "gif resbody_lbl5\npush 2\ngoto rescue_lbl2\n" \
      "resbody_lbl5:\npush_exception\npush StandardError\nsend === 1\n" \
      "gif rescue_lbl3\npush 3\ngoto rescue_lbl2\nrescue_lbl3:\n" \
      "push_exception\nraise_exc\nrescue_lbl2:\nclear_exception\n\#exc_end exc1\nsret\n"
  end

  it "compiles rescue with multiple classes" do
    compile [:rescue, [:true], [:resbody, 
      [:array, [:const, :Blah], [:const, :Bleh]], [:lit, 4]]]
    @method.assembly.should == 
      "rescue_lbl4:\n\#exc_start exc1\npush true\ngoto rescue_lbl2\n\#exceptions exc1\n" \
      "push_exception\npush Blah\nsend === 1\n" \
      "git resbody_lbl5\npush_exception\npush Bleh\nsend === 1\n" \
      "gif rescue_lbl3\n" \
      "resbody_lbl5:\npush 4\ngoto rescue_lbl2\n" \
      "rescue_lbl3:\npush_exception\nraise_exc\n" \
      "rescue_lbl2:\nclear_exception\n\#exc_end exc1\nsret\n"
  end

  it "compiles argscat" do
    compile [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]
    @method.assembly.should == "push 99\ncast_array_for_args 2\npush_array\npush 2\npush 1\nsret\n"
  end

  it "compiles call with argscat" do
    compile [:call, [:self], :b, [:argscat, [:array, [:lit, 1], [:lit, 2]], [:lit, 99]]]
    @method.assembly.should == 
      "push 99\ncast_array_for_args 2\npush_array\npush 2\npush 1\nget_args\npush nil\n" \
      "swap\npush self\nswap\nset_args\nsend b +\nsret\n"
  end

  it "compiles call with argscat and newline" do
    compile [:call, [:const, :Hash], :[], [:newline, 1, "(eval)", [:argscat, [:array, [:lit, 1], [:lit, 2]]]]]
    @method.assembly.should == "cast_array_for_args 2\npush_array\npush 2\npush 1\nget_args\npush nil\nswap\npush Hash\nswap\nset_args\nsend [] +\nsret\n"
  end

  it "compiles yield" do
    compile [:yield, [:array, [:lit, 1], [:lit, 2]], false]
    @method.assembly.should == 
      "push 2\npush 1\npush_block\nsend call 2\nsret\n"
  end

  it "compiles yield with a splat" do
    compile [:yield, [:splat, [:array, [:lit, 5], [:lit, 6]]], true]
    @method.assembly.should == "push 5\npush 6\nmake_array 2\ncast_array_for_args 0\npush_array\nget_args\npush_block\nsend call +\nsret\n"
  end

  it "compiles ivar" do
    compile [:ivar, :@blah]
    @method.assembly.should == "push @blah\nsret\n"
  end

  it "compiles iasgn" do
    compile [:iasgn, :@blah, [:lit, 99]]
    @method.assembly.should == "push 99\nset @blah\nsret\n"
  end

  it "compiles ivar_as_index" do
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
      "push_self\nadd_method blah\nsret\n"
    m.literals.first.assembly.should == 
      "check_argcount 0 0\n\#line \npush_my_field 1\npop\npush 11\nstore_my_field 1\nsret\n"
  end

  it "compiles a hash literal" do
    compile [:hash, [:lit, 1], [:lit, 2], [:lit, 3], [:lit, 4]]
    @method.assembly.should == "push 4\npush 3\npush 2\npush 1\npush Hash\nsend [] 4\nsret\n"
  end

  it "compiles colon2" do
    compile [:colon2, [:const, :B], :A]
    @method.assembly.should == "push B\nfind A\nsret\n"
  end

  it "compiles colon3" do
    compile [:colon3, :A]
    @method.assembly.should == "push_cpath_top\nfind A\nsret\n"
  end

  it "compiles const" do
    compile [:const, :A]
    @method.assembly.should == "push A\nsret\n"
  end

  it "compiles cdecl" do
    compile [:cdecl, :Blah, [:lit, 8], nil]
    @method.assembly.should == "push 8\nset Blah\nsret\n"
  end

  it "compiles cdecl with path" do
    compile [:cdecl, nil, [:lit, 8], [:colon2, [:const, :A], :Blah]]
    @method.assembly.should == "push A\npush 8\nset +Blah\nsret\n"
  end

  it "compiles class" do
    compile [:class, [:colon2, :Blah], nil, [:scope, [:true]]]
    @method.assembly.should == 
      "push nil\nopen_class Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\n" \
      "push_encloser\nsret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nsret\n"
  end

  it "compiles class with sugar" do
    compile [:class, [:colon2, :Blah], [:colon2, [:const, :A], :B], [:scope, [:true]]]
    @method.assembly.should == 
      "push A\nfind B\n" \
      "open_class Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\n" \
      "push_encloser\nsret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nsret\n"
  end

  it "compiles class at cpath" do
    compile [:class, [:colon2, [:const, :A], :Blah], nil, [:scope, [:true]]]
    @method.assembly.should == 
      "push A\npush nil\nopen_class_under Blah\ndup\npush_literal 0\nswap\n" \
      "attach __class_init__\npop\nsend __class_init__\npush_encloser\nsret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nsret\n"
  end

  it "compiles module" do
    compile [:module, [:colon2, :A], [:scope, [:true]]]
    @method.assembly.should == 
      "open_module A\ndup\npush_literal 0\nswap\n" \
      "attach __module_init__\npop\nsend __module_init__\n" \
      "pop\npush self\npush_encloser\nsret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nsret\n"
  end

  it "compiles module at cpath" do
    compile [:module, [:colon2, [:const, :B], :A], [:scope, [:true]]]
    @method.assembly.should == 
      "push B\nopen_module_under A\ndup\npush_literal 0\nswap\n" \
      "attach __module_init__\npop\nsend __module_init__\n" \
      "pop\npush self\npush_encloser\nsret\n"
    m = @method.literals.first
    m.kind_of?(Bytecode::MethodDescription).should == true
    m.assembly.should == "push self\nset_encloser\npush true\nsret\n"
  end

  it "compiles return" do
    compile [:return, [:lit, 8]]
    @method.assembly.should == "push 8\nsret\nsret\n"
  end

  it "compiles ensure" do
    compile [:ensure, [:lit, 10], [:lit, 11]]
    @method.assembly.should == 
      "\#exc_start exc1\npush 10\n\#exceptions exc1\npush_exception\n" \
      "\#exc_start exc2\npush 11\npop\ngoto ensure_lbl3\n\#exceptions exc2\n" \
      "git ensure_lbl4\npop\nensure_lbl4:\npush_exception\n" \
      "ensure_lbl3:\n\#exc_end exc2\ndup\ngif ensure_lbl5\nraise_exc\n" \
      "ensure_lbl5:\npop\n\#exc_end exc1\nsret\n"
  end

  it "compiles defn" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 2 2\n\#line \npush true\nsret\n"
  end

  it "compiles defn with splat" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], [:c, 4], nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == 
      "check_argcount 2 0\nallocate_stack 1\n\#line \nmake_rest_fp 2\nset_local_fp 1\npop\npush true\nsret\n"
  end

  it "compiles defn with default" do
    compile [:defn, :blah,
     [:scope,
      [:block,
       [:args, [:a], [:b], nil, [:block, [:lasgn, :b, 3, [:lit, 9]]]],
       [:false],
       [:true]],
      [:a, :b]]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == 
      "check_argcount 1 2\n\#line \npassed_arg 1\ngit set_lbl1\npush 9\n" \
      "set b:0\npop\ngoto set_lbl2\nset_lbl1: set_local_from_fp 0 1\n" \
      "set_lbl2:\npush false\npop\npush true\nsret\n"
  end

  it "compiles defn with block arg" do
    compile [:defn, :blah,
     [:scope,
      [:block,
       [:args],
       [:block_arg, :b, 2],
       [:true]],
      [:b]]]
    @method.assembly.should == 
      "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == 
      "check_argcount 0 0\n" \
      "allocate_stack 1\n\#line \npush_block\npush Proc\nsend from_environment 1\nset_local_fp 1\n" \
      "pop\npush true\nsret\n"
  end

  it "compiles defn with primitive" do
    compile [:defn, :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:call, [:const, :Ruby], :primitive, [:array, [:lit, :at]]],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 2 2\n\#line \npush true\nsret\n"
    defn.primitive.should == :at
  end

  it "compiles defn with inline bytecodes" do
    compile [:defn, :blah, [:scope, [:block,
      [:args, [:a, :b], [], nil, nil],
      [:true],
      [:call, [:const, :Ruby], :asm, [:array, [:str, "push self"]]]],
        [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush_self\nadd_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    defn.assembly.should == "check_argcount 2 2\n\#line \npush true\npop\npush self\nsret\n"
  end

  it "compiles defs" do
    compile [:defs, [:const, :Object], :blah, [:scope, [:block,
        [:args, [:a, :b], [], nil, nil],
        [:true]],
      [:a, :b]
    ]]
    @method.assembly.should == "push_literal 0\npush Object\nattach_method blah\nsret\n"
    defn = @method.literals.first
    defn.kind_of?(Bytecode::MethodDescription).should == true
    @method.assembly.should == "push_literal 0\npush Object\nattach_method blah\nsret\n"
  end

  it "compiles masgn with no splat" do
    compile [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
      nil,
      [:to_ary, [:lit, 8]]]
    @method.assembly.should == 
      "push 8\ncast_tuple\ndup\n" \
      "unshift_tuple\nset_local_fp 3 ; local a\npop\n" \
      "unshift_tuple\nset_local_fp 2 ; local b\npop\n" \
      "unshift_tuple\nset_local_fp 1 ; local c\npop\npop\ncast_array\nsret\n"
  end

  it "compiles masgn with splat" do
    compile [:masgn,
      [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
      [:lasgn, :c, 4],
      [:to_ary, [:lit, 8]]]
    @method.assembly.should == 
      "push 8\ncast_tuple\ndup\n" \
      "unshift_tuple\nset_local_fp 3 ; local a\npop\n" \
      "unshift_tuple\nset_local_fp 2 ; local b\npop\n" \
      "cast_array\nset_local_fp 1 ; local c\npop\ncast_array\nsret\n"
  end

  it "compiles masgn with array as the source" do
    compile [:masgn,
     [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
     nil,
     [:array, [:lit, 99], [:lit, 8]]]
   
    @method.assembly.should == 
      "push 8\npush 99\nset_local_fp 2 ; local a\npop\nset_local_fp 1 ; local b\npop\npush true\nsret\n"
  end

  # TODO - Add correct asm expectation, remove raise_error
  it "compiles masgn with array as the source and too many lhs" do
    lambda {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3], [:lasgn, :c, 4]],
       nil,
       [:array, [:lit, 99], [:lit, 8]]]
    }.should raise_error(RuntimeError)
  end

  # TODO - Add correct asm expectation, remove raise_error
  it "compiles masgn with array as the source and too many rhs" do
    lambda {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
       nil,
       [:array, [:lit, 99], [:lit, 8], [:lit, 93]]]
    }.should raise_error(RuntimeError)
  end

  # TODO - Add correct asm expectation, remove raise_error
  it "compiles masgn with array as source splat is stupid" do
    lambda {
      compile [:masgn,
       [:array, [:lasgn, :a, 2], [:lasgn, :b, 3]],
       [:lasgn, :c, 4],
       [:array, [:lit, 99], [:lit, 8]]]
    }.should raise_error(RuntimeError)
  end

  it "compiles call" do
    compile [:call, [:self], :b, [:array, [:lit, 1], [:lit, 2]]]
    @method.assembly.should == "push 2\npush 1\npush self\nsend b 2\nsret\n"
  end

  it "compiles block pass" do
    compile [:block_pass, [:lit, 10], [:fcall, :d, [:array, [:lit, 9]]]]
    @method.assembly.should == "push 9\npush 10\npush self\nset_call_flags 1\n&send d 1\nps_lbl1:\nsret\n"
  end

  it "compiles iter" do
    compile [:iter, [:fcall, :m], [:lasgn, :a, 0], [:block, [:dasgn_curr, :a], 
      [:true], [:fcall, :p, [:array, [:lit, 2]] ] ] ]
    @method.assembly.should == 
      "push &ps_lbl1\npush &iter_lbl2\ncreate_block 1\ngoto iter_lbl3\n" \
      "unshift_tuple\nset a:0:0\npop\npop\nredo_lbl4:\npush true\npop\npush 2\n" \
      "push self\nset_call_flags 1\nsend p 1\niter_lbl2: soft_return\niter_lbl3:\n" \
      "push self\nset_call_flags 1\n&send m 0\nps_lbl1:\nsret\n"
  end

  it "compiles a string literal" do
    compile [:str, "blah"]
    @method.assembly.should == "push_literal 0\nstring_dup\nsret\n"
  end

  it "compiles dstr" do
    compile [:dstr, "blah ", [:evstr, [:lit, 1]], [:str, " more "], [:evstr, [:lit, 2]]]
    @method.assembly.should == 
      "push 2\nsend to_s\npush_literal 0\nstring_dup\npush 1\n" \
      "send to_s\npush_literal 1\n" \
      "string_dup\nstring_append\nstring_append\n" \
      "string_append\nsret\n"
  end

  it "compiles newline" do
    compile [:newline, 7, "blah.rb", [:true]]
    @method.assembly.should == "\#line 7\npush true\nsret\n"
    @method.file.should == "blah.rb"
  end

  it "compiles correct lvars" do
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
      "check_argcount 0 1\n\#line \npassed_arg 0\ngit set_lbl1\npush 10\nset base:0\npop\n" \
      "goto set_lbl2\nset_lbl1: set_local_from_fp 0 0\nset_lbl2:\npush base:0\npush self\n" \
      "set_call_flags 1\nsend based_to_s 1\nsret\n"
  end

  it "compiles a simple case" do
    compile [:case, [:lit, 1], [[:when, [:array, [:const, :String]], [:lit, 9]]]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngif case_lbl1\n" \
      "push 9\ngoto case_lbl2\ncase_lbl1:\npush nil\ncase_lbl2:\nswap\npop\nsret\n"
  end

  it "compiles a case with many when" do
    compile [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah]], [:lit, 3423]]
            ]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngit when_lbl4\n" \
      "dup\npush Fixnum\nsend === 1\ngif case_lbl1\n" \
      "when_lbl4:\npush 9\ngoto case_lbl3\ncase_lbl1:\ndup\npush Blah\nsend === 1\n" \
      "gif case_lbl2\npush 3423\ngoto case_lbl3\ncase_lbl2:\npush nil\ncase_lbl3:\nswap\npop\nsret\n"
  end

  it "compiles case with else" do
    compile [:case, [:lit, 1], [[:when, [:array, [:const, :String]], 
      [:lit, 9]]], [:lit, 10]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngif case_lbl1\n" \
      "push 9\ngoto case_lbl2\ncase_lbl1:\npush 10\ncase_lbl2:\nswap\npop\nsret\n"
  end

  it "compiles case with else and multiple when" do
    compile [:case, [:lit, 1], [
            [:when, [:array, [:const, :String], [:const, :Fixnum]], [:lit, 9]],
            [:when, [:array, [:const, :Blah], [:const, :Go]], [:lit, 3423]]
            ], [:lit, 632]]
    @method.assembly.should == 
      "push 1\ndup\npush String\nsend === 1\ngit when_lbl4\n" \
      "dup\npush Fixnum\nsend === 1\ngif case_lbl1\n" \
      "when_lbl4:\npush 9\ngoto case_lbl3\n" \
      "case_lbl1:\ndup\npush Blah\nsend === 1\n" \
      "git when_lbl5\ndup\npush Go\nsend === 1\ngif case_lbl2\n" \
      "when_lbl5:\npush 3423\ngoto case_lbl3\ncase_lbl2:\npush 632\n" \
      "case_lbl3:\nswap\npop\nsret\n"
  end

  it "compiles dot2" do
    compile [:dot2, [:lit, 100], [:lit, 1]]
    @method.assembly.should == "push 1\npush 100\npush Range\nsend new 2\nsret\n"
  end

  it "compiles dot3" do
    compile [:dot3, [:lit, 100], [:lit, 1]]
    @method.assembly.should == "push true\npush 1\npush 100\npush Range\nsend new 3\nsret\n"
  end
end
