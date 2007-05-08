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
end
