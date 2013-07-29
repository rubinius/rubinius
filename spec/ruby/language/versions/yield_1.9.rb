describe "taking matching arguments with splats and post args" do
  it "raises a LocalJumpError when the method is not passed a block" do
    lambda { @y.rs(1, 2, [3, 4]) }.should raise_error(LocalJumpError)
  end

  it "passes the arguments to the block" do
    @y.rs([1, 2], 3, 4) { |(*a, b), c, d| [a, b, c, d] }.should == [[1], 2, 3, 4]
  end
end
