describe :enumerable_entries, :shared => true do
  it "returns an array containing the elements" do
    numerous = EnumerableSpecs::Numerous.new(1, nil, 'a', 2, false, true)
    numerous.send(@method).should == [1, nil, "a", 2, false, true]
  end

  ruby_version_is "1.8.7" do
    it "passes through the values yielded by #each_with_index" do
      [:a, :b].each_with_index.send(@method).should == [[:a, 0], [:b, 1]]
    end

    it "passes arguments to each" do
      count = EnumerableSpecs::EachCounter.new(1, 2, 3)
      count.send(@method, :hello, "world").should == [1, 2, 3]
      count.arguments_passed.should == [:hello, "world"]
    end
  end

  ruby_version_is "" ... "1.9" do
    it "does not copy tainted status to the returning value if self is tainted" do
      EnumerableSpecs::Numerous.new.taint.send(@method).tainted?.should be_false
    end
  end

  ruby_version_is "1.9" do
    it "returns a tainted array if self is tainted" do
      EnumerableSpecs::Empty.new.taint.send(@method).tainted?.should be_true
    end

    it "returns an untrusted array if self is untrusted" do
      EnumerableSpecs::Empty.new.untrust.send(@method).untrusted?.should be_true
    end
  end
end
