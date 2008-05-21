shared :string_concat do |cmd|
  describe "String##{cmd}" do
    it "concatenates the given argument to self and returns self" do
      str = 'hello '
      str.send(cmd, 'world').should equal(str)
      str.should == "hello world"
    end
    
    it "converts the given argument to a String using to_str" do
      obj = mock('world!')
      obj.should_receive(:to_str).and_return("world!")
      a = 'hello '.send(cmd, obj)
      a.should == 'hello world!'
      
      obj = mock('world!')
      obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_str).and_return("world!")
      a = 'hello '.send(cmd, obj)
      a.should == 'hello world!'
    end
    
    it "raises a TypeError if the given argument can't be converted to a String" do
      lambda { a = 'hello '.send(cmd, :world)    }.should raise_error(TypeError)
      lambda { a = 'hello '.send(cmd, mock('x')) }.should raise_error(TypeError)
    end

    compliant_on :ruby, :jruby do
      it "raises a TypeError when self is frozen" do
        a = "hello"
        a.freeze
  
        lambda { a.send(cmd, "")     }.should raise_error(TypeError)
        lambda { a.send(cmd, "test") }.should raise_error(TypeError)
      end
    end
    
    it "works when given a subclass instance" do
      a = "hello"
      a << StringSpecs::MyString.new(" world")
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
      lambda { "hello world".send(cmd, 333) }.should raise_error(TypeError)
      lambda { "".send(cmd, (256 * 3 + 64)) }.should raise_error(TypeError)
      lambda { "".send(cmd, -200)           }.should raise_error(TypeError)
    end
  
    it "doesn't call to_int on its argument" do
      x = mock('x')
      x.should_not_receive(:to_int)
      
      lambda { "".send(cmd, x) }.should raise_error(TypeError)
    end
  
    compliant_on :ruby, :jruby do
      it "raises a TypeError when self is frozen" do
        a = "hello"
        a.freeze
  
        lambda { a.send(cmd, 0)  }.should raise_error(TypeError)
        lambda { a.send(cmd, 33) }.should raise_error(TypeError)
      end
    end
  end
end