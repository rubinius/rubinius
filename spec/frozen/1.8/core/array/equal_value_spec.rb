require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#==" do
  it "returns true if each element is == to the corresponding element in the other array" do
    [].should == []
    ["a", "c", 7].should == ["a", "c", 7]

    obj = mock('5')
    def obj.==(other) true end
    [obj].should == [5]
  end
  
  it "returns false if any element is not == to the corresponding element in the other the array" do
    ([ "a", "c" ] == [ "a", "c", 7 ]).should == false
  end
  
  it "returns false immediately when sizes of the arrays differ" do
    obj = mock('1')
    obj.should_not_receive(:==)
    
    [].should_not == [obj]
    [obj].should_not == []
  end

  # Broken in MRI as well. See MRI bug #11585:
  # http://rubyforge.org/tracker/index.php?func=detail&aid=11585&group_id=426&atid=1698
  compliant_on :r19, :jruby do
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
end
