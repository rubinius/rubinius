shared :array_length do |cmd|
  describe "Array##{cmd}" do
    it "returns the number of elements" do
      [].send(cmd).should == 0
      [1, 2, 3].send(cmd).should == 3
    end
  end
end
