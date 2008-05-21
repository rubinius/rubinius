shared :string_slice do |cmd|
  describe "String##{cmd} with index" do
    it "returns the character code of the character at the given index" do
      "hello".send(cmd, 0).should == ?h
      "hello".send(cmd, -1).should == ?o
    end

    it "returns nil if index is outside of self" do
      "hello".send(cmd, 20).should == nil
      "hello".send(cmd, -20).should == nil

      "".send(cmd, 0).should == nil
      "".send(cmd, -1).should == nil
    end

    it "calls to_int on the given index" do
      "hello".send(cmd, 0.5).should == ?h

      obj = mock('1')
      obj.should_receive(:to_int).and_return(1)
      "hello".send(cmd, obj).should == ?e

      obj = mock('1')
      obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_int).and_return(1)
      "hello".send(cmd, obj).should == ?e
    end
    
    it "raises a TypeError if the given index is nil" do
      lambda { "hello".send(cmd, nil) }.should raise_error(TypeError)
    end
    
    it "raises a TypeError if the given index can't be converted to an Integer" do
      lambda { "hello".send(cmd, mock('x')) }.should raise_error(TypeError)
      lambda { "hello".send(cmd, {})        }.should raise_error(TypeError)
      lambda { "hello".send(cmd, [])        }.should raise_error(TypeError)
    end
  end

  describe "String##{cmd} with index, length" do
    it "returns the substring starting at the given index with the given length" do
      "hello there".send(cmd, 0,0).should == ""
      "hello there".send(cmd, 0,1).should == "h"
      "hello there".send(cmd, 0,3).should == "hel"
      "hello there".send(cmd, 0,6).should == "hello "
      "hello there".send(cmd, 0,9).should == "hello the"
      "hello there".send(cmd, 0,12).should == "hello there"

      "hello there".send(cmd, 1,0).should == ""
      "hello there".send(cmd, 1,1).should == "e"
      "hello there".send(cmd, 1,3).should == "ell"
      "hello there".send(cmd, 1,6).should == "ello t"
      "hello there".send(cmd, 1,9).should == "ello ther"
      "hello there".send(cmd, 1,12).should == "ello there"

      "hello there".send(cmd, 3,0).should == ""
      "hello there".send(cmd, 3,1).should == "l"
      "hello there".send(cmd, 3,3).should == "lo "
      "hello there".send(cmd, 3,6).should == "lo the"
      "hello there".send(cmd, 3,9).should == "lo there"

      "hello there".send(cmd, 4,0).should == ""
      "hello there".send(cmd, 4,3).should == "o t"
      "hello there".send(cmd, 4,6).should == "o ther"
      "hello there".send(cmd, 4,9).should == "o there"

      "foo".send(cmd, 2,1).should == "o"
      "foo".send(cmd, 3,0).should == ""
      "foo".send(cmd, 3,1).should == ""

      "".send(cmd, 0,0).should == ""
      "".send(cmd, 0,1).should == ""

      "x".send(cmd, 0,0).should == ""
      "x".send(cmd, 0,1).should == "x"
      "x".send(cmd, 1,0).should == ""
      "x".send(cmd, 1,1).should == ""

      "x".send(cmd, -1,0).should == ""
      "x".send(cmd, -1,1).should == "x"

      "hello there".send(cmd, -3,2).should == "er"
    end

    it "always taints resulting strings when self is tainted" do
      str = "hello world"
      str.taint

      str.send(cmd, 0,0).tainted?.should == true
      str.send(cmd, 0,1).tainted?.should == true
      str.send(cmd, 2,1).tainted?.should == true
    end

    it "returns nil if the offset falls outside of self" do
      "hello there".send(cmd, 20,3).should == nil
      "hello there".send(cmd, -20,3).should == nil

      "".send(cmd, 1,0).should == nil
      "".send(cmd, 1,1).should == nil

      "".send(cmd, -1,0).should == nil
      "".send(cmd, -1,1).should == nil

      "x".send(cmd, 2,0).should == nil
      "x".send(cmd, 2,1).should == nil

      "x".send(cmd, -2,0).should == nil
      "x".send(cmd, -2,1).should == nil
    end

    it "returns nil if the length is negative" do
      "hello there".send(cmd, 4,-3).should == nil
      "hello there".send(cmd, -4,-3).should == nil
    end

    it "calls to_int on the given index and the given length" do
      "hello".send(cmd, 0.5, 1).should == "h"
      "hello".send(cmd, 0.5, 2.5).should == "he"
      "hello".send(cmd, 1, 2.5).should == "el"

      obj = mock('2')
      obj.should_receive(:to_int).exactly(4).times.and_return(2)

      "hello".send(cmd, obj, 1).should == "l"
      "hello".send(cmd, obj, obj).should == "ll"
      "hello".send(cmd, 0, obj).should == "he"

      obj = mock('2')
      obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_int).exactly(2).times.and_return(2)
      "hello".send(cmd, obj, obj).should == "ll"
    end

    it "raises a TypeError when idx or length can't be converted to an integer" do
      lambda { "hello".send(cmd, mock('x'), 0) }.should raise_error(TypeError)
      lambda { "hello".send(cmd, 0, mock('x')) }.should raise_error(TypeError)

      # I'm deliberately including this here.
      # It means that str.send(cmd, other, idx) isn't supported.
      lambda { "hello".send(cmd, "", 0) }.should raise_error(TypeError)
    end
    
    it "raises a TypeError when the given index or the given length is nil" do
      lambda { "hello".send(cmd, 1, nil)   }.should raise_error(TypeError)
      lambda { "hello".send(cmd, nil, 1)   }.should raise_error(TypeError)
      lambda { "hello".send(cmd, nil, nil) }.should raise_error(TypeError)
    end

    it "returns subclass instances" do
      s = StringSpecs::MyString.new("hello")
      s.send(cmd, 0,0).class.should == StringSpecs::MyString
      s.send(cmd, 0,4).class.should == StringSpecs::MyString
      s.send(cmd, 1,4).class.should == StringSpecs::MyString
    end
  end

  describe "String##{cmd} with Range" do
    it "returns the substring given by the offsets of the range" do
      "hello there".send(cmd, 1..1).should == "e"
      "hello there".send(cmd, 1..3).should == "ell"
      "hello there".send(cmd, 1...3).should == "el"
      "hello there".send(cmd, -4..-2).should == "her"
      "hello there".send(cmd, -4...-2).should == "he"
      "hello there".send(cmd, 5..-1).should == " there"
      "hello there".send(cmd, 5...-1).should == " ther"

      "".send(cmd, 0..0).should == ""

      "x".send(cmd, 0..0).should == "x"
      "x".send(cmd, 0..1).should == "x"
      "x".send(cmd, 0...1).should == "x"
      "x".send(cmd, 0..-1).should == "x"

      "x".send(cmd, 1..1).should == ""
      "x".send(cmd, 1..-1).should == ""
    end

    it "returns nil if the beginning of the range falls outside of self" do
      "hello there".send(cmd, 12..-1).should == nil
      "hello there".send(cmd, 20..25).should == nil
      "hello there".send(cmd, 20..1).should == nil
      "hello there".send(cmd, -20..1).should == nil
      "hello there".send(cmd, -20..-1).should == nil

      "".send(cmd, -1..-1).should == nil
      "".send(cmd, -1...-1).should == nil
      "".send(cmd, -1..0).should == nil
      "".send(cmd, -1...0).should == nil
    end

    it "returns an empty string if range.begin is inside self and > real end" do
      "hello there".send(cmd, 1...1).should == ""
      "hello there".send(cmd, 4..2).should == ""
      "hello".send(cmd, 4..-4).should == ""
      "hello there".send(cmd, -5..-6).should == ""
      "hello there".send(cmd, -2..-4).should == ""
      "hello there".send(cmd, -5..-6).should == ""
      "hello there".send(cmd, -5..2).should == ""

      "".send(cmd, 0...0).should == ""
      "".send(cmd, 0..-1).should == ""
      "".send(cmd, 0...-1).should == ""

      "x".send(cmd, 0...0).should == ""
      "x".send(cmd, 0...-1).should == ""
      "x".send(cmd, 1...1).should == ""
      "x".send(cmd, 1...-1).should == ""
    end

    it "always taints resulting strings when self is tainted" do
      str = "hello world"
      str.taint

      str.send(cmd, 0..0).tainted?.should == true
      str.send(cmd, 0...0).tainted?.should == true
      str.send(cmd, 0..1).tainted?.should == true
      str.send(cmd, 0...1).tainted?.should == true
      str.send(cmd, 2..3).tainted?.should == true
      str.send(cmd, 2..0).tainted?.should == true
    end

    it "returns subclass instances" do
      s = StringSpecs::MyString.new("hello")
      s.send(cmd, 0...0).class.should == StringSpecs::MyString
      s.send(cmd, 0..4).class.should == StringSpecs::MyString
      s.send(cmd, 1..4).class.should == StringSpecs::MyString
    end

    it "calls to_int on range arguments" do
      from = mock('from')
      to = mock('to')

      # So we can construct a range out of them...
      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      def from.to_int() 1 end
      def to.to_int() -2 end

      "hello there".send(cmd, from..to).should == "ello ther"
      "hello there".send(cmd, from...to).should == "ello the"

      from = mock('from')
      to = mock('to')

      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      from.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      from.should_receive(:method_missing).with(:to_int).and_return(1)
      to.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      to.should_receive(:method_missing).with(:to_int).and_return(-2)

      "hello there".send(cmd, from..to).should == "ello ther"
    end

    it "works with Range subclasses" do
      a = "GOOD"
      range_incl = StringSpecs::MyRange.new(1, 2)
      range_excl = StringSpecs::MyRange.new(-3, -1, true)

      a.send(cmd, range_incl).should == "OO"
      a.send(cmd, range_excl).should == "OO"
    end
  end

  describe "String##{cmd} with Regexp" do
    it "returns the matching portion of self" do
      "hello there".send(cmd, /[aeiou](.)\1/).should == "ell"
      "".send(cmd, //).should == ""
    end

    it "returns nil if there is no match" do
      "hello there".send(cmd, /xyz/).should == nil
    end

    it "always taints resulting strings when self or regexp is tainted" do
      strs = ["hello world"]
      strs += strs.map { |s| s.dup.taint }

      strs.each do |str|
        str.send(cmd, //).tainted?.should == str.tainted?
        str.send(cmd, /hello/).tainted?.should == str.tainted?

        tainted_re = /./
        tainted_re.taint

        str.send(cmd, tainted_re).tainted?.should == true
      end
    end

    it "returns subclass instances" do
      s = StringSpecs::MyString.new("hello")
      s.send(cmd, //).class.should == StringSpecs::MyString
      s.send(cmd, /../).class.should == StringSpecs::MyString
    end
    
    it "sets $~ to MatchData when there is a match and nil when there's none" do
      'hello'.send(cmd, /./)
      $~[0].should == 'h'

      'hello'.send(cmd, /not/)
      $~.should == nil
    end
  end

  describe "String##{cmd} with Regexp, index" do
    it "returns the capture for the given index" do
      "hello there".send(cmd, /[aeiou](.)\1/, 0).should == "ell"
      "hello there".send(cmd, /[aeiou](.)\1/, 1).should == "l"
      "hello there".send(cmd, /[aeiou](.)\1/, -1).should == "l"

      "har".send(cmd, /(.)(.)(.)/, 0).should == "har"
      "har".send(cmd, /(.)(.)(.)/, 1).should == "h"
      "har".send(cmd, /(.)(.)(.)/, 2).should == "a"
      "har".send(cmd, /(.)(.)(.)/, 3).should == "r"
      "har".send(cmd, /(.)(.)(.)/, -1).should == "r"
      "har".send(cmd, /(.)(.)(.)/, -2).should == "a"
      "har".send(cmd, /(.)(.)(.)/, -3).should == "h"
    end

    it "always taints resulting strings when self or regexp is tainted" do
      strs = ["hello world"]
      strs += strs.map { |s| s.dup.taint }

      strs.each do |str|
        str.send(cmd, //, 0).tainted?.should == str.tainted?
        str.send(cmd, /hello/, 0).tainted?.should == str.tainted?

        str.send(cmd, /(.)(.)(.)/, 0).tainted?.should == str.tainted?
        str.send(cmd, /(.)(.)(.)/, 1).tainted?.should == str.tainted?
        str.send(cmd, /(.)(.)(.)/, -1).tainted?.should == str.tainted?
        str.send(cmd, /(.)(.)(.)/, -2).tainted?.should == str.tainted?

        tainted_re = /(.)(.)(.)/
        tainted_re.taint

        str.send(cmd, tainted_re, 0).tainted?.should == true
        str.send(cmd, tainted_re, 1).tainted?.should == true
        str.send(cmd, tainted_re, -1).tainted?.should == true
      end
    end

    it "returns nil if there is no match" do
      "hello there".send(cmd, /(what?)/, 1).should == nil
    end

    it "returns nil if there is no capture for the given index" do
      "hello there".send(cmd, /[aeiou](.)\1/, 2).should == nil
      # You can't refer to 0 using negative indices
      "hello there".send(cmd, /[aeiou](.)\1/, -2).should == nil
    end

    it "calls to_int on the given index" do
      obj = mock('2')
      obj.should_receive(:to_int).and_return(2)

      "har".send(cmd, /(.)(.)(.)/, 1.5).should == "h"
      "har".send(cmd, /(.)(.)(.)/, obj).should == "a"

      obj = mock('2')
      obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_int).and_return(2)
      "har".send(cmd, /(.)(.)(.)/, obj).should == "a"
    end
    
    it "raises a TypeError when the given index can't be converted to Integer" do
      lambda { "hello".send(cmd, /(.)(.)(.)/, mock('x')) }.should raise_error(TypeError)
      lambda { "hello".send(cmd, /(.)(.)(.)/, {})        }.should raise_error(TypeError)
      lambda { "hello".send(cmd, /(.)(.)(.)/, [])        }.should raise_error(TypeError)
    end
    
    it "raises a TypeError when the given index is nil" do
      lambda { "hello".send(cmd, /(.)(.)(.)/, nil) }.should raise_error(TypeError)
    end

    it "returns subclass instances" do
      s = StringSpecs::MyString.new("hello")
      s.send(cmd, /(.)(.)/, 0).class.should == StringSpecs::MyString
      s.send(cmd, /(.)(.)/, 1).class.should == StringSpecs::MyString
    end
    
    it "sets $~ to MatchData when there is a match and nil when there's none" do
      'hello'.send(cmd, /.(.)/, 0)
      $~[0].should == 'he'

      'hello'.send(cmd, /.(.)/, 1)
      $~[1].should == 'e'

      'hello'.send(cmd, /not/, 0)
      $~.should == nil
    end
  end

  describe "String##{cmd} with String" do
    it "returns other_str if it occurs in self" do
      s = "lo"
      "hello there".send(cmd, s).should == s
    end

    it "taints resulting strings when other is tainted" do
      strs = ["", "hello world", "hello"]
      strs += strs.map { |s| s.dup.taint }

      strs.each do |str|
        strs.each do |other|
          r = str.send(cmd, other)

          r.tainted?.should == !r.nil? & other.tainted?
        end
      end
    end
    
    it "doesn't set $~" do
      $~ = nil
      
      'hello'.send(cmd, 'll')
      $~.should == nil
    end

    it "returns nil if there is no match" do
      "hello there".send(cmd, "bye").should == nil
    end

    it "doesn't call to_str on its argument" do
      o = mock('x')
      o.should_not_receive(:to_str)

      lambda { "hello".send(cmd, o) }.should raise_error(TypeError)
    end

    it "returns a subclass instance when given a subclass instance" do
      s = StringSpecs::MyString.new("el")
      r = "hello".send(cmd, s)
      r.should == "el"
      r.class.should == StringSpecs::MyString
    end
  end
end