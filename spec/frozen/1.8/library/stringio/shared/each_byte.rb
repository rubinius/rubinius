shared :stringio_each_byte do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new("xyz")
    end

    it "yields each character code in turn" do
      seen = []
      @io.send(cmd) { |b| seen << b }
      seen.should == [120, 121, 122]
    end

    it "updates the position before each yield" do
      seen = []
      @io.send(cmd) { |b| seen << @io.pos }
      seen.should == [1, 2, 3]
    end

    it "does not yield if the current position is out of bounds" do
      @io.pos = 1000
      @io.send(cmd) { |b| b }.should be_nil
    end

    ruby_version_is "" ... "1.8.7" do
      it "returns nil" do
        @io.send(cmd) {}.should be_nil
      end
      
      it "yields a LocalJumpError when passed no block" do
        lambda { @io.send(cmd) }.should raise_error(LocalJumpError)
      end
    end

    ruby_version_is "1.8.7" do
      it "returns self" do
        @io.send(cmd) {}.should equal(@io)
      end

      it "returns an Enumerator when passed no block" do
        enum = @io.send(cmd)
        enum.instance_of?(Enumerable::Enumerator).should be_true
        
        seen = []
        enum.each { |b| seen << b }
        seen.should == [120, 121, 122]
      end
    end
  end
  
  describe "StringIO##{cmd} when self is not readable" do
    it "raises an IOError" do
      io = StringIO.new("xyz", "w")
      lambda { io.send(cmd) { |b| b } }.should raise_error(IOError)

      io = StringIO.new("xyz")
      io.close_read
      lambda { io.send(cmd) { |b| b } }.should raise_error(IOError)
    end
  end
end