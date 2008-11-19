require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#==" do
  it "return true if the other is self" do
    a = [1, 2, 3]
    a.should == a
  end

  it "returns true if each element is == to the corresponding element in the other array" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    [1, 2, 3].should == [1.0, 2.0, 3.0]

    obj = mock('5')
    def obj.==(other) true end
    [obj].should == [5]
  end

  it "returns false if the other is shorter than self" do
    a = [1, 2, 3]
    b = [1, 2]
    a.should_not == b
  end
  
  it "returns false if the other is longer than self" do
    a = [1, 2, 3]
    b = [1, 2, 3, 4]
    a.should_not == b
  end
  
  it "returns false if any element is not == to the corresponding element in the other the array" do
    a = ["a", "b", "c"]
    b = ["a", "b", "not equal value"]
    a.should_not == b

    c = "c"
    def c.==(x); false end
    ["a", "b", c].should_not == a
  end
  
  it "returns false immediately when sizes of the arrays differ" do
    obj = mock('1')
    obj.should_not_receive(:==)
    
    [].should_not == [obj]
    [obj].should_not == []
  end

  it "properly handles recursive arrays" do
    empty = ArraySpecs.empty_recursive_array
    empty2 = []; empty2 << empty2
    (empty == empty.dup).should be_true
    (empty == empty2).should be_false

    array = ArraySpecs.recursive_array
    (array == array).should be_true
    (array == array.dup).should be_true
    (array == empty).should be_false
    (array == [1, 2]).should be_false
  end

  # Broken in MRI as well. See MRI bug #11585:
  # http://rubyforge.org/tracker/index.php?func=detail&aid=11585&group_id=426&atid=1698
  compliant_on :r19, :jruby, :ir  do
    it "calls to_ary on its argument" do
      obj = mock('to_ary')
      obj.should_receive(:to_ary).and_return([1, 2, 3])
    
      [1, 2, 3].should == obj
    end
  end
  
  it "does not call to_ary on array subclasses" do
    [5, 6, 7].should == ArraySpecs::ToAryArray[5, 6, 7]
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].should == [1, 2, 3]
    ArraySpecs::MyArray[1, 2, 3].should == ArraySpecs::MyArray[1, 2, 3]
    [1, 2, 3].should == ArraySpecs::MyArray[1, 2, 3]
  end

  it "can be assymetric (but should not)" do
    bad_array = ArraySpecs::MyArray[1, 2, 3]
    def bad_array.==(x) false end
    [1, 2, 3].should == bad_array
    bad_array.should_not == [1, 2, 3]
  end
end
