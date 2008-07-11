shared :stringio_each do |cmd|
  describe "StringIO##{cmd} when passed [seperator]" do
    before(:each) do
      @io = StringIO.new("a b c d e\n1 2 3 4 5") 
    end

    it "uses the passed argument as the line seperator" do
      seen = []
      @io.send(cmd, " ") {|s| seen << s}
      seen.should == ["a ", "b ", "c ", "d ", "e\n1 ", "2 ", "3 ", "4 ", "5"]
    end
    
    it "does not change $_" do
      $_ = "test"
      @io.send(cmd, " ") { |s| s}
      $_.should == "test"
    end

    it "returns self" do
      @io.send(cmd) {|l| l }.should equal(@io)
    end

    it "tries to convert the passed seperator to a String using #to_str" do
      obj = mock("to_str")
      obj.stub!(:to_str).and_return(" ")
      
      seen = []
      @io.send(cmd, obj) { |l| seen << l }
      seen.should == ["a ", "b ", "c ", "d ", "e\n1 ", "2 ", "3 ", "4 ", "5"]
    end
    
    it "checks whether the passed seperator responds to #to_str" do
      obj = mock('method_missing to_str')
      obj.should_receive(:respond_to?).any_number_of_times.with(:to_str).and_return(true)
      obj.should_receive(:method_missing).any_number_of_times.with(:to_str).and_return(" ")

      seen = []
      @io.send(cmd, obj) { |l| seen << l }
      seen.should == ["a ", "b ", "c ", "d ", "e\n1 ", "2 ", "3 ", "4 ", "5"]
    end
    
    it "yields self's content starting from the current position when the passed seperator is nil" do
      seen = []
      io = StringIO.new("1 2 1 2 1 2")
      io.pos = 2
      io.send(cmd, nil) {|s| seen << s}
      seen.should == ["2 1 2 1 2"]
    end
    
    it "yields each paragraph when passed an empty String as seperator" do
      seen = []
      io = StringIO.new("para1\n\npara2\n\n\npara3")
      io.send(cmd, "") {|s| seen << s}
      seen.should == ["para1\n", "para2\n", "para3"]
    end
  end
  
  describe "StringIO##{cmd} when passed no arguments" do
    before(:each) do
      @io = StringIO.new("a b c d e\n1 2 3 4 5") 
    end

    it "yields each line to the passed block" do
      seen = []
      @io.send(cmd) {|s| seen << s }
      seen.should == ["a b c d e\n", "1 2 3 4 5"]
    end
    
    it "yields each line starting from the current position" do
      seen = []
      @io.pos = 4
      @io.send(cmd) {|s| seen << s }
      seen.should == ["c d e\n", "1 2 3 4 5"]
    end

    it "does not change $_" do
      $_ = "test"
      @io.send(cmd) { |s| s}
      $_.should == "test"
    end

    it "uses $/ as the default line seperator" do
      seen = []
      begin
        old_rs, $/ = $/, " "
        @io.send(cmd) {|s| seen << s }
        seen.should eql(["a ", "b ", "c ", "d ", "e\n1 ", "2 ", "3 ", "4 ", "5"])
      ensure
        $/ = old_rs
      end
    end

    it "returns self" do
      @io.send(cmd) {|l| l }.should equal(@io)
    end
    
    ruby_version_is "" ... "1.8.7" do
      it "yields a LocalJumpError when passed no block" do
        lambda { @io.send(cmd) }.should raise_error(LocalJumpError)
      end
    end

    ruby_version_is "1.8.7" do
      it "returns an Enumerator when passed no block" do
        enum = @io.send(cmd)
        enum.instance_of?(Enumerable::Enumerator).should be_true
        
        seen = []
        enum.each { |b| seen << b }
        seen.should == ["a b c d e\n", "1 2 3 4 5"]
      end
    end
  end

  describe "StringIO##{cmd} when self is not readable" do
    it "raises an IOError" do
      io = StringIO.new("a b c d e", "w")
      lambda { io.send(cmd) { |b| b } }.should raise_error(IOError)

      io = StringIO.new("a b c d e")
      io.close_read
      lambda { io.send(cmd) { |b| b } }.should raise_error(IOError)
    end
  end
end
