shared :stringio_getc do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new("example")
    end
    
    it "increases self's position by one" do
      @io.send(cmd)
      @io.pos.should eql(1)

      @io.send(cmd)
      @io.pos.should eql(2)

      @io.send(cmd)
      @io.pos.should eql(3)
    end
    
    it "returns the 8-bit byte at the current position" do
      @io.send(cmd).should == ?e
      @io.send(cmd).should == ?x
      @io.send(cmd).should == ?a
    end
    
    it "returns nil when called at the end of self" do
      @io.pos = 7
      @io.send(cmd).should be_nil
      @io.send(cmd).should be_nil
      @io.send(cmd).should be_nil
    end
    
    it "does not increase self's position when called at the end of file" do
      @io.pos = 7
      @io.send(cmd)
      @io.pos.should eql(7)
      
      @io.send(cmd)
      @io.pos.should eql(7)
    end
  end

  describe "StringIO##{cmd} when self is not readable" do
    it "raises an IOError" do
      io = StringIO.new("xyz", "w")
      lambda { io.send(cmd) }.should raise_error(IOError)

      io = StringIO.new("xyz")
      io.close_read
      lambda { io.send(cmd) }.should raise_error(IOError)
    end
  end
end