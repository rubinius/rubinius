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
  
  it "should assign to a global variable" do
    $global_for_block_assignment = 0
    a = [1,2,3]
    a.each {|$global_for_block_assignment| ;}
    $global_for_block_assignment.should == 3
  end

  it "should call method=" do
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

  it "should capture variables from outer scope" do
    a = [1,2,3]    
    sum = 0
    var = nil
    a.each {|var| sum += var}
    sum.should == 6
    var.should == 3
  end
end
