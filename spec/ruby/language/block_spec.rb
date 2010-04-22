require File.expand_path('../../spec_helper', __FILE__)
require File.expand_path('../fixtures/block', __FILE__)

describe "A block with mismatched arguments" do
  it "Should fill in unsupplied arguments with nil" do
    ret = nil
    BlockSpecs::Yield.new.two_args {|one, two, three| ret = [one, two, three]}
    ret.should == [1, 2, nil]
  end
  
  it "raises ArgumentError if argument is passed, but the block takes none" do
    lambda{
      lambda{ || p "block with no argument" }.call(:arg)
    }.should raise_error(ArgumentError)
  end
  
end

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

  it "yields the correct arguments in a nested block" do
    a = []
    BlockSpecs::Yield.new.yield_splat_inside_block {|a1, a2| a << [a1, a2]}
    a.should == [[1, 0], [2, 1]]
  end
end

language_version __FILE__, "block"
