require File.dirname(__FILE__) + '/../spec_helper'

def block_spec_method(*args)
  yield(*args)
end

describe "A block with a 'rest' arg" do
  it "should collect all of the yielded arguments" do
    ret = nil
    block_spec_method(1,2,3) {|*args| ret = args}
    ret.should == [1,2,3]
  end
end

describe "A block with an anonymous 'rest' arg" do
  it "should ignore all of the yielded arguments" do
    ret = [1].each {|*| }
    ret.should == [1]
  end
end

describe "Block parameters" do
  it "should assign to local variable" do
    i = 0
    a = [1,2,3]
    a.each {|i| ;}
    i.should == 3
  end

  compliant :mri do
    require 'spec/language/strange_block_args_spec.rb'
  end

  it "should capture variables from outer scope" do
    a = [1,2,3]    
    sum = 0
    var = nil
    a.each {|var| sum += var}
    sum.should == 6
    var.should == 3
  end
end
