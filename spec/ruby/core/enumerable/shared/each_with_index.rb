describe :each_with_index, :shared => true do
  before :each do
    @b = EnumerableSpecs::Numerous.new(2, 5, 3, 6, 1, 4)
  end

  it "passes each element and its index to block" do
    @a = []
    @b.send(@method) { |o, i| @a << [o, i] }
    @a.should == [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]]
  end

  it "provides each element to the block" do
    acc = []
    obj = EnumerableSpecs::EachDefiner.new()
    res = obj.send(@method) {|a,i| acc << [a,i]}
    acc.should == []
    obj.should == res
  end

  it "provides each element to the block and its index" do
    acc = []
    res = @b.send(@method) {|a,i| acc << [a,i]}
    [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]].should == acc
    res.should eql(@b)
  end

  it "binds splat arguments properly" do
    acc = []
    res = @b.send(@method) { |*b| c,d = b; acc << c; acc << d }
    [2, 0, 5, 1, 3, 2, 6, 3, 1, 4, 4, 5].should == acc
    res.should eql(@b)
  end

  it "returns an enumerator if no block" do
    e = @b.send(@method)
    e.should be_an_instance_of(enumerator_class)
    e.to_a.should == [[2, 0], [5, 1], [3, 2], [6, 3], [1, 4], [4, 5]]
  end
end
