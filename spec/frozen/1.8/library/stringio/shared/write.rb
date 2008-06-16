shared :stringio_write do |cmd|
  describe "StringIO##{cmd} when passed [Object]" do
    before(:each) do
      @io = StringIO.new('12345')
    end
    
    it "tries to convert the passed Object to a String using #to_s" do
      obj = mock("to_s")
      obj.should_receive(:to_s).and_return("to_s")
      @io.send(cmd, obj)
      @io.string.should == "to_s5"
    end
  end
  
  describe "StringIO##{cmd} when passed [String]" do
    before(:each) do
      @io = StringIO.new('12345')
    end

    # TODO: RDoc says that #write appends at the current position.
    it "writes the passed String at the current buffer position" do
      @io.pos = 2
      @io.send(cmd, 'x').should == 1
      @io.string.should == '12x45'
      @io.send(cmd, 7).should == 1
      @io.string.should == '12x75'
    end

    it "pads self with \\000 when the current position is after the end" do
      @io.pos = 8
      @io.send(cmd, 'x')
      @io.string.should == "12345\000\000\000x"
      @io.send(cmd, 9)
      @io.string.should == "12345\000\000\000x9"
    end

    it "returns the number of bytes written" do
      @io.send(cmd, '').should == 0
      @io.send(cmd, nil).should == 0
      str = "1" * 100
      @io.send(cmd, str).should == 100
    end

    it "updates self's position" do
      @io.send(cmd, 'test')
      @io.pos.should eql(4)
    end

    it "taints self's String when the passed argument is tainted" do
      @io.send(cmd, "test".taint)
      @io.string.tainted?.should be_true
    end

    it "does not taint self when the passed argument is tainted" do
      @io.send(cmd, "test".taint)
      @io.tainted?.should be_false
    end
  end

  describe "StringIO##{cmd} when self is not writable" do
    it "raises an IOError" do
      io = StringIO.new("test", "r")
      lambda { io.send(cmd, "test") }.should raise_error(IOError)

      io = StringIO.new("test")
      io.close_write
      lambda { io.send(cmd, "test") }.should raise_error(IOError)
    end
  end

  describe "StringIO##{cmd} when in append mode" do
    before(:each) do
      @io = StringIO.new("example", "a")
    end

    it "appends the passed argument to the end of self" do
      @io.send(cmd, ", just testing")
      @io.string.should == "example, just testing"
      
      @io.send(cmd, " and more testing")
      @io.string.should == "example, just testing and more testing"
    end

    it "correctly updates self's position" do
      @io.send(cmd, ", testing")
      @io.pos.should eql(16)
    end
  end
end
