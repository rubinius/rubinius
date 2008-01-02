shared :string_length do |cmd|
  describe "String##{cmd}" do
    it "returns the length of self" do
      "".send(cmd).should == 0
      "\x00".send(cmd).should == 1
      "one".send(cmd).should == 3
      "two".send(cmd).should == 3
      "three".send(cmd).should == 5
      "four".send(cmd).should == 4
    end
  end
end