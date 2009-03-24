require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('data')
require File.dirname(__FILE__) + '/ext/data'

describe "CApiAllocSpecs (a class with an alloc func defined)" do
  it "calls the alloc func" do
    @s = CApiAllocSpecs.new
    @s.wrapped_data.should == 42 # not defined in initialize
  end
end

describe "CApiWrappedStruct" do
  before :each do
    @s = CApiWrappedStructSpecs.new
  end
  
  it "Data_Wrap_Struct should wrap and Data_Get_Struct should return data" do
    a = @s.wrap_struct(1024)
    @s.get_struct(a).should == 1024
  end

  it "RDATA()->data should return the struct data" do
    a = @s.wrap_struct(1024)
    @s.get_struct_rdata(a).should == 1024
  end

  it "Changing RDATA()->data should change the wrapped struct" do
    a = @s.wrap_struct(1024)
    @s.change_struct(a, 100)
    @s.get_struct(a).should == 100
  end
end
