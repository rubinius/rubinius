shared :string_to_s do |cmd|
  describe "String##{cmd}" do
    it "returns self when self.class == String" do
      a = "a string"
      a.should equal(a.send(cmd))
    end
  
    it "returns a new instance of String when called on a subclass" do
      a = StringSpecs::MyString.new("a string")
      s = a.send(cmd)
      s.should == "a string"
      s.class.should == String
    end

    it "taints the result when self is tainted" do
      "x".taint.send(cmd).tainted?.should == true
      StringSpecs::MyString.new("x").taint.send(cmd).tainted?.should == true
    end
  end
end
