shared :stringio_read do |cmd|
  describe "StringIO##{cmd} when passed [length, buffer]" do
    before(:each) do
      @io = StringIO.new("example")
    end

    it "returns the passed buffer String" do
      # Note: Rubinius bug:
      # @io.send(cmd, 7, buffer = "").should equal(buffer)
      ret = @io.send(cmd, 7, buffer = "")
      ret.should equal(buffer)
    end

    it "reads length bytes and writes them to the buffer String" do
      @io.send(cmd, 7, buffer = "")
      buffer.should == "example"
    end

    it "tries to convert the passed buffer Object to a String using #to_str" do
      obj = mock("to_str")
      obj.should_receive(:to_str).and_return(buffer = "")

      @io.send(cmd, 7, obj)
      buffer.should == "example"
    end

    it "raises a TypeError when the passed buffer Object can't be converted to a String" do
      lambda { @io.send(cmd, 7, Object.new) }.should raise_error(TypeError)
    end

    it "checks whether the passed buffer Object responds to #to_str" do
      obj = mock('method_missing to_str')
      obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_str).and_return(buffer = "")
      @io.send(cmd, 7, obj)
      buffer.should == "example"
    end

    not_compliant_on :rubinius do
      it "raises an error when passed a frozen String as buffer" do
        lambda { @io.send(cmd, 7, "".freeze) }.should raise_error(TypeError)
      end
    end
  end

  describe "StringIO##{cmd} when passed [length]" do
    before(:each) do
      @io = StringIO.new("example")
    end

    it "reads length bytes from the current position and returns them" do
      @io.pos = 3
      @io.send(cmd, 4).should == "mple"
    end

    it "reads at most the whole content" do
      @io.send(cmd, 999).should == "example"
    end

    it "correctly updates the position" do
      @io.send(cmd, 3)
      @io.pos.should eql(3)

      @io.send(cmd, 999)
      @io.pos.should eql(7)
    end

    it "tries to convert the passed length to an Integer using #to_int" do
      obj = mock("to_int")
      obj.should_receive(:to_int).and_return(7)
      @io.send(cmd, obj).should == "example"
    end

    it "raises a TypeError when the passed length can't be converted to an Integer" do
      lambda { @io.send(cmd, Object.new) }.should raise_error(TypeError)
    end

    it "raises a TypeError when the passed length is negative" do
      lambda { @io.send(cmd, -2) }.should raise_error(ArgumentError)
    end

    it "checks whether the passed length Object responds to #to_int" do
      obj = mock('method_missing to_int')
      obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_int).and_return(7)
      @io.send(cmd, obj).should == "example"
    end
  end

  describe "StringIO##{cmd} when passed no arguments" do
    before(:each) do
      @io = StringIO.new("example")
    end

    it "reads the whole content starting from the current position" do
      @io.send(cmd).should == "example"

      @io.pos = 3
      @io.send(cmd).should == "mple"
    end

    it "correctly updates the current position" do
      @io.send(cmd)
      @io.pos.should eql(7)
    end
  end

  describe "StringIO##{cmd} when self is not readable" do
    it "raises an IOError" do
      io = StringIO.new("test", "w")
      lambda { io.send(cmd) }.should raise_error(IOError)

      io = StringIO.new("test")
      io.close_read
      lambda { io.send(cmd) }.should raise_error(IOError)
    end
  end
end