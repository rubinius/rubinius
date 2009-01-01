describe :collect, :shared => true do
  before :all do
    @data = [ [1, 2], [1, 2] ]
  end

  it "returns an instance of Matrix" do
    Matrix[ *@data ].send(@method){|n| n * 2 }.should be_kind_of(Matrix)
  end

  it "returns a Matrix where each element is the result of the block" do
    Matrix[ *@data ].send(@method) { |n| n * 2 }.should == Matrix[ [2, 4], [2, 4] ]
  end
end
