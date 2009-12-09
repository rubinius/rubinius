describe :collect, :shared => true do
  before :all do
    @m = Matrix[ [1, 2], [1, 2] ]
  end

  it "returns an instance of Matrix" do
    @m.send(@method){|n| n * 2 }.should be_kind_of(Matrix)
  end

  it "returns a Matrix where each element is the result of the block" do
    @m.send(@method) { |n| n * 2 }.should == Matrix[ [2, 4], [2, 4] ]
  end

  ruby_bug "#1531", "1.8.7" do
    it "returns an enumerator if no block is given" do
      @m.send(@method).should be_kind_of(enumerator_class)
    end
  end
end
