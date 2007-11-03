shared :string_to_s do |cmd|
  it "returns self when self.class == String" do
    a = "a string"
    a.equal?(a.send(cmd)).should == true
  end
  
  it "returns a new instance of String when called on a subclass" do
    a = MyString.new("a string")
    s = a.send(cmd)
    s.should == "a string"
    s.class.should == String
  end

  it "taints the result when self is tainted" do
    "x".taint.send(cmd).tainted?.should == true
    MyString.new("x").taint.send(cmd).tainted?.should == true
  end
end