describe :array_equal, :shared => true do
  it "returns true if each element is == to the corresponding element in the other array" do
    [].send(@method, []).should == true
    ["a", "c", 7].send(@method, ["a", "c", 7]).should == true
    [1, 2, [3, 4]].send(@method, [1, 2, [3, 4]]).should == true

    obj = mock('5')
    def obj.==(other) true end
    [obj].send(@method, [5]).should == true
  end

  it "returns false if any element is not == to the corresponding element in the other the array" do
    [ "a", "c" ].send(@method, [ "a", "c", 7 ]).should == false
  end

  it "returns false immediately when sizes of the arrays differ" do
    obj = mock('1')
    obj.should_not_receive(:==)

    [].send(@method, obj).should == false
    [obj].send(@method, []).should == false
  end

  # Broken in MRI as well. See MRI bug #11585:
  # http://rubyforge.org/tracker/index.php?func=detail&aid=11585&group_id=426&atid=1698
  compliant_on(:r19) do
    it "calls to_ary on its argument" do
      obj = mock('to_ary')
      obj.should_receive(:to_ary).and_return([1, 2, 3])

      [1, 2, 3].send(@method, obj).should == true
    end
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6, 7] == ArraySpecs::ToAryArray[5, 6, 7]).should == true
  end

  it "ignores array class differences" do
    ArraySpecs::MyArray[1, 2, 3].send(@method, [1, 2, 3]).should == true
    ArraySpecs::MyArray[1, 2, 3].send(@method, ArraySpecs::MyArray[1, 2, 3]).should == true
    [1, 2, 3].send(@method, ArraySpecs::MyArray[1, 2, 3]).should == true
  end
end
