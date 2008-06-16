shared :stringio_eof do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new("eof")
    end

    it "returns true when self's position is greater than or equal to self's size" do
      @io.pos = 3
      @io.send(cmd).should be_true
      
      @io.pos = 6
      @io.send(cmd).should be_true
    end

    it "returns false when self's position is less than self's size" do
      @io.pos = 0
      @io.send(cmd).should be_false

      @io.pos = 1
      @io.send(cmd).should be_false

      @io.pos = 2
      @io.send(cmd).should be_false
    end
  end
end
