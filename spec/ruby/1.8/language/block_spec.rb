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

not_compliant_on :rubinius do
  # This functionality is being removed from MRI and has never
  # been used in 1.8, therefore rubinius doesn't support
  # it already.
  describe "Block parameters (to be removed from MRI)" do
    it "assigns to a global variable" do
      $global_for_block_assignment = 0
      a = [1,2,3]
      a.each {|$global_for_block_assignment| ;}
      $global_for_block_assignment.should == 3
    end

    it "calls method=" do
      class T
        def n; return @n; end
        def n=(val); @n = val + 1; end
        def initialize; @n = 0; end
      end
      t = T.new
      t.n.should == 0
      a = [1,2,3]    
      a.each {|t.n| }
      t.n.should == 4    
    end
  end
end
