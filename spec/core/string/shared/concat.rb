shared :string_concat do |cmd|
  describe "String##{cmd}" do
    it "concatenates the given argument to self and returns self" do
      str = 'hello '
      str.send(cmd, 'world').equal?(str).should == true
      str.should == "hello world"
    end
    
    it "converts the given argument to a String using to_str" do
      obj = Object.new
      obj.should_receive(:to_str, :returning => "world!")
      a = 'hello '.send(cmd, obj)
      a.should == 'hello world!'
      
      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_str], :count => :any, :returning => true)
      obj.should_receive(:method_missing, :with => [:to_str], :returning => "world!")
      a = 'hello '.send(cmd, obj)
      a.should == 'hello world!'
    end
    
    it "raises a TypeError if the given argument can't be converted to a String" do
      should_raise(TypeError) do
        a = 'hello '.send(cmd, :world)
      end
  
      should_raise(TypeError) do
        a = 'hello '.send(cmd, Object.new)
      end
    end
  
    it "raises a TypeError when self is frozen" do
      a = "hello"
      a.freeze
  
      should_raise(TypeError) { a.send(cmd, "") }
      should_raise(TypeError) { a.send(cmd, "test") }
    end
    
    it "works when given a subclass instance" do
      a = "hello"
      a << MyString.new(" world")
      a.should == "hello world"
    end
    
    it "taints self if other is tainted" do
      x = "x"
      x.send(cmd, "".taint).tainted?.should == true
  
      x = "x"
      x.send(cmd, "y".taint).tainted?.should == true
    end
  end
  
  describe "String##{cmd} with Fixnum" do
    it "converts the given Fixnum to a char before concatenating" do
      b = 'hello '.send(cmd, 'world').send(cmd, 33)
      b.should == "hello world!"
      b.send(cmd, 0)
      b.should == "hello world!\x00"
    end
  
    it "raises a TypeError when the given Fixnum is not between 0 and 255" do
      should_raise(TypeError) { "hello world".send(cmd, 333) }
      should_raise(TypeError) { "".send(cmd, (256 * 3 + 64)) }
      should_raise(TypeError) { "".send(cmd, -200) }
    end
  
    it "doesn't call to_int on its argument" do
      x = Object.new
      x.should_not_receive(:to_int)
      
      should_raise(TypeError) { "".send(cmd, x) }
    end
  
    it "raises a TypeError when self is frozen" do
      a = "hello"
      a.freeze
  
      should_raise(TypeError) { a.send(cmd, 0) }
      should_raise(TypeError) { a.send(cmd, 33) }
    end
  end
end