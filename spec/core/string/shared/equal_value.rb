@string_equal_value = shared "String#==" do |cmd|
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
      b = MyString.new("hello")
      
      a.send(cmd, b).should == true
      b.send(cmd, a).should == true
    end  
  end
  
  describe "String##{cmd}" do
    it "returns false if obj does not respond to to_str" do
      'hello'.send(cmd, 5).should == false
      'hello'.send(cmd, :hello).should == false
      'hello'.send(cmd, Object.new).should == false
    end
    
    it "returns obj == self if obj responds to to_str" do
      obj = Object.new
      def obj.to_str() 'world!' end
      obj.should_receive(:==, :count => 2, :returning => true)
      'hello'.send(cmd, obj).should == true
      'world!'.send(cmd, obj).should == true 
      
      obj = Object.new
      other = "abc"
      obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
      obj.should_receive(:==, :returning => true)
      other.send(cmd, obj).should == true
    end
  end
end