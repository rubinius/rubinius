require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#delete_at" do
  it "removes the element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2)
    a.should == [1, 2, 4]
    a.delete_at(-1)
    a.should == [1, 2]
  end

  it "returns the removed element at the specified index" do
    a = [1, 2, 3, 4]
    a.delete_at(2).should == 3
    a.delete_at(-1).should == 4
  end
  
  it "returns nil if the index is out of range" do
    a = [1, 2]
    a.delete_at(3).should == nil
  end

  it "calls to_int on its argument" do
    obj = Object.new
    def obj.to_int() -1 end
    [1, 2].delete_at(obj).should == 2
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
    [1, 2].delete_at(obj).should == 2
  end
  
  it "accepts negative indices" do
    a = [1, 2]
    a.delete_at(-2).should == 1
  end

  compliant :mri do
    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { [1,2,3].freeze.delete_at(0) }
    end
  end
end
