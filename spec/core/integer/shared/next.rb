@integer_next = shared "Integer#next" do |cmd|
  describe "Integer##{cmd}" do
    it "returns the Integer equal to self + 1" do
      0.send(cmd).should == 1 
      -1.send(cmd).should == 0
      0xffffffff.send(cmd).should == 4294967296 
      20.send(cmd).should == 21
    end
  end
end
