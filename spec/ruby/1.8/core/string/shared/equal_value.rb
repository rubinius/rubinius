shared :string_equal_value do |cmd|
  describe "String##{cmd} with String" do
    it "returns true if self <=> string returns 0" do
      'hello'.send(cmd, 'hello').should == true
    end
    
    it "returns false if self <=> string does not return 0" do
      "more".send(cmd, "MORE").should == false
      "less".send(cmd, "greater").should == false
    end
    
    it "ignores subclass differences" do
      a = "hello"
      b = StringSpecs::MyString.new("hello")
      
      a.send(cmd, b).should == true
      b.send(cmd, a).should == true
    end
  end
end