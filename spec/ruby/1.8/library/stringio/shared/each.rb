shared :stringio_each do |cmd|
  describe "StringIO##{cmd}" do
    before(:each) do
      @io = StringIO.new("a b c d e") 
    end

    it "yields each line by default" do
      seen = []
      @io.send(cmd) {|s| seen << s}.should == @io
      seen.should == ["a b c d e"]
    end

    it "supports a separator argument" do
      seen = []
      @io.send(cmd, ' ') {|s| seen << s}.should == @io
      seen.should == ["a ", "b ", "c ", "d ", "e"]
    end
  end
end
