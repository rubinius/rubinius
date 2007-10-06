require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#values_at" do
  it "returns an array of elements at the indexes when passed indexes" do
    [1, 2, 3, 4, 5].values_at().should == []
    [1, 2, 3, 4, 5].values_at(1, 0, 5, -1, -8, 10).should == [2, 1, nil, 5, nil, nil]
  end

  it "calls to_int on its indices" do
    obj = Object.new
    def obj.to_int() 1 end
    [1, 2].values_at(obj, obj, obj).should == [2, 2, 2]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    [1, 2].values_at(obj).should == [2]
  end
  
  it "returns an array of elements in the ranges when passes ranges" do
    [1, 2, 3, 4, 5].values_at(0..2, 1...3, 4..6).should == [1, 2, 3, 2, 3, 5, nil]
    [1, 2, 3, 4, 5].values_at(6..4).should == []
  end

  it "calls to_int on arguments of ranges when passes ranges" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    ary = [1, 2, 3, 4, 5]
    ary.values_at(from .. to, from ... to, to .. from).should == [2, 3, 4, 2, 3]

    from = Object.new
    to = Object.new
    
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end
      
    from.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    to.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    to.should_receive(:method_missing, :with => [:to_int], :returning => -2)
    ary.values_at(from .. to).should == [2, 3, 4]
  end
  
  it "does not return subclass instance on Array subclasses" do
    MyArray[1, 2, 3].values_at(0, 1..2, 1).class.should == Array
  end  
end
