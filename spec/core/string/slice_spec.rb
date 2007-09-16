require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

@string_slice = shared "String#slice" do |cmd|
  describe "String##{cmd} with index" do
    it "returns the character code of the character at idx" do
      "hello".send(cmd, 0).should == ?h
      "hello".send(cmd, -1).should == ?o
    end

    it "returns nil if idx is outside of self" do
      "hello".send(cmd, 20).should == nil
      "hello".send(cmd, -20).should == nil

      "".send(cmd, 0).should == nil
      "".send(cmd, -1).should == nil
    end

    it "calls to_int on idx" do
      "hello".send(cmd, 0.5).should == ?h

      obj = Object.new
      obj.should_receive(:to_int, :returning => 1)
      "hello".send(cmd, obj).should == ?e

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
      "hello".send(cmd, obj).should == ?e
    end
  end

  describe "String##{cmd} with index, length" do
    it "returns the substring starting at idx and the given length" do
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

    it "calls to_int on idx and length" do
      "hello".send(cmd, 0.5, 1).should == "h"
      "hello".send(cmd, 0.5, 2.5).should == "he"
      "hello".send(cmd, 1, 2.5).should == "el"

      obj = Object.new
      obj.should_receive(:to_int, :count => 4, :returning => 2)

      "hello".send(cmd, obj, 1).should == "l"
      "hello".send(cmd, obj, obj).should == "ll"
      "hello".send(cmd, 0, obj).should == "he"

      obj = Object.new
      obj.should_receive(:respond_to?, :count => 2, :with => [:to_int], :returning => true)
      obj.should_receive(:method_missing, :count => 2, :with => [:to_int], :returning => 2)
      "hello".send(cmd, obj, obj).should == "ll"
    end

    it "raises TypeError when idx or length can't be converted to an integer" do
      should_raise(TypeError) { "hello".send(cmd, Object.new, 0) }
      should_raise(TypeError) { "hello".send(cmd, 0, Object.new) }

      # I'm deliberately including this here.
      # It means that str.send(cmd, other, idx) isn't supported.
      should_raise(TypeError) { "hello".send(cmd, "", 0) }
    end

    it "returns subclass instances" do
      s = MyString.new("hello")
      s.send(cmd, 0,0).class.should == MyString
      s.send(cmd, 0,4).class.should == MyString
      s.send(cmd, 1,4).class.should == MyString
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
      s = MyString.new("hello")
      s.send(cmd, 0...0).class.should == MyString
      s.send(cmd, 0..4).class.should == MyString
      s.send(cmd, 1..4).class.should == MyString
    end

    it "calls to_int on range arguments" do
      from = Object.new
      to = Object.new

      # So we can construct a range out of them...
      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      def from.to_int() 1 end
      def to.to_int() -2 end

      "hello there".send(cmd, from..to).should == "ello ther"
      "hello there".send(cmd, from...to).should == "ello the"

      from = Object.new
      to = Object.new

      def from.<=>(o) 0 end
      def to.<=>(o) 0 end

      from.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      from.should_receive(:method_missing, :with => [:to_int], :returning => 1)
      to.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      to.should_receive(:method_missing, :with => [:to_int], :returning => -2)

      "hello there".send(cmd, from..to).should == "ello ther"
    end

    it "works with Range subclasses" do
      a = "GOOD"
      range_incl = MyRange.new(1, 2)
      range_excl = MyRange.new(-3, -1, true)

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
      s = MyString.new("hello")
      s.send(cmd, //).class.should == MyString
      s.send(cmd, /../).class.should == MyString
    end
    
    it "sets $~ to MatchData when there is a match and nil when there's none" do
      'hello'.send(cmd, /./)
      $~[0].should == 'h'

      'hello'.send(cmd, /not/)
      $~.should == nil
    end
  end

  describe "String##{cmd} with Regexp, index" do
    it "returns the capture for idx" do
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

    it "returns nil if there is no capture for idx" do
      "hello there".send(cmd, /[aeiou](.)\1/, 2).should == nil
      # You can't refer to 0 using negative indices
      "hello there".send(cmd, /[aeiou](.)\1/, -2).should == nil
    end

    it "calls to_int on idx" do
      obj = Object.new
      obj.should_receive(:to_int, :returning => 2)

      "har".send(cmd, /(.)(.)(.)/, 1.5).should == "h"
      "har".send(cmd, /(.)(.)(.)/, obj).should == "a"

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
      "har".send(cmd, /(.)(.)(.)/, obj).should == "a"
    end

    it "returns subclass instances" do
      s = MyString.new("hello")
      s.send(cmd, /(.)(.)/, 0).class.should == MyString
      s.send(cmd, /(.)(.)/, 1).class.should == MyString
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
      o = Object.new
      o.should_not_receive(:to_str)

      should_raise(TypeError) { "hello".send(cmd, o) }
    end

    it "returns a subclass instance when given a subclass instance" do
      s = MyString.new("el")
      r = "hello".send(cmd, s)
      r.should == "el"
      r.class.should == MyString
    end
  end
end

describe "String#slice" do
  it_behaves_like(@string_slice, :slice)
end

describe "String#slice! with index" do
  it "deletes and return the char at the given position" do
    a = "hello"
    a.slice!(1).should == ?e
    a.should == "hllo"
    a.slice!(-1).should == ?o
    a.should == "hll"
  end
  
  it "returns nil if idx is outside of self" do
    a = "hello"
    a.slice!(20).should == nil
    a.should == "hello"
    a.slice!(-20).should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "hello".freeze.slice!(1) }
  end
  
  it "doesn't raise a TypeError if self is frozen and idx is outside of self" do
    "hello".freeze.slice!(10)
    "".freeze.slice!(0)
  end
  
  version '1.8.6' do
    it "calls to_int on index" do
      "hello".slice!(0.5).should == ?h

      obj = Object.new
      # MRI calls this twice so we can't use should_receive here.
      def obj.to_int() 1 end
      "hello".slice!(obj).should == ?e

      obj = Object.new
      def obj.respond_to?(name) name == :to_int ? true : super; end
      def obj.method_missing(name, *) name == :to_int ? 1 : super; end
      "hello".slice!(obj).should == ?e
    end
  end
  
  version '1.8.4'..'1.8.5' do
    it "raises IndexError when passed other than a Fixnum" do
      should_raise(IndexError) { "hello".slice!(0.5).should == ?h }

      obj = Object.new
      # MRI calls this twice so we can't use should_receive here.
      def obj.to_int() 1 end
      should_raise(IndexError) { "hello".slice!(obj).should == ?e }

      obj = Object.new
      def obj.respond_to?(name) name == :to_int ? true : super; end
      def obj.method_missing(name, *) name == :to_int ? 1 : super; end
      should_raise(IndexError) { "hello".slice!(obj).should == ?e }
    end
    
    it "returns nil when passed a String" do
      "hello".slice!('1.5').should == nil
    end
  end
end

describe "String#slice! with index, length" do
  it "deletes and returns the substring at idx and the given length" do
    a = "hello"
    a.slice!(1, 2).should == "el"
    a.should == "hlo"

    a.slice!(1, 0).should == ""
    a.should == "hlo"

    a.slice!(-2, 4).should == "lo"
    a.should == "h"
  end

  it "always taints resulting strings when self is tainted" do
    str = "hello world"
    str.taint

    str.slice!(0, 0).tainted?.should == true
    str.slice!(2, 1).tainted?.should == true
  end

  it "returns nil if the given position is out of self" do
    a = "hello"
    a.slice(10, 3).should == nil
    a.should == "hello"

    a.slice(-10, 20).should == nil
    a.should == "hello"
  end
  
  it "returns nil if the length is negative" do
    a = "hello"
    a.slice(4, -3).should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "hello".freeze.slice!(1, 2) }
  end
  
  it "doesn't raise a TypeError if self is frozen but the given position is out of self" do
    "hello".freeze.slice!(10, 3)
    "hello".freeze.slice!(-10, 3)
  end

  it "doesn't raise a TypeError if self is frozen but length is negative" do
    "hello".freeze.slice!(4, -3)
  end
  
  it "calls to_int on idx and length" do
    "hello".slice!(0.5, 2.5).should == "he"

    obj = Object.new
    def obj.to_int() 2 end
    "hello".slice!(obj, obj).should == "ll"

    obj = Object.new
    def obj.respond_to?(name) name == :to_int; end
    def obj.method_missing(name, *) name == :to_int ? 2 : super; end
    "hello".slice!(obj, obj).should == "ll"
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s.slice!(0, 0).class.should == MyString
    s.slice!(0, 4).class.should == MyString
  end
end

describe "String#slice! Range" do
  it "deletes and return the substring given by the offsets of the range" do
    a = "hello"
    a.slice!(1..3).should == "ell"
    a.should == "ho"
    a.slice!(0..0).should == "h"
    a.should == "o"
    a.slice!(0...0).should == ""
    a.should == "o"
    
    # Edge Case?
    "hello".slice!(-3..-9).should == ""
  end
  
  it "returns nil if the given range is out of self" do
    a = "hello"
    a.slice!(-6..-9).should == nil
    a.should == "hello"
    
    b = "hello"
    b.slice!(10..20).should == nil
    b.should == "hello"
  end
  
  it "always taints resulting strings when self is tainted" do
    str = "hello world"
    str.taint

    str.slice!(0..0).tainted?.should == true
    str.slice!(2..3).tainted?.should == true
  end

  it "returns subclass instances" do
    s = MyString.new("hello")
    s.slice!(0...0).class.should == MyString
    s.slice!(0..4).class.should == MyString
  end

  it "calls to_int on range arguments" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end

    "hello there".slice!(from..to).should == "ello ther"

    from = Object.new
    to = Object.new

    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.respond_to?(name) name == :to_int; end
    def from.method_missing(name) name == :to_int ? 1 : super; end
    def to.respond_to?(name) name == :to_int; end
    def to.method_missing(name) name == :to_int ? -2 : super; end

    "hello there".slice!(from..to).should == "ello ther"
  end
  
  it "works with Range subclasses" do
    a = "GOOD"
    range_incl = MyRange.new(1, 2)

    a.slice!(range_incl).should == "OO"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "hello".freeze.slice!(1..3) }
  end
  
  it "doesn't raise a TypeError if self is frozen but the given range is out of self" do
    "hello".freeze.slice!(10..20).should == nil
  end
end

describe "String#slice! with Regexp" do
  it "deletes and returns the first match from self" do
    s = "this is a string"
    s.slice!(/s.*t/).should == 's is a st'
    s.should == 'thiring'
    
    c = "hello hello"
    c.slice!(/llo/).should == "llo"
    c.should == "he hello"
  end
  
  it "returns nil if there was no match" do
    s = "this is a string"
    s.slice!(/zzz/).should == nil
    s.should == "this is a string"
  end
  
  it "always taints resulting strings when self or regexp is tainted" do
    strs = ["hello world"]
    strs += strs.map { |s| s.dup.taint }

    strs.each do |str|
      str = str.dup
      str.slice!(//).tainted?.should == str.tainted?
      str.slice!(/hello/).tainted?.should == str.tainted?

      tainted_re = /./
      tainted_re.taint

      str.slice!(tainted_re).tainted?.should == true
    end
  end

  it "doesn't taint self when regexp is tainted" do
    s = "hello"
    s.slice!(/./.taint)
    s.tainted?.should == false
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s.slice!(//).class.should == MyString
    s.slice!(/../).class.should == MyString
  end

  # This currently fails, but passes in a pure Rubinius environment (without mspec)
  # probably because mspec uses match internally for its operation
  it "sets $~ to MatchData when there is a match and nil when there's none" do
    'hello'.slice!(/./)
    $~[0].should == 'h'

    'hello'.slice!(/not/)
    $~.should == nil
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "this is a string".freeze.slice!(/s.*t/) }
  end
  
  it "doesn't raise a TypeError if self is frozen but there is no match" do
    "this is a string".freeze.slice!(/zzz/).should == nil
  end
end

describe "String#slice! with Regexp, index" do
  it "deletes and returns the capture for idx from self" do
    str = "hello there"
    str.slice!(/[aeiou](.)\1/, 0).should == "ell"
    str.should == "ho there"
    str.slice!(/(t)h/, 1).should == "t"
    str.should == "ho here"
  end

  it "always taints resulting strings when self or regexp is tainted" do
    strs = ["hello world"]
    strs += strs.map { |s| s.dup.taint }

    strs.each do |str|
      str = str.dup
      str.slice!(//, 0).tainted?.should == str.tainted?
      str.slice!(/hello/, 0).tainted?.should == str.tainted?

      tainted_re = /(.)(.)(.)/
      tainted_re.taint

      str.slice!(tainted_re, 1).tainted?.should == true
    end
  end
  
  it "doesn't taint self when regexp is tainted" do
    s = "hello"
    s.slice!(/(.)(.)/.taint, 1)
    s.tainted?.should == false
  end
  
  it "returns nil if there was no match" do
    s = "this is a string"
    s.slice!(/x(zzz)/, 1).should == nil
    s.should == "this is a string"
  end
  
  it "returns nil if there is no capture for idx" do
    "hello there".slice!(/[aeiou](.)\1/, 2).should == nil
    # You can't refer to 0 using negative indices
    "hello there".slice!(/[aeiou](.)\1/, -2).should == nil
  end

  it "calls to_int on idx" do
    obj = Object.new
    def obj.to_int() 2 end

    "har".slice!(/(.)(.)(.)/, 1.5).should == "h"
    "har".slice!(/(.)(.)(.)/, obj).should == "a"

    obj = Object.new
    def obj.respond_to?(name) name == :to_int; end
    def obj.method_missing(name) name == :to_int ? 2: super; end
    "har".slice!(/(.)(.)(.)/, obj).should == "a"
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s.slice!(/(.)(.)/, 0).class.should == MyString
    s.slice!(/(.)(.)/, 1).class.should == MyString
  end

  it "sets $~ to MatchData when there is a match and nil when there's none" do
    'hello'[/.(.)/, 0]
    $~[0].should == 'he'

    'hello'[/.(.)/, 1]
    $~[1].should == 'e'

    'hello'[/not/, 0]
    $~.should == nil
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "this is a string".freeze.slice!(/s.*t/) }
  end
  
  it "doesn't raise a TypeError if self is frozen but there is no match" do
    "this is a string".freeze.slice!(/zzz/, 0).should == nil
  end

  it "doesn't raise a TypeError if self is frozen but there is no capture for idx" do
    "this is a string".freeze.slice!(/(.)/, 2).should == nil
  end
end

describe "String#slice! with String" do
  it "removes and returns the first occurrence of other_str from self" do
    c = "hello hello"
    c.slice!('llo').should == "llo"
    c.should == "he hello"
  end
  
  it "taints resulting strings when other is tainted" do
    strs = ["", "hello world", "hello"]
    strs += strs.map { |s| s.dup.taint }

    strs.each do |str|
      str = str.dup
      strs.each do |other|
        other = other.dup
        r = str.slice!(other)

        r.tainted?.should == !r.nil? & other.tainted?
      end
    end
  end
  
  it "doesn't set $~" do
    $~ = nil
    
    'hello'.slice!('ll')
    $~.should == nil
  end
  
  it "returns nil if self does not contain other" do
    a = "hello"
    a.slice!('zzz').should == nil
    a.should == "hello"
  end
  
  it "doesn't call to_str on its argument" do
    o = Object.new
    o.should_not_receive(:to_str)

    should_raise(TypeError) { "hello".slice!(o) }
  end

  it "returns a subclass instance when given a subclass instance" do
    s = MyString.new("el")
    r = "hello".slice!(s)
    r.should == "el"
    r.class.should == MyString
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "hello hello".freeze.slice!('llo') }
  end
  
  it "doesn't raise a TypeError if self is frozen but self does not contain other" do
    "this is a string".freeze.slice!('zzz').should == nil
  end
end
