describe :array_length, :shared => true do
  it "returns the number of elements" do
    [].send(@method).should == 0
    [1, 2, 3].send(@method).should == 3
  end
end
