shared :stringio_eof do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new("eof")
    end

    it "is false for a new StringIO" do
      @io.send(cmd).should == false
    end

    it "is true once the IO has been read" do
      @io.read(3)
      @io.send(cmd).should == true
    end

    it "is true after gets returns nil" do
      @io.gets.should == 'eof'
      @io.gets.should == nil
      @io.send(cmd).should == true
    end

    it "is true after seeking to the end" do
      @io.seek(3)
      @io.send(cmd).should == true
    end

    it "is true after seeking beyond the end" do
      @io.seek(5)
      @io.send(cmd).should == true
    end

    it "is true after setting the position to the end" do
      @io.pos = 3
      @io.send(cmd).should == true
    end

    it "is true after setting the position beyond the end" do
      @io.pos = 5
      @io.send(cmd).should == true
    end
  end
end
