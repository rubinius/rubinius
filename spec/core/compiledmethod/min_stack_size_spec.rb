require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "CompiledMethod#min_stack_size" do
  before do
    @cm1 = CompiledMethodSpecs.instance_method(:simple_puts).compiled_method
    @cm2 = CompiledMethodSpecs.instance_method(:mthd_with_splat).compiled_method
    @cm3 = CompiledMethodSpecs.instance_method(:more_complex).compiled_method
  end

  it "calculates the minimum stack size required by a method" do
    @cm1.min_stack_size.should == [2,true]
    @cm2.min_stack_size.should == [3,true]
    @cm3.min_stack_size.should == [4,false]
  end
end