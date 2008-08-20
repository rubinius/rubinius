require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_wrapped_struct')
require File.dirname(__FILE__) + '/ext/subtend_wrapped_struct'

describe "SubtendWrappedStruct" do
  before :each do
    @s = SubtendWrappedStruct.new
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
