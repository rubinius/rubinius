shared :tempfile_length do |cmd|
  describe "Tempfile##{cmd}" do
    before(:each) do
      @tempfile = Tempfile.new("specs")
    end
  
    it "returns the size of self" do
      @tempfile.send(cmd).should eql(0)
      @tempfile.puts("Test!")
      @tempfile.send(cmd).should eql(6)
    end
  
    it "returns 0 when self is closed" do
      @tempfile.puts("Test!")
      @tempfile.close
      @tempfile.send(cmd).should eql(0)
    end
  end
end