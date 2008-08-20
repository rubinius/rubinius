require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_struct_define')
require File.dirname(__FILE__) + '/ext/subtend_struct_define'

describe "SubtendStructDefine" do
  before :each do
    @s = SubtendStructDefine.new
  end

  it "rb_struct_define defines a structure" do
    @s.rb_struct_define("MyStruct", "attr1", "attr2", "attr3")
    instance = Struct::MyStruct.new
    instance.members.sort.should == ["attr1", "attr2", "attr3"].sort
    (instance.attr1 = 1).should == 1
    (instance.attr2 = 2).should == 2
    (instance.attr3 = 3).should == 3
    # Verify that attributes are on an instance basis
    Struct::MyStruct.new.attr1.should == nil
  end

  it "rb_struct_define allows for anonymous structures" do
    klass = @s.rb_struct_define(nil, "attr1", "attr2", "attr3")
    instance = klass.new()
    instance.members.sort.should == ["attr1", "attr2", "attr3"].sort
    (instance.attr1 = 1).should == 1
    (instance.attr2 = 2).should == 2
    (instance.attr3 = 3).should == 3
  end
end
