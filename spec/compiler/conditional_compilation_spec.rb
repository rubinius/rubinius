require File.dirname(__FILE__) + "/spec_helper"

# This is just:
#   Rubinius.compile_if($CompilerSpecsConditional) { $stderr.puts "Conditional" }


describe Compiler, "conditional compilation with Rubinius.compile_if given an argument evaluating to false at compile time" do
  it "completely omits the call block and its contained code from the bytecode output as if the block did not exist" do
    full_sexp =  [ :newline, 1, "../some/file",
                   [:iter,
                     [:call, [:const, :Rubinius], :compile_if,
                       [:array, [:gvar, :$CompilerSpecsConditional]]
                     ], nil,
                     [:newline, 1, "../some/file",
                       [:call, [:gvar, :$stderr], :puts,
                         [:array, [:str, "Conditional"]]
                       ]
                     ]
                   ]
                 ]

    omitted_sexp = [:nil]

    $CompilerSpecsConditional = false

    c = Compiler.new Compiler::Generator
    c.activate :conditional_compilation

    a = c.into_script full_sexp
    b = c.into_script omitted_sexp

    a.class.should == Compiler::Node::Script
    b.class.should == Compiler::Node::Script
    a.body.class.should == Compiler::Node::Nil
    b.body.class.should == Compiler::Node::Nil
  end
end

describe Compiler, "conditional compilation with Rubinius.compile_if given an argument evaluating to true at compile time" do
  it "omits the Rubinius.compile_if block artifacts from the produced bytecode" do
    full_sexp =  [ :newline, 1, "../some/file",
                   [:iter,
                     [:call, [:const, :Rubinius], :compile_if,
                       [:array, [:gvar, :$CompilerSpecsConditional]]
                     ], nil,
                     [:newline, 1, "../some/file",
                       [:call, [:gvar, :$stderr], :puts,
                         [:array, [:str, "Conditional"]]
                       ]
                     ]
                   ]
                 ]

    included_sexp =  [:newline, 1, "../some/file",
                       [:call, [:gvar, :$stderr], :puts,
                         [:array, [:str, "Conditional"]]
                       ]
                     ]

    $CompilerSpecsConditional = true

    c = Compiler.new Compiler::Generator
    c.activate :conditional_compilation

    a = c.into_script full_sexp
    b = c.into_script included_sexp

    a.class.should == b.class
    a.body.class.should == b.body.class
    a.body.file.should == b.body.file
    a.body.child.method.should == b.body.child.method
    a.body.child.object.name.should == b.body.child.object.name
  end
end

