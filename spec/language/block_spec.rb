require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/block'

describe "A block with a 'rest' arg" do
  it "collects all of the arguments passed to yield" do
    ret = nil
    BlockSpecs::Yield.new.splat(1,2,3) {|*args| ret = args}
    ret.should == [1,2,3]
  end
end

describe "A block with an anonymous 'rest' arg" do
  it "ignores all of the arguments passed to yield" do
    ret = [1].each {|*| }
    ret.should == [1]
  end
end

describe "Block parameters" do
  it "assign to local variable" do
    i = 0
    a = [1,2,3]
    a.each {|i| ;}
    i.should == 3
  end

  compliant :mri do
    require 'spec/language/strange_block_args_subspec.rb'
  end

  it "captures variables from the outer scope" do
    a = [1,2,3]    
    sum = 0
    var = nil
    a.each {|var| sum += var}
    sum.should == 6
    var.should == 3
  end
end

describe "A block whose arguments are splatted" do
  it "captures the arguments passed to the block in an array" do
    a = []
    BlockSpecs::Yield.new.two_args { |*args| a << args }
    a.should == [[1, 2]]
  end

  it "captures the array passed to the block in an array" do
    a = []
    BlockSpecs::Yield.new.two_arg_array { |*args| a << args }
    a.should == [[[1, 2]]]
  end
end
