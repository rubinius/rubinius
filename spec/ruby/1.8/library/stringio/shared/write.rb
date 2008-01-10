shared :stringio_write do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new('12345')
    end

    it "writes at the current buffer position" do
      @io.read(2)
      @io.send(cmd, 'x').should == 1
      @io.string.should == '12x45'
      @io.send(cmd, 7).should == 1
      @io.string.should == '12x75'
    end

    it "pads with null bytes if the position is after the end" do
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
  end
end
