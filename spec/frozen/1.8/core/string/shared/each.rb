shared :string_each do |cmd|
  describe "String##{cmd}" do
    it "splits self using the supplied record separator and passes each substring to the block" do
      a = []
      "one\ntwo\r\nthree".send(cmd, "\n") { |s| a << s }
      a.should == ["one\n", "two\r\n", "three"]
      
      b = []
      "hello\nworld".send(cmd, 'l') { |s| b << s }
      b.should == [ "hel", "l", "o\nworl", "d" ]
      
      c = []
      "hello\n\n\nworld".send(cmd, "\n") { |s| c << s }
      c.should == ["hello\n", "\n", "\n", "world"]
    end
    
    it "taints substrings that are passed to the block if self is tainted" do
      "one\ntwo\r\nthree".taint.send(cmd) { |s| s.tainted?.should == true }
  
      "x.y.".send(cmd, ".".taint) { |s| s.tainted?.should == false }
    end
    
    it "passes self as a whole to the block if the separator is nil" do
      a = []
      "one\ntwo\r\nthree".send(cmd, nil) { |s| a << s }
      a.should == ["one\ntwo\r\nthree"]
    end
    
    it "appends multiple successive newlines together when the separator is an empty string" do
      a = []
      "hello\nworld\n\n\nand\nuniverse\n\n\n\n\n".send(cmd, '') { |s| a << s }
      a.should == ["hello\nworld\n\n\n", "and\nuniverse\n\n\n\n\n"]
    end
  
    it "uses $/ as the separator when none is given" do
      [
        "", "x", "x\ny", "x\ry", "x\r\ny", "x\n\r\r\ny",
        "hello hullo bello"
      ].each do |str|
        ["", "llo", "\n", "\r", nil].each do |sep|
          begin
            expected = []
            str.send(cmd, sep) { |x| expected << x }
  
            old_rec_sep, $/ = $/, sep
  
            actual = []
            str.send(cmd) { |x| actual << x }
  
            actual.should == expected
          ensure
            $/ = old_rec_sep
          end
        end
      end
    end
    
    it "yields subclass instances for subclasses" do
      a = []
      StringSpecs::MyString.new("hello\nworld").send(cmd) { |s| a << s.class }
      a.should == [StringSpecs::MyString, StringSpecs::MyString]
    end
    
    it "returns self" do
      s = "hello\nworld"
      (s.send(cmd) {}).should equal(s)
    end
  
    it "tries to convert the separator to a string using to_str" do
      separator = mock('l')
      def separator.to_str() 'l' end
      
      a = []
      "hello\nworld".send(cmd, separator) { |s| a << s }
      a.should == [ "hel", "l", "o\nworl", "d" ]
      
      obj = mock('l')
      obj.should_receive(:respond_to?).with(:to_str).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_str).and_return("l")
      
      a = []
      "hello\nworld".send(cmd, obj) { |s| a << s }
      a.should == [ "hel", "l", "o\nworl", "d" ]
    end
    
    it "raises a RuntimeError if the string is modified while substituting" do
      str = "hello\nworld"
      lambda { str.send(cmd) { str[0] = 'x' } }.should raise_error(RuntimeError)
    end
    
    it "raises a TypeError when the separator can't be converted to a string" do
      lambda { "hello world".send(cmd, false) {}     }.should raise_error(TypeError)
      lambda { "hello world".send(cmd, ?o) {}        }.should raise_error(TypeError)
      lambda { "hello world".send(cmd, :o) {}        }.should raise_error(TypeError)
      lambda { "hello world".send(cmd, mock('x')) {} }.should raise_error(TypeError)
    end
  end
end
