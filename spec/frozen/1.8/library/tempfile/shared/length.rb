shared :tempfile_length do |cmd|
  describe "Tempfile##{cmd}" do
    before(:each) do
      @tempfile = Tempfile.new("specs")
    end
  
    it "returns the size of self" do
      @tempfile.send(cmd).should eql(0)
      @tempfile.print("Test!")
      @tempfile.send(cmd).should eql(5)
    end
  
    it "returns 0 when self is closed" do
      @tempfile.print("Test!")
      @tempfile.close
      @tempfile.send(cmd).should eql(0)
    end
  end
end