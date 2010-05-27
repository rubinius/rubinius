require File.expand_path('../spec_helper', __FILE__)

load_extension("struct")

describe "CApiStruct" do
  before :each do
    @s = CApiStructSpecs.new
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
