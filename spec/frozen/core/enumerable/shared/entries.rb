describe :enumerable_entries, :shared => true do
  it "returns an array containing the elements" do
    numerous = EnumerableSpecs::Numerous.new(1, nil, 'a', 2, false, true)
    numerous.send(@method).should == [1, nil, "a", 2, false, true]
  end
  
  ruby_version_is '1.8.7' do
    it "passes arguments to each" do
      count = EnumerableSpecs::EachCounter.new(1, 2, 3)
      count.to_a(:hello, "world").should == [1, 2, 3]
      count.arguments_passed.should == [:hello, "world"]
    end
  end
end
