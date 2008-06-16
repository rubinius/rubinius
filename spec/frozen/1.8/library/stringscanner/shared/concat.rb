shared :strscan_concat do |cmd|
  describe "StringScanner##{cmd}" do
    it "concatenates the given argument to self and returns self" do
      s = StringScanner.new("hello ")
      s.send(cmd, 'world').should == s
      s.string.should == "hello world"
      s.eos?.should be_false
    end
  
    it "raises a TypeError if the given argument can't be converted to a String" do
      lambda { a = StringScanner.new('hello').send(cmd, :world)    }.should raise_error(TypeError)
      lambda { a = StringScanner.new('hello').send(cmd, mock('x')) }.should raise_error(TypeError)
    end
  end
  
  describe "String##{cmd} with Fixnum" do
    it "raises a TypeError when a Fixnum is passed as argument" do
      a = StringScanner.new("hello world")
      lambda { a.send(cmd, 333) }.should raise_error(TypeError)
      b = StringScanner.new("")
      lambda { b.send(cmd, (256 * 3 + 64)) }.should raise_error(TypeError)
      lambda { b.send(cmd, -200)           }.should raise_error(TypeError)
    end
  
    it "doesn't call to_int on its argument" do
      x = mock('x')
      x.should_not_receive(:to_int)
      
      lambda { "".send(cmd, x) }.should raise_error(TypeError)
    end
  end
end