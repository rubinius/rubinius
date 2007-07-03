require File.dirname(__FILE__) + '/../spec_helper'

# %, *, +, <<, <=>, ==, =~, [], []=, capitalize, capitalize!,
# casecmp, center, chomp, chomp!, chop, chop!, concat, count, crypt,
# delete, delete!, downcase, downcase!, dump, each, each_byte,
# each_line, empty?, eql?, gsub, gsub!, hash, hex, include?, index,
# initialize_copy, insert, inspect, intern, length, ljust, lstrip,
# lstrip!, match, next, next!, oct, replace, reverse, reverse!,
# rindex, rjust, rstrip, rstrip!, scan, size, slice, slice!, split,
# squeeze, squeeze!, strip, strip!, sub, sub!, succ, succ!, sum,
# swapcase, swapcase!, to_f, to_i, to_s, to_str, to_sym, tr, tr!,
# tr_s, tr_s!, unpack, upcase, upcase!, upto

class MyString < String; end
class MyArray < Array; end

describe "String#%(Object)" do
  it "formats multiple expressions" do
    ("%b %x %d %s" % [10, 10, 10, 10]).should == "1010 a 10 10"
  end
  
  it "formats expressions mid string" do
    ("hello %s!" % "world").should == "hello world!"
  end
  
  it "formats %% into %" do
    ("%d%% %s" % [10, "of chickens!"]).should == "10% of chickens!"
  end

  it "ignores unused arguments when $DEBUG is false" do
    begin
      old_debug = $DEBUG
      $DEBUG = false

      ("" % [1, 2, 3]).should == ""
      ("%s" % [1, 2, 3]).should == "1"
    ensure
      $DEBUG = old_debug
    end
  end

  it "raises ArgumentError for unused arguments when $DEBUG is true" do
    begin
      old_debug = $DEBUG
      $DEBUG = true

      should_raise(ArgumentError) { "" % [1, 2, 3] }
      should_raise(ArgumentError) { "%s" % [1, 2, 3] }
    ensure
      $DEBUG = old_debug
    end
  end
  
  it "always allows unused arguments when positional argument style is used" do
    begin
      old_debug = $DEBUG

      $DEBUG = false
      ("%2$s" % [1, 2, 3]).should == "2"
      $DEBUG = true
      ("%2$s" % [1, 2, 3]).should == "2"
    ensure
      $DEBUG = old_debug
    end
  end
  
  it "ignores percent signs at end of string / before newlines" do
    ("%" % []).should == "%"
    ("foo%" % []).should == "foo%"
  end
  
  it "replaces percent sign followed by null byte with a percent sign" do
    ("%\0x hello" % []).should == "%x hello"
  end

  it "replaces trailing absolute argument specifier without type with percent sign" do
    ("hello %1$" % "foo").should == "hello %"
  end
  
  it "raises an ArgumentError when given invalid argument specifiers" do
    should_raise(ArgumentError) { "%1" % [] }
    should_raise(ArgumentError) { "%+" % [] }
    should_raise(ArgumentError) { "%-" % [] }
    should_raise(ArgumentError) { "%#" % [] }
    should_raise(ArgumentError) { "%0" % [] }
    should_raise(ArgumentError) { "%*" % [] }
    should_raise(ArgumentError) { "%." % [] }
    should_raise(ArgumentError) { "%_" % [] }
    should_raise(ArgumentError) { "%0$s" % "x" }
    should_raise(ArgumentError) { "%*0$s" % [5, "x"] }
    should_raise(ArgumentError) { "%*1$.*0$1$s" % [1, 2, 3] }
  end

  it "raises an ArgumentError when multiple positional argument tokens are given for one format specifier" do
    should_raise(ArgumentError) { "%1$1$s" % "foo" }
  end

  it "raises an ArgumentError when multiple width star tokens are given for one format specifier" do
    should_raise(ArgumentError) { "%**s" % [5, 5, 5] }
  end

  it "raises an ArgumentError when a width star token is seen after a width token" do
    should_raise(ArgumentError) { "%5*s" % [5, 5] }
  end

  it "raises an ArgumentError when multiple precision tokens are given" do
    should_raise(ArgumentError) { "%.5.5s" % 5 }
    should_raise(ArgumentError) { "%.5.*s" % [5, 5] }
    should_raise(ArgumentError) { "%.*.5s" % [5, 5] }
  end
  
  it "raises an ArgumentError when there are less arguments than format specifiers" do
    ("foo" % []).should == "foo"
    should_raise(ArgumentError) { "%s" % [] }
    should_raise(ArgumentError) { "%s %s" % [1] }
  end
  
  it "raises an ArgumentError when absolute and relative argument numbers are mixed" do
    should_raise(ArgumentError) { "%s %1$s" % "foo" }
    should_raise(ArgumentError) { "%1$s %s" % "foo" }

    should_raise(ArgumentError) { "%s %2$s" % ["foo", "bar"] }
    should_raise(ArgumentError) { "%2$s %s" % ["foo", "bar"] }

    should_raise(ArgumentError) { "%*2$s" % [5, 5, 5] }
    should_raise(ArgumentError) { "%*.*2$s" % [5, 5, 5] }
    should_raise(ArgumentError) { "%*2$.*2$s" % [5, 5, 5] }
    should_raise(ArgumentError) { "%*.*2$s" % [5, 5, 5] }
  end
  
  it "allows reuse of the one argument multiple via absolute argument numbers" do
    ("%1$s %1$s" % "foo").should == "foo foo"
    ("%1$s %2$s %1$s %2$s" % ["foo", "bar"]).should == "foo bar foo bar"
  end
  
  it "always interprets an array argument as a list of argument parameters" do
    should_raise(ArgumentError) { "%p" % [] }
    ("%p" % [1]).should == "1"
    ("%p %p" % [1, 2]).should == "1 2"
  end

  it "always interprets an array subclass argument as a list of argument parameters" do
    should_raise(ArgumentError) { "%p" % MyArray[] }
    ("%p" % MyArray[1]).should == "1"
    ("%p %p" % MyArray[1, 2]).should == "1 2"
  end
  
  it "allows positional arguments for width star and precision star arguments" do
    ("%*1$.*2$3$d" % [10, 5, 1]).should == "     00001"
  end
  
  it "calls to_int on width star and precision star tokens" do
    w = Object.new
    def w.to_int() 10 end
    p = Object.new
    def p.to_int() 5 end
    
    ("%*.*f" % [w, p, 1]).should == "   1.00000"
  end
  
  it "doesn't call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2] end
    def obj.to_s() "obj" end
    should_raise(ArgumentError) { "%s %s" % obj }
    ("%s" % obj).should == "obj"
  end

  it "supports binary formats using %b" do
    ("%b" % 10).should == "1010"
    ("% b" % 10).should == " 1010"
    ("%1$b" % [10, 20]).should == "1010"
    ("%#b" % 10).should == "0b1010"
    ("%+b" % 10).should == "+1010"
    ("%-9b" % 10).should == "1010     "
    ("%05b" % 10).should == "01010"
    ("%*b" % [10, 6]).should == "       110"
    ("%*b" % [-10, 6]).should == "110       "
    
    ("%b" % -5).should == "..1011"
    ("%0b" % -5).should == "1011"
    ("%.1b" % -5).should == "1011"
    ("%.7b" % -5).should == "1111011"
    ("%.10b" % -5).should == "1111111011"
    ("% b" % -5).should == "-101"
    ("%+b" % -5).should == "-101"
    ("%b" % -(2 ** 64 + 5)).should ==
    "..101111111111111111111111111111111111111111111111111111111111111011"
  end
  
  it "supports binary formats using %B with same behaviour as %b except for using 0B instead of 0b for #" do
    ("%B" % 10).should == ("%b" % 10)
    ("% B" % 10).should == ("% b" % 10)
    ("%1$B" % [10, 20]).should == ("%1$b" % [10, 20])
    ("%+B" % 10).should == ("%+b" % 10)
    ("%-9B" % 10).should == ("%-9b" % 10)
    ("%05B" % 10).should == ("%05b" % 10)
    ("%*B" % [10, 6]).should == ("%*b" % [10, 6])
    ("%*B" % [-10, 6]).should == ("%*b" % [-10, 6])

    ("%B" % -5).should == ("%b" % -5)
    ("%0B" % -5).should == ("%0b" % -5)
    ("%.1B" % -5).should == ("%.1b" % -5)
    ("%.7B" % -5).should == ("%.7b" % -5)
    ("%.10B" % -5).should == ("%.10b" % -5)
    ("% B" % -5).should == ("% b" % -5)
    ("%+B" % -5).should == ("%+b" % -5)
    ("%B" % -(2 ** 64 + 5)).should == ("%b" % -(2 ** 64 + 5))

    ("%#B" % 10).should == "0B1010"
  end
    
  it "supports character formats using %c" do
    ("%c" % 10).should == "\n"
    ("%2$c" % [10, 11, 14]).should == "\v"
    ("%-4c" % 10).should == "\n   "
    ("%*c" % [10, 3]).should == "         \003"
    ("%c" % (256 + 42)).should == "*"
    
    should_raise(TypeError) { "%c" % Object }
    
    obj = Object.new
    def obj.to_int() 65 end
    ("%c" % obj).should == ("%c" % obj.to_int)
  end
  
  %w(d i).each do |f|
    format = "%" + f
    
    it "supports integer formats using #{format}" do
      ("%#{f}" % 10).should == "10"
      ("% #{f}" % 10).should == " 10"
      ("%1$#{f}" % [10, 20]).should == "10"
      ("%+#{f}" % 10).should == "+10"
      ("%-7#{f}" % 10).should == "10     "
      ("%04#{f}" % 10).should == "0010"
      ("%*#{f}" % [10, 4]).should == "         4"
    end
  end
  
  it "supports float formats using %E" do
    ("%E" % 10).should == "1.000000E+01"
    ("% E" % 10).should == " 1.000000E+01"
    ("%1$E" % 10).should == "1.000000E+01"
    ("%#E" % 10).should == "1.000000E+01"
    ("%+E" % 10).should == "+1.000000E+01"
    ("%-7E" % 10).should == "1.000000E+01"
    ("%05E" % 10).should == "1.000000E+01"
    ("%*E" % [10, 9]).should == "9.000000E+00"
  end

  it "supports float formats using %e" do
    ("%e" % 10).should == "1.000000e+01"
    ("% e" % 10).should == " 1.000000e+01"
    ("%1$e" % 10).should == "1.000000e+01"
    ("%#e" % 10).should == "1.000000e+01"
    ("%+e" % 10).should == "+1.000000e+01"
    ("%-7e" % 10).should == "1.000000e+01"
    ("%05e" % 10).should == "1.000000e+01"
    ("%*e" % [10, 9]).should == "9.000000e+00"
    ("%e" % (0.0/0)).should == "nan"
  end
  
  it "supports float formats using %f" do
    ("%f" % 10).should == "10.000000"
    ("% f" % 10).should == " 10.000000"
    ("%1$f" % 10).should == "10.000000"
    ("%#f" % 10).should == "10.000000"
    ("%+f" % 10).should == "+10.000000"
    ("%-7f" % 10).should == "10.000000"
    ("%05f" % 10).should == "10.000000"
    ("%*f" % [10, 9]).should == "  9.000000"
  end
  
  it "supports float formats using %G" do
    ("%G" % 10).should == "10"
    ("% G" % 10).should == " 10"
    ("%1$G" % 10).should == "10"
    ("%#G" % 10).should == "10.0000"
    ("%+G" % 10).should == "+10"
    ("%-7G" % 10).should == "10     "
    ("%05G" % 10).should == "00010"
    ("%*G" % [10, 9]).should == "         9"
  end
  
  it "supports float formats using %g" do
    ("%g" % 10).should == "10"
    ("% g" % 10).should == " 10"
    ("%1$g" % 10).should == "10"
    ("%#g" % 10).should == "10.0000"
    ("%+g" % 10).should == "+10"
    ("%-7g" % 10).should == "10     "
    ("%05g" % 10).should == "00010"
    ("%*g" % [10, 9]).should == "         9"
  end
  
  it "supports octal formats using %o" do
    ("%o" % 10).should == "12"
    ("% o" % 10).should == " 12"
    ("%1$o" % [10, 20]).should == "12"
    ("%#o" % 10).should == "012"
    ("%+o" % 10).should == "+12"
    ("%-9o" % 10).should == "12       "
    ("%05o" % 10).should == "00012"
    ("%*o" % [10, 6]).should == "         6"

    ("%o" % -5).should == "..73"
    ("%0o" % -5).should == "73"
    ("%.1o" % -5).should == "73"
    ("%.7o" % -5).should == "7777773"
    ("%.10o" % -5).should == "7777777773"
    ("% o" % -26).should == "-32"
    ("%+o" % -26).should == "-32"
    ("%o" % -(2 ** 64 + 5)).should == "..75777777777777777777773"
  end
  
  it "supports inspect formats using %p" do
    ("%p" % 10).should == "10"
    ("%1$p" % [10, 5]).should == "10"
    ("%-22p" % 10).should == "10                    "
    ("%*p" % [10, 10]).should == "        10"

    obj = Object.new
    obj.should_receive(:inspect, :returning => "obj")
    ("%p" % obj).should == "obj"
  end
  
  it "supports string formats using %s" do
    ("%s" % 10).should == "10"
    ("%1$s" % [10, 8]).should == "10"
    ("%-5s" % 10).should == "10   "
    ("%*s" % [10, 9]).should == "         9"

    obj = Object.new
    obj.should_receive(:to_s, :returning => "obj")
    ("%s" % obj).should == "obj"
  end

  # MRI crashes on this one.
  # See http://groups.google.com/group/ruby-core-google/t/c285c18cd94c216d
  failure :mri do
    it "ignores huge precisions for %s" do
      ("%.25555555555555555555555555555555555555s" % "hello world").should ==
      "hello world"
    end
  end
  
  it "supports unsigned formats using %u" do
    ("%u" % 10).should == "10"
    ("% u" % 10).should == " 10"
    ("%1$u" % [10, 20]).should == "10"
    ("%+u" % 10).should == "+10"
    ("%-7u" % 10).should == "10     "
    ("%04u" % 10).should == "0010"
    ("%*u" % [10, 4]).should == "         4"
    
    ("%u" % -5).should == "..4294967291"
    ("%0u" % -5).should == "4294967291"
    ("%.1u" % -5).should == "4294967291"
    ("%.7u" % -5).should == "4294967291"
    ("%.10u" % -5).should == "4294967291"
    ("% u" % -26).should == "-26"
    ("%+u" % -26).should == "-26"
    ("%u" % -(2 ** 64 + 5)).should == "-79228162495817593519834398715"
  end
  
  it "supports hex formats using %x" do
    ("%x" % 10).should == "a"
    ("% x" % 10).should == " a"
    ("%1$x" % [10, 20]).should == "a"
    ("%#x" % 10).should == "0xa"
    ("%+x" % 10).should == "+a"
    ("%-9x" % 10).should == "a        "
    ("%05x" % 10).should == "0000a"
    ("%*x" % [10, 6]).should == "         6"

    ("%x" % -5).should == "..fb"
    ("%0x" % -5).should == "fb"
    ("%.1x" % -5).should == "fb"
    ("%.7x" % -5).should == "ffffffb"
    ("%.10x" % -5).should == "fffffffffb"
    ("% x" % -26).should == "-1a"
    ("%+x" % -26).should == "-1a"
    ("%x" % -(2 ** 64 + 5)).should == "..fefffffffffffffffb"
  end
  
  it "supports hex formats using %X" do
    ("%X" % 10).should == "A"
    ("% X" % 10).should == " A"
    ("%1$X" % [10, 20]).should == "A"
    ("%#X" % 10).should == "0XA"
    ("%+X" % 10).should == "+A"
    ("%-9X" % 10).should == "A        "
    ("%05X" % 10).should == "0000A"
    ("%*X" % [10, 6]).should == "         6"
    
    ("%X" % -5).should == "..FB"
    ("%0X" % -5).should == "FB"
    ("%.1X" % -5).should == "FB"
    ("%.7X" % -5).should == "FFFFFFB"
    ("%.10X" % -5).should == "FFFFFFFFFB"
    ("% X" % -26).should == "-1A"
    ("%+X" % -26).should == "-1A"
    ("%X" % -(2 ** 64 + 5)).should == "..FEFFFFFFFFFFFFFFFB"
  end
  
  %w(b d i o u X x).each do |f|
    format = "%" + f
    
    it "calls to_i on #{format} arguments" do
      (format % "10").should == (format % 10)
      (format % nil).should == (format % 0)
      
      obj = Object.new
      obj.should_receive(:to_i, :returning => 5)
      (format % obj).should == (format % 5)
    end
  end
  
  %w(E e f G g).each do |f|
    format = "%" + f
    
    it "calls to_f on #{format} arguments" do
      (format % 10).should == (format % 10.0)
      
      obj = Object.new
      obj.should_receive(:to_f, :returning => 5.0)
      (format % obj).should == (format % 5.0)
    end
  end
end

describe "String#*(count)" do
  it "returns a new string containing count copies of self" do
    ("cool" * 0).should == ""
    ("cool" * 1).should == "cool"
    ("cool" * 3).should == "coolcoolcool"
  end
  
  it "tries to convert the given argument to an integer using to_int" do
    ("cool" * 3.1).should == "coolcoolcool"
    ("a" * 3.999).should == "aaa"
    
    a = Object.new
    def a.to_int() 4; end
    
    ("a" * a).should == "aaaa"
  end
  
  it "raises an ArgumentError when given integer is negative" do
    should_raise(ArgumentError) do
      "cool" * -3
    end
    
    should_raise(ArgumentError) do
      "cool" * -3.14
    end
  end
  
  it "raises a RangeError when given integer is a Bignum" do
    should_raise(RangeError) do
      "cool" * 9999999999
    end
  end
  
  it "returns subclass instances" do
    (MyString.new("cool") * 0).class.should == MyString
    (MyString.new("cool") * 1).class.should == MyString
    (MyString.new("cool") * 2).class.should == MyString
  end
end

describe "String#+(string)" do
  it "returns a new string containing the given string concatenated to self" do
    ("" + "").should == ""
    ("" + "Hello").should == "Hello"
    ("Hello" + "").should == "Hello"
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end
  
  it "converts the argument to a string using to_str" do
    c = Object.new
    def c.to_str() "aaa" end
    
    ("a" + c).should == "aaaa"
  end
  
  it "doesn't return subclass instances" do
    (MyString.new("hello") + "").class.should == String
    (MyString.new("hello") + "foo").class.should == String
    (MyString.new("hello") + MyString.new("foo")).class.should == String
    (MyString.new("hello") + MyString.new("")).class.should == String
    (MyString.new("") + MyString.new("")).class.should == String
    ("hello" + MyString.new("foo")).class.should == String
    ("hello" + MyString.new("")).class.should == String
  end
end

describe "String#<<(string)" do
  it "concatenates the given argument to self and returns self" do
    str = 'hello '
    (str << 'world').equal?(str).should == true
    str.should == "hello world"
  end
  
  it "converts the given argument to a String using to_str" do
    obj = Object.new
    def obj.to_str() "world!" end
    
    a = 'hello ' << obj
    a.should == 'hello world!'
  end
  
  it "raises a TypeError if the given argument can't be converted to a String" do
    should_raise(TypeError) do
      a = 'hello ' << :world
    end

    should_raise(TypeError) do
      a = 'hello ' << Object.new
    end
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze

    should_raise(TypeError) { a << "" }
    should_raise(TypeError) { a << "test" }
  end
  
  it "works when given a subclass instance" do
    a = "hello"
    a << MyString.new(" world")
    a.should == "hello world"
  end
end

describe "String#<<(fixnum)" do
  it "converts the given Fixnum to a char before concatenating" do
    b = 'hello ' << 'world' << 33
    b.should == "hello world!"
    b << 0
    b.should == "hello world!\x00"
  end
  
  it "raises a TypeError when the given Fixnum is not between 0 and 255" do
    should_raise(TypeError) do
      "hello world" << 333
    end
  end

  it "doesn't call to_int on its argument" do
    x = Object.new
    x.should_not_receive(:to_int)
    
    should_raise(TypeError) { "" << x }
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze

    should_raise(TypeError) { a << 0 }
    should_raise(TypeError) { a << 33 }
  end
end

describe "String#<=>(other_string)" do
  it "compares individual characters based on their ascii value" do
    ascii_order = Array.new(256) { |x| x.chr }
    sort_order = ascii_order.sort
    sort_order.should == ascii_order
  end
  
  it "returns -1 when self is less than other" do
    ("this" <=> "those").should == -1
  end

  it "returns 0 when self is equal to other" do
    ("yep" <=> "yep").should == 0
  end

  it "returns 1 when self is greater than other" do
    ("yoddle" <=> "griddle").should == 1
  end
  
  it "considers string that comes lexicographically first to be less if strings have same size" do
    ("aba" <=> "abc").should == -1
    ("abc" <=> "aba").should == 1
  end

  it "doesn't consider shorter string to be less if longer string starts with shorter one" do
    ("abc" <=> "abcd").should == -1
    ("abcd" <=> "abc").should == 1
  end

  it "compares shorter string with corresponding number of first chars of longer string" do
    ("abx" <=> "abcd").should == 1
    ("abcd" <=> "abx").should == -1
  end
  
  it "ignores subclass differences" do
    a = "hello"
    b = MyString.new("hello")
    
    (a <=> b).should == 0
    (b <=> a).should == 0
  end
end

describe "String#<=>(obj)" do
  it "returns nil if its argument does not respond to to_str" do
    ("abc" <=> 1).should == nil
    ("abc" <=> :abc).should == nil
    ("abc" <=> Object.new).should == nil
  end
  
  it "returns nil if its argument does not respond to <=>" do
    obj = Object.new
    def obj.to_str() "" end
    
    ("abc" <=> obj).should == nil
  end
  
  it "compares its argument and self by calling <=> on obj and turning the result around" do
    obj = Object.new
    def obj.to_str() "" end
    def obj.<=>(arg) 1  end
    
    ("abc" <=> obj).should == -1
    ("xyz" <=> obj).should == -1
  end
end

describe "String#==(other_string)" do
  it "returns true if self <=> string returns 0" do
    ('hello' == 'hello').should == true
  end
  
  it "returns false if self <=> string does not return 0" do
    ("more" == "MORE").should == false
    ("less" == "greater").should == false
  end
  
  it "ignores subclass differences" do
    a = "hello"
    b = MyString.new("hello")
    
    (a == b).should == true
    (b == a).should == true
  end  
end

describe "String#==(obj)" do
  it "returns false if obj does not respond to to_str" do
    ('hello' == 5).should == false
    ('hello' == :hello).should == false
    ('hello' == Object.new).should == false
  end
  
  it "returns obj == self if obj responds to to_str" do
    obj = Object.new
    def obj.to_str() "world!" end
    def obj.==(other) true end
    
    ('hello' == obj).should == true
    ('world!' == obj).should == true 
  end
end

describe "String#=~(obj)" do
  it "behaves the same way as index() when given a regexp" do
    ("rudder" =~ /udder/).should == "rudder".index(/udder/)
    ("boat" =~ /[^fl]oat/).should == "boat".index(/[^fl]oat/)
    ("bean" =~ /bag/).should == "bean".index(/bag/)
    ("true" =~ /false/).should == "true".index(/false/)
  end

  it "raises a TypeError if a obj is a string" do
    should_raise(TypeError) { "some string" =~ "another string" }
    should_raise(TypeError) { "a" =~ MyString.new("b") }
  end
  
  it "invokes obj.=~ with self if obj is neither a string nor regexp" do
    str = "w00t"
    obj = Object.new

    obj.should_receive(:=~, :with => [str], :returning => true)
    (str =~ obj).should == true

    obj.should_receive(:=~, :with => [str], :returning => false)
    (str =~ obj).should == false
  end
end

describe "String#[idx]" do
  it "returns the character code of the character at fixnum" do
    "hello"[0].should == ?h
    "hello"[-1].should == ?o
  end
  
  it "returns nil if idx is outside of self" do
    "hello"[20].should == nil
    "hello"[-20].should == nil
    
    ""[0].should == nil
    ""[-1].should == nil
  end
  
  it "calls to_int on idx" do
    "hello"[0.5].should == ?h
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => 1)
    "hello"[obj].should == ?e
  end
end

describe "String#[idx, length]" do
  it "returns the substring starting at idx and the given length" do
    "hello there"[0,0].should == ""
    "hello there"[0,1].should == "h"
    "hello there"[0,3].should == "hel"
    "hello there"[0,6].should == "hello "
    "hello there"[0,9].should == "hello the"
    "hello there"[0,12].should == "hello there"

    "hello there"[1,0].should == ""
    "hello there"[1,1].should == "e"
    "hello there"[1,3].should == "ell"
    "hello there"[1,6].should == "ello t"
    "hello there"[1,9].should == "ello ther"
    "hello there"[1,12].should == "ello there"

    "hello there"[3,0].should == ""
    "hello there"[3,1].should == "l"
    "hello there"[3,3].should == "lo "
    "hello there"[3,6].should == "lo the"
    "hello there"[3,9].should == "lo there"

    "hello there"[4,0].should == ""
    "hello there"[4,3].should == "o t"
    "hello there"[4,6].should == "o ther"
    "hello there"[4,9].should == "o there"
    
    "foo"[2,1].should == "o"
    "foo"[3,0].should == ""
    "foo"[3,1].should == ""

    ""[0,0].should == ""
    ""[0,1].should == ""

    "x"[0,0].should == ""
    "x"[0,1].should == "x"
    "x"[1,0].should == ""
    "x"[1,1].should == ""

    "x"[-1,0].should == ""
    "x"[-1,1].should == "x"

    "hello there"[-3,2].should == "er"
  end
  
  it "returns nil if the offset falls outside of self" do
    "hello there"[20,3].should == nil
    "hello there"[-20,3].should == nil

    ""[1,0].should == nil
    ""[1,1].should == nil
    
    ""[-1,0].should == nil
    ""[-1,1].should == nil
    
    "x"[2,0].should == nil
    "x"[2,1].should == nil

    "x"[-2,0].should == nil
    "x"[-2,1].should == nil
  end
  
  it "returns nil if the length is negative" do
    "hello there"[4,-3].should == nil
    "hello there"[-4,-3].should == nil
  end
  
  it "calls to_int on idx and length" do
    "hello"[0.5, 1].should == "h"
    "hello"[0.5, 2.5].should == "he"
    "hello"[1, 2.5].should == "el"
    
    obj = Object.new
    obj.should_receive(:to_int, :count => 4, :returning => 2)

    "hello"[obj, 1].should == "l"
    "hello"[obj, obj].should == "ll"
    "hello"[0, obj].should == "he"
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s[0,0].class.should == MyString
    s[0,4].class.should == MyString
    s[1,4].class.should == MyString
  end
end

describe "String#[range]" do
  it "returns the substring given by the offsets of the range" do
    "hello there"[1..1].should == "e"
    "hello there"[1..3].should == "ell"
    "hello there"[1...3].should == "el"
    "hello there"[-4..-2].should == "her"
    "hello there"[-4...-2].should == "he"
    "hello there"[5..-1].should == " there"
    "hello there"[5...-1].should == " ther"
    
    ""[0..0].should == ""

    "x"[0..0].should == "x"
    "x"[0..1].should == "x"
    "x"[0...1].should == "x"
    "x"[0..-1].should == "x"
    
    "x"[1..1].should == ""
    "x"[1..-1].should == ""
  end
  
  it "returns nil if the beginning of the range falls outside of self" do
    "hello there"[12..-1].should == nil
    "hello there"[20..25].should == nil
    "hello there"[20..1].should == nil
    "hello there"[-20..1].should == nil
    "hello there"[-20..-1].should == nil

    ""[-1..-1].should == nil
    ""[-1...-1].should == nil
    ""[-1..0].should == nil
    ""[-1...0].should == nil
  end
  
  it "returns an empty string if range.begin is inside self and > real end" do
    "hello there"[1...1].should == ""
    "hello there"[4..2].should == ""
    "hello"[4..-4].should == ""
    "hello there"[-5..-6].should == ""
    "hello there"[-2..-4].should == ""
    "hello there"[-5..-6].should == ""
    "hello there"[-5..2].should == ""

    ""[0...0].should == ""
    ""[0..-1].should == ""
    ""[0...-1].should == ""
    
    "x"[0...0].should == ""
    "x"[0...-1].should == ""
    "x"[1...1].should == ""
    "x"[1...-1].should == ""
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s[0...0].class.should == MyString
    s[0..4].class.should == MyString
    s[1..4].class.should == MyString
  end
  
  it "calls to_int on range arguments" do
    from = Object.new
    to = Object.new

    # So we can construct a range out of them...
    def from.<=>(o) 0 end
    def to.<=>(o) 0 end

    def from.to_int() 1 end
    def to.to_int() -2 end
      
    "hello there"[from..to].should == "ello ther"
    "hello there"[from...to].should == "ello the"
  end
end

describe "String#[regexp]" do
  it "returns the matching portion of self" do
    "hello there"[/[aeiou](.)\1/].should == "ell"
    ""[//].should == ""
  end
  
  it "returns nil if there is no match" do
    "hello there"[/xyz/].should == nil
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s[//].class.should == MyString
    s[/../].class.should == MyString
  end
end

describe "String#[regexp, idx]" do
  it "returns the capture for idx" do
    "hello there"[/[aeiou](.)\1/, 0].should == "ell"
    "hello there"[/[aeiou](.)\1/, 1].should == "l"
    "hello there"[/[aeiou](.)\1/, -1].should == "l"

    "har"[/(.)(.)(.)/, 0].should == "har"
    "har"[/(.)(.)(.)/, 1].should == "h"
    "har"[/(.)(.)(.)/, 2].should == "a"
    "har"[/(.)(.)(.)/, 3].should == "r"
    "har"[/(.)(.)(.)/, -1].should == "r"
    "har"[/(.)(.)(.)/, -2].should == "a"
    "har"[/(.)(.)(.)/, -3].should == "h"
  end
  
  it "returns nil if there is no match" do
    "hello there"[/(what?)/, 1].should == nil
  end
  
  it "returns nil if there is no capture for idx" do
    "hello there"[/[aeiou](.)\1/, 2].should == nil
    # You can't refer to 0 using negative indices
    "hello there"[/[aeiou](.)\1/, -2].should == nil
  end
  
  it "calls to_int on idx" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 2)
      
    "har"[/(.)(.)(.)/, 1.5].should == "h"
    "har"[/(.)(.)(.)/, obj].should == "a"
  end
  
  it "returns subclass instances" do
    s = MyString.new("hello")
    s[/(.)(.)/, 0].class.should == MyString
    s[/(.)(.)/, 1].class.should == MyString
  end
end

describe "String#[other_string]" do
  it "returns the string if it occurs in self" do
    s = "lo"
    "hello there"[s].should == s
  end
  
  it "returns nil if there is no match" do
    "hello there"["bye"].should == nil
  end
  
  it "doesn't call to_str on its argument" do
    o = Object.new
    o.should_not_receive(:to_str)
      
    should_raise(TypeError) { "hello"[o] }
  end
  
  it "returns a subclass instance when given a subclass instance" do
    s = MyString.new("el")
    r = "hello"[s]
    r.should == "el"
    r.class.should == MyString
  end
end

describe "String#[idx] = char" do
  it "sets the code of the character at idx to char modulo 256" do
    a = "hello"
    a[0] = ?b
    a.should == "bello"
    a[-1] = ?a
    a.should == "bella"
    a[-1] = 0
    a.should == "bell\x00"
    a[-5] = 0
    a.should == "\x00ell\x00"
    
    a = "x"
    a[0] = ?y
    a.should == "y"
    a[-1] = ?z
    a.should == "z"
    
    a[0] = 255
    a[0].should == 255
    a[0] = 256
    a[0].should == 0
    a[0] = 256 * 3 + 42
    a[0].should == 42
    a[0] = -214
    a[0].should == 42
  end
 
  it "raises an IndexError without changing self if idx is outside of self" do
    a = "hello"
    
    should_raise(IndexError) { a[20] = ?a }
    a.should == "hello"
    
    should_raise(IndexError) { a[-20] = ?a }
    a.should == "hello"
    
    should_raise(IndexError) { ""[0] = ?a }
    should_raise(IndexError) { ""[-1] = ?a }
  end
  
  # Broken in MRI 1.8.4
  it "calls to_int on idx" do
    str = "hello"
    str[0.5] = ?c
    str.should == "cello"
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => -1)
    str[obj] = ?y
    str.should == "celly"
  end
  
  it "doesn't call to_int on char" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    should_raise(TypeError) { "hi"[0] = obj }
  end
  
  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) { a[0] = ?b }
  end
end

describe "String#[idx] = other_str" do
  it "replaces the char at idx with other_str" do
    a = "hello"
    a[0] = "bam"
    a.should == "bamello"
    a[-2] = ""
    a.should == "bamelo"
  end

  it "raises an IndexError  without changing self if idx is outside of self" do
    str = "hello"

    should_raise(IndexError) { str[20] = "bam" }    
    str.should == "hello"
    
    should_raise(IndexError) { str[-20] = "bam" }
    str.should == "hello"

    should_raise(IndexError) { ""[0] = "bam" }
    should_raise(IndexError) { ""[-1] = "bam" }
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) { a[0] = "bam" }
  end
  
  # Broken in MRI 1.8.4
  it "calls to_int on idx" do
    str = "hello"
    str[0.5] = "hi "
    str.should == "hi ello"
    
    obj = Object.new
    obj.should_receive(:to_int, :returning => -1)
    str[obj] = "!"
    str.should == "hi ell!"
  end
  
  it "tries to convert other_str to a String using to_str" do
    other_str = Object.new
    def other_str.to_str() "-test-" end
    
    a = "abc"
    a[1] = other_str
    a.should == "a-test-c"
  end
  
  it "raises a TypeError if other_str can't be converted to a String" do
    should_raise(TypeError) { "test"[1] = :test }
    should_raise(TypeError) { "test"[1] = Object.new }
    should_raise(TypeError) { "test"[1] = nil }
  end
end

# TODO: Add more String#[]= specs

describe "String#capitalize" do
  it "returns a copy of self with the first character converted to uppercase and the remainder to lowercase" do
    "hello".capitalize.should == "Hello"
    "HELLO".capitalize.should == "Hello"
    "123ABC".capitalize.should == "123abc"
  end
end

describe "String#capitalize!" do
  it "capitalizes self in place" do
    a = "hello"
    a.capitalize!.should == "Hello"
    a.should == "Hello"
  end
  
  it "returns nil when no changes are made" do
    a = "Hello"
    a.capitalize!.should == nil
    a.should == "Hello"
    
    "".capitalize!.should == nil
  end

  it "raises a TypeError when self is frozen" do
    ["", "Hello", "hello"].each do |a|
      a.freeze
      should_raise(TypeError) { a.capitalize! }
    end
  end
end

describe "String#casecmp" do
  it "is a case-insensitive version of String#<=>" do
    "abcdef".casecmp("abcde").should == 1
    "aBcDeF".casecmp("abcdef").should == 0
    "abcdef".casecmp("abcdefg").should == -1
    "abcdef".casecmp("ABCDEF").should == 0
  end
  
  it "ignores subclass differences" do
    str = "abcdef"
    my_str = MyString.new(str)
    
    str.casecmp(my_str).should == 0
    my_str.casecmp(str).should == 0
    my_str.casecmp(my_str).should == 0
  end
end

describe "String#center(integer, padstr)" do
  it "returns a new string of length integer with self centered and padded with padstr" do
    "one".center(9, '.').should       == "...one..."
    "hello".center(20, '123').should  == "1231231hello12312312"
    "middle".center(13, '-').should   == "---middle----"
  end
  
  it "pads with whitespace if no padstr is given" do
    "two".center(5).should    == " two "
    "hello".center(20).should == "       hello        "
  end
  
  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "123" end
    
    "hello".center(20, padstr).should == "1231231hello12312312"
  end
  
  it "raises an ArgumentError when padstr can't be converted to a string" do
    should_raise(ArgumentError) do
      "hello".chomp(20, ?o)
    end
    
    should_raise(ArgumentError) do
      "hello".chomp(20, :llo)
    end
    
    should_raise(ArgumentError) do
      "hello".chomp(20, Object.new)
    end
  end
  
  it "returns self if it's longer than or as long as integer chars" do
    "hello".center(4).should == "hello"
    "this".center(3).should == "this"
    "radiology".center(8, '-').should == "radiology"
  end
  
  it "raises an ArgumentError if padstr length is 0" do
    should_raise(ArgumentError) do
      "hello".center(10, "")
    end
  end
end

describe "String#chomp(separator)" do
  it "returns a new string with the given record separator removed" do
    "hello".chomp("llo").should == "he"
  end

  it "removes carriage return chars multiple times when seperator is an empty string" do
    "hello".chomp("").should == "hello"
    "hello\n".chomp("").should == "hello"
    "hello\r\n".chomp("").should == "hello"
    "hello\r\n\r\n\n\n\r\n".chomp("").should == "hello"
  end
  
  it "removes carriage return chars when separator is \\n" do
    "hello".chomp("\n").should == "hello"
    "hello\n".chomp("\n").should == "hello"
    "hello\r\n".chomp("\n").should == "hello"
    "hello\n\r".chomp("\n").should == "hello\n"
    "hello\r".chomp("\n").should == "hello"
    "hello \n there".chomp("\n").should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp("\n").should == "hello\r\n\r\n\n\n"
  end
  
  it "removes carriage return chars when separator is $/ (default)" do
    "hello".chomp.should == "hello"
    "hello\n".chomp.should == "hello"
    "hello\r\n".chomp.should == "hello"
    "hello\n\r".chomp.should == "hello\n"
    "hello\r".chomp.should == "hello"
    "hello \n there".chomp.should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp.should == "hello\r\n\r\n\n\n"
  end
  
  it "returns self if the separator is nil" do
    "hello\n\n".chomp(nil).should == "hello\n\n"
  end
  
  it "tries to convert separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() "llo" end
    
    "hello".chomp(separator).should == "he"
  end
  
  it "raises a TypeError if separator can't be converted to a string" do
    should_raise(TypeError) do
      "hello".chomp(?o)
    end
    
    should_raise(TypeError) do
      "hello".chomp(:llo)
    end
    
    should_raise(TypeError) do
      "hello".chomp(Object.new)
    end
  end
end

describe "String#chomp!(seperator)" do
  it "modifies self in place" do
    s = "one\n"
    s.chomp!
    s.should == "one"
    
    t = "two\r\n"
    t.chomp!
    t.should == "two"
    
    u = "three\r"
    u.chomp!
    u.should == "three"
    
    w = "four\n\r"
    w.chomp!
    w.should == "four\n"
  end
  
  it "returns nil if no modifications were made" do
     v = "four"
     v.chomp!.should == nil
     v.should == "four"
    
    "".chomp!.should == nil
    "line".chomp!.should == nil
  end

  it "raises a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "string\n\r"
      a.freeze
      a.chomp!
    end
  end
end

describe "String#concat(String)" do
  it "is an alias of String#<<" do
    (a = "abc") << "xyz"
    (b = "abc").concat("xyz")
    a.should == b
  end
end

describe "String#delete(*String)" do
  it "returns a new string with the chars from other_string removed" do
    "hello".delete("lo").should == "he"
  end
  
  it "deletes only the intersection of characters in other_strings" do
    "hello".delete("l", "lo").should == "heo"
  end

  it "negates strings starting with ^" do
    "hello".delete("aeiou", "^e").should == "hell"
  end

  it "deletes all chars in a sequence" do
    "hello".delete("ej-m").should == "ho"
  end

  it "tries to convert each other_string given to a string using to_str" do
    other_string = Object.new
    def other_string.to_str() "lo" end
    
    other_string2 = Object.new
    def other_string2.to_str() "o" end
    
    "hello world".delete(other_string, other_string2).should == "hell wrld"
  end
  
  it "raises a TypeError when one other_string can't be converted to a string" do
    should_raise(TypeError) do
      "hello world".delete(?o)
    end

    should_raise(TypeError) do
      "hello world".delete(:o)
    end

    should_raise(TypeError) do
      "hello world".delete(Object.new)
    end
  end
end

describe "String#delete!([other_strings])" do
  it "modifies self in place" do
    a = "hello"
    a.delete!("aeiou", "^e").should == "hell"
    a.should == "hell"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.delete!("z").should == nil
    a.should == "hello"
  end

  it "raises a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.delete!("aeiou", "^e")
    end
  end
end

describe "String#downcase" do
  it "returns a copy of self with all uppercase letters downcase'd" do
    "hELLO".downcase.should == "hello"
    "hello".downcase.should == "hello"
  end
  
  it "is locale insensitive (only replacing A-Z)" do
    "ÄÖÜ".downcase.should == "ÄÖÜ"
  end
end

describe "String#downcase!" do
  it "modifies self in place" do
    a = "HeLlO"
    a.downcase!.should == "hello"
    a.should == "hello"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.downcase!.should == nil
    a.should == "hello"
  end

  it "raises a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "HeLlO"
      a.freeze
      a.downcase!
    end
  end
end

describe "String#dump" do
  # Older versions of MRI wrongly print \b as \010
  it "produces a version of self with all nonprinting charaters replaced by \\nnn notation" do
    ("\000".."A").to_a.to_s.dump.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end
  
  it "ignores the $KCODE setting" do
    old_kcode = $KCODE

    begin
      $KCODE = "NONE"
      "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""

      $KCODE = "UTF-8"
      "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""
    ensure
      $KCODE = old_kcode
    end
  end
end

describe "String#each_byte" do
  it "passes each byte in self to the given block" do
    a = []
    "hello".each_byte { |c| a << c }
    a.should == [104, 101, 108, 108, 111]
  end
  
  it "returns self" do
    ("hello".each_byte {}).should == "hello"
  end
end

describe "String#each(separator)" do
  it "splits self using the supplied record separator (default: $/) and pass each substring to the block" do
    a = []
    "one\ntwo\r\nthree".each { |s| a << s }
    a.should == ["one\n", "two\r\n", "three"]
    
    b = []
    "hello\nworld".each('l') { |s| b << s }
    b.should == [ "hel", "l", "o\nworl", "d" ]
    
    c = []
    "hello\n\n\nworld".each { |s| c << s }
    c.should == ["hello\n", "\n", "\n", "world"]
  end
  
  it "taints substrings that are passed to the block if self is tainted" do
    a = "one\ntwo\r\nthree"
    b = []
    a.taint
    a.each { |s| b << s.tainted? }
    b.should == [true, true, true]
  end
  
  it "passes self as a whole to the block if the separator is nil" do
    a = []
    "one\ntwo\r\nthree".each(nil) { |s| a << s }
    a.should == ["one\ntwo\r\nthree"]
  end
  
  it "appends multiple successive newlines together when the separotar is an empty string" do
    a = []
    "hello\n\n\nworld".each('') { |s| a << s }
    a.should == ["hello\n\n\n", "world"]
  end
  
  it "returns self" do
    ("hello\nworld".each {}).should == "hello\nworld"
  end

  it "tries to convert each separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() 'l' end
    
    a = []
    "hello\nworld".each(separator) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
  end
  
  it "raises a TypeError when the separator can't be converted to a string" do
    should_raise(TypeError) do
      "hello world".each(?o)
    end

    should_raise(TypeError) do
      "hello world".each(:o)
    end

    should_raise(TypeError) do
      "hello world".each(Object.new)
    end
  end
end

describe "String#each_line(separator)" do
  it "is an alias of String#each" do
    a = []
    "one\ntwo\r\nthree".each { |s| a << s }

    b = []
    "one\ntwo\r\nthree".each_line { |s| b << s }

    a.should == b
  end
end

describe "String#empty?" do
  it "returns true if the string has a length of zero" do
    "hello".empty?.should == false
    " ".empty?.should == false
    "".empty?.should == true
  end
end

describe "String#eql?" do
  it "returns true if two strings have the same length and content" do
    "hello".eql?("hello").should == true
    "hello".eql?("hell").should == false
    "1".eql?(1).should == false
  end
end

describe "String#gsub(pattern, replacement)" do
  it "returns a copy of self with all occurences of pattern replaced with replacement" do
    "hello".gsub(/[aeiou]/, '*').should == "h*ll*"
  end
  
  it "doesn't interpret regexp metacharacters if pattern is a string" do
    "12345".gsub('\d', 'a').should == "12345"
    '\d'.gsub('\d', 'a').should == "a"
  end
  
  it "interpolates successive groups in the match with \\1, \\2 and so on" do
    "hello".gsub(/([aeiou])/, '<\1>').should == "h<e>ll<o>"
  end
  
  it "inherits the tainting in the original string or the replacement" do
    a = "hello"
    a.taint
    a.gsub(/./, 'a').tainted?.should == true
    
    b = "hello"
    c = 'a'
    c.taint
    b.gsub(/./, c).tainted?.should == true
  end
  
  it "tries to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".gsub(/hello/, replacement).should == "hello_replacement"
  end
  
  it "raises a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) do
      "hello".gsub(/[aeiou]/, :woot)
    end

    should_raise(TypeError) do
      "hello".gsub(/[aeiou]/, 5)
    end
  end
end

describe "String#gsub(pattern) { block }" do
  it "returns a copy of self with all occurences of pattern replaced with the block's return value" do
    "hello".gsub(/./) { |s| s[0].to_s + ' ' }.should == "104 101 108 108 111 "
  end
  
  it "allows the use of variables such as $1, $2, $`, $& and $' in the block" do
    "hello".gsub(/([aeiou])/) { "<#$1>" }.should == "h<e>ll<o>"
    "hello".gsub(/([aeiou])/) { "<#$&>" }.should == "h<e>ll<o>"
  end
  
  it "converts the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".gsub(/hello/) { replacement }.should == "hello_replacement"
  end
  
# TODO: This should raise a RuntimeError, but does not
#  it "raises a RuntimeError" do
#    str = "a" * 0x20
#    str.gsub(/\z/) {
#      dest = nil
#      ObjectSpace.each_object(String) {|o|
#         dest = o if o.length == 0x20+30
#      }
#      dest
#    }
#  end
end

describe "String#gsub!(pattern, replacement)" do
  it "modifies self in place" do
    a = "hello"
    a.gsub!(/[aeiou]/, '*').should == "h*ll*"
    a.should == "h*ll*"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.gsub!(/z/, '*').should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.gsub!(/[aeiou]/, '*')
    end
  end
end

describe "String#hash" do
  it "returns a hash based on a string's length and content" do
    "abc".hash.should == "abc".hash
    "abc".hash.should_not == "cba".hash
  end
end

describe "String#hex" do
  it "treats leading characters of self as a string of hex digits" do
    "0a".hex.should == 10
    "0x".hex.should == 0
  end
  
  it "takes an optional sign" do
    "-1234".hex.should == -4660
    "+1234".hex.should == 4660
  end
  
  it "takes an optional 0x" do
    "0x0a".hex.should == 10
  end
  
  it "returns 0 on error" do
    "wombat".hex.should == 0
  end
end

describe "String#include?(other)" do
  it "returns true if self contains other" do
    "hello".include?("lo").should == true
    "hello".include?("ol").should == false
  end
  
  it "tries to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "lo" end
    
    "hello".include?(other).should == true
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) do
      "hello".include?(:lo)
    end
    
    should_raise(TypeError) do
      "hello".include?(Object.new)
    end
  end
end

describe "String#include?(fixnum)" do
  it "returns true if self contains the given char" do
    "hello".include?(?h).should == true
    "hello".include?(?z).should == false
  end
end

describe "String#index(fixnum [, offset])" do
  it "returns the index of the first occurence of the given character" do
    "hello".index(?e).should == 1
  end
  
  it "starts the search at the given offset" do
    "hello".index(?o, -2).should == 4
  end
  
  it "returns nil if no occurence is found" do
    "hello".index(?z).should == nil
    "hello".index(?e, -2).should == nil
  end
end

describe "String#index(substring [, offset])" do
  it "returns the index of the first occurence of the given substring" do
    "hello".index('e').should == 1
    "hello".index('lo').should == 3
  end
  
  it "starts the search at the given offset" do
    "hello".index('o', -3).should == 4
  end
  
  it "returns nil if no occurence is found" do
    "hello".index('z').should == nil
    "hello".index('e', -2).should == nil
    "a-b-c".split("-").should == ["a", "b", "c"]
  end
  
  it "raises a TypeError if no string was given" do
    should_raise(TypeError) do
      "hello".index(:sym)
    end
    
    should_raise(TypeError) do
      "hello".index(Object.new)
    end
    "a   b c  ".split(/\s+/).should == ["a", "b", "c"]
  end
end

describe "String#index(regexp [, offset])" do
  it "returns the index of the first match with the given regexp" do
    "hello".index(/[aeiou]/).should == 1
  end
  
  it "starts the search at the given offset" do
    "hello".index(/[aeiou]/, -3).should == 4
  end
  
  it "returns nil if no occurence is found" do
    "hello".index(/z/).should == nil
    "hello".index(/e/, -2).should == nil
  end
end

describe "String#insert(index, other)" do
  it "inserts other before the character at the given index" do
    "abcd".insert(0, 'X').should == "Xabcd"
    "abcd".insert(3, 'X').should == "abcXd"
    "abcd".insert(4, 'X').should == "abcdX"
  end
  
  it "modifies self in place" do
    a = "abcd"
    a.insert(4, 'X').should == "abcdX"
    a.should == "abcdX"
  end
  
  it "inserts after the given character on an negative count" do
    "abcd".insert(-3, 'X').should == "abXcd"
    "abcd".insert(-1, 'X').should == "abcdX"
  end
  
  it "raises an IndexError if the index is out of string" do
    should_raise(IndexError) do
      "abcd".insert(5, 'X')
    end
    
    should_raise(IndexError) do
      "abcd".insert(-6, 'X')
    end
  end
  
  it "converts other to a string using to_str" do
    other = Object.new
    def other.to_str() "XYZ" end
    
    "abcd".insert(-3, other).should == "abXYZcd"
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) do
      "abcd".insert(-6, :sym)
    end
    
    should_raise(TypeError) do
      "abcd".insert(-6, 12)
    end
    
    should_raise(TypeError) do
      "abcd".insert(-6, Object.new)
    end
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcd"
      a.freeze
      a.insert(4, 'X')
    end
  end
end

describe "String#inspect" do
  # Older versions of MRI wrongly print \b as \010
  it "produces a version of self with all nonprinting charaters replaced by \\nnn notation" do
    ("\000".."A").to_a.to_s.inspect.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end
  
  it "produces different output based on $KCODE" do
    old_kcode = $KCODE

    begin
      $KCODE = "NONE"
      "äöü".inspect.should == "\"\\303\\244\\303\\266\\303\\274\""

      $KCODE = "UTF-8"
      "äöü".inspect.should == "\"äöü\""
    ensure
      $KCODE = old_kcode
    end
  end
end

describe "String#length" do
  it "returns the length of self" do
    "".length.should == 0
    "one".length.should == 3
    "two".length.should == 3
    "three".length.should == 5
    "four".length.should == 4
  end
end

describe "String#ljust(integer, padstr)" do
  it "returns a new integer with length of integer and self left justified and padded with padstr (default: whitespace)" do
    "hello".ljust(20).should         == "hello               "
    "hello".ljust(20, '1234').should == "hello123412341234123"
  end

  it "returns self if self is longer than integer" do
    "hello".ljust(5).should == "hello"
    "hello".ljust(1).should == "hello"
  end
  
  it "raises an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".ljust(10, '')
    end
  end
  
  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "1234" end
    
    "hello".ljust(20, padstr).should == "hello123412341234123"
  end
  
  it "raises a TypeError when padstr can't be converted" do
    should_raise(TypeError) do
      "hello".ljust(20, :sym)
    end
    
    should_raise(TypeError) do
      "hello".ljust(20, ?c)
    end
    
    should_raise(TypeError) do
      "hello".ljust(20, Object.new)
    end
  end
end

describe "String#lstrip" do
  it "returns a copy of self with leading whitespace removed" do
   "  hello  ".lstrip.should == "hello  "
   "  hello world  ".lstrip.should == "hello world  "
   "\n\r\t\n\rhello world  ".lstrip.should == "hello world  "
   "hello".lstrip.should == "hello"
  end
end

describe "String#lstrip!" do
  it "modifies self in place" do
    a = "  hello  "
    a.lstrip!.should == "hello  "
    a.should == "hello  "
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.lstrip!.should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.lstrip!
    end
  end
end

describe "String#match(pattern)" do
  it "matches the pattern against self" do
    'hello'.match(/(.)\1/)[0].should == 'll'
  end

  it "converts pattern to a regexp if it isn't already one" do
    'hello'.match('(.)\1')[0].should == 'll'
  end
  
  it "returns nil if there's no match" do
    'hello'.match('xx').should == nil
  end
  
  it "raises a TypeError if pattern is not a regexp or a string" do
    should_raise TypeError do
      'hello'.match(10)
    end
    
    should_raise TypeError do
      'hello'.match(:ell)
    end
  end
end

describe "String#next" do
  it "is an alias of String#succ" do
    "abcd".succ.should == "abcd".next
    "98".succ.should == "98".next
    "ZZZ9999".succ.should == "ZZZ9999".next
  end
end

describe "String#next!" do
  it "is an alias of String#succ!" do
    a = "THX1138"
    b = "THX1138"
    a.succ!.should == b.next!
    a.should == b
  end
end

describe "String#oct" do
  it "treats leading characters of self as octal digits" do
    "123".oct.should == 83
    "0377bad".oct.should == 255
  end
  
  it "takes an optional sign" do
    "-377".oct.should == -255
    "+377".oct.should == 255
  end
  
  it "returns 0 if the conversion fails" do
    "bad".oct.should == 0
  end
end

describe "String#replace(other)" do
  it "replaces the content of self with other" do
    a = "some string"
    a.replace("another string")
    a.should == "another string"
  end
  
  it "replaces the taintedness of self with that of other" do
    a = "an untainted string"
    b = "a tainted string"
    b.taint
    a.replace(b)
    a.tainted?.should == true
  end
  
  it "tries to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "an object converted to a string" end
    
    "hello".replace(other).should == "an object converted to a string"
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) do
      "hello".replace(123)
    end
    
    should_raise(TypeError) do
      "hello".replace(:test)
    end
    
    should_raise(TypeError) do
      "hello".replace(Object.new)
    end
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.replace("world")
    end
  end
end

describe "String#reverse" do
  it "returns a new string with the characters of self in reverse order" do
    "stressed".reverse.should == "desserts"
    "m".reverse.should == "m"
    "".reverse.should == ""
  end
end

describe "String#reverse!" do
  it "reverses self in place" do
    a = "stressed"
    a.reverse!.should == "desserts"
    a.should == "desserts"
    
    b = ""
    b.reverse!.should == ""
    b.should == ""
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.reverse!
    end
  end
end

describe "String#rindex(obj [, start_offset])" do
  it "raises a TypeError if obj isn't a String, Fixnum or Regexp" do
    should_raise(TypeError) { "hello".rindex(:sym) }    
    should_raise(TypeError) { "hello".rindex(Object.new) }
  end

  it "calls neither to_str nor to_int on obj" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    obj.should_not_receive(:to_str)
    
    should_raise(TypeError) { "hello".rindex(obj) }
  end
end

describe "String#rindex(fixnum [, start_offset])" do
  it "returns the index of the last occurrence of the given character" do
    "hello".rindex(?e).should == 1
    "hello".rindex(?l).should == 3
  end
  
  it "starts the search at the given offset" do
    "blablabla".rindex(?b, 0).should == 0
    "blablabla".rindex(?b, 1).should == 0
    "blablabla".rindex(?b, 2).should == 0
    "blablabla".rindex(?b, 3).should == 3
    "blablabla".rindex(?b, 4).should == 3
    "blablabla".rindex(?b, 5).should == 3
    "blablabla".rindex(?b, 6).should == 6
    "blablabla".rindex(?b, 7).should == 6
    "blablabla".rindex(?b, 8).should == 6
    "blablabla".rindex(?b, 9).should == 6
    "blablabla".rindex(?b, 10).should == 6

    "blablabla".rindex(?a, 2).should == 2
    "blablabla".rindex(?a, 3).should == 2
    "blablabla".rindex(?a, 4).should == 2
    "blablabla".rindex(?a, 5).should == 5
    "blablabla".rindex(?a, 6).should == 5
    "blablabla".rindex(?a, 7).should == 5
    "blablabla".rindex(?a, 8).should == 8
    "blablabla".rindex(?a, 9).should == 8
    "blablabla".rindex(?a, 10).should == 8
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    [?a, ?b].each do |needle|
      (-str.length .. -1).each do |offset|
        str.rindex(needle, offset).should ==
        str.rindex(needle, offset + str.length)
      end
    end
  end
  
  it "returns nil if the character isn't found" do
    "hello".rindex(0).should == nil
    
    "hello".rindex(?H).should == nil
    "hello".rindex(?z).should == nil
    "hello".rindex(?o, 2).should == nil
    
    "blablabla".rindex(?a, 0).should == nil
    "blablabla".rindex(?a, 1).should == nil
    
    "blablabla".rindex(?a, -8).should == nil
    "blablabla".rindex(?a, -9).should == nil
    
    "blablabla".rindex(?b, -10).should == nil
    "blablabla".rindex(?b, -20).should == nil
  end
  
  it "calls to_int on start_offset" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    
    "str".rindex(?s, obj).should == 0
  end
end

describe "String#rindex(substring [, start_offset])" do
  it "behaves the same as String#rindex(char) for one-character strings" do
    ["blablabla", "hello cruel world...!"].each do |str|
      str.split("").uniq.each do |str|
        chr = str[0]
        str.rindex(str).should == str.rindex(chr)
        
        0.upto(str.size + 1) do |start|
          str.rindex(str, start).should == str.rindex(chr, start)
        end
        
        (-str.size - 1).upto(-1) do |start|
          str.rindex(str, start).should == str.rindex(chr, start)
        end
      end
    end
  end
  
  it "returns the index of the last occurrence of the given substring" do
    "blablabla".rindex("").should == 9
    "blablabla".rindex("a").should == 8
    "blablabla".rindex("la").should == 7
    "blablabla".rindex("bla").should == 6
    "blablabla".rindex("abla").should == 5
    "blablabla".rindex("labla").should == 4
    "blablabla".rindex("blabla").should == 3
    "blablabla".rindex("ablabla").should == 2
    "blablabla".rindex("lablabla").should == 1
    "blablabla".rindex("blablabla").should == 0
    
    "blablabla".rindex("l").should == 7
    "blablabla".rindex("bl").should == 6
    "blablabla".rindex("abl").should == 5
    "blablabla".rindex("labl").should == 4
    "blablabla".rindex("blabl").should == 3
    "blablabla".rindex("ablabl").should == 2
    "blablabla".rindex("lablabl").should == 1
    "blablabla".rindex("blablabl").should == 0

    "blablabla".rindex("b").should == 6
    "blablabla".rindex("ab").should == 5
    "blablabla".rindex("lab").should == 4
    "blablabla".rindex("blab").should == 3
    "blablabla".rindex("ablab").should == 2
    "blablabla".rindex("lablab").should == 1
    "blablabla".rindex("blablab").should == 0
    
    "blablabla".rindex(/BLA/i).should == 6
  end  
  
  it "starts the search at the given offset" do
    "blablabla".rindex("bl", 0).should == 0
    "blablabla".rindex("bl", 1).should == 0
    "blablabla".rindex("bl", 2).should == 0
    "blablabla".rindex("bl", 3).should == 3

    "blablabla".rindex("bla", 0).should == 0
    "blablabla".rindex("bla", 1).should == 0
    "blablabla".rindex("bla", 2).should == 0
    "blablabla".rindex("bla", 3).should == 3

    "blablabla".rindex("blab", 0).should == 0
    "blablabla".rindex("blab", 1).should == 0
    "blablabla".rindex("blab", 2).should == 0
    "blablabla".rindex("blab", 3).should == 3
    "blablabla".rindex("blab", 6).should == 3
    "blablablax".rindex("blab", 6).should == 3

    "blablabla".rindex("la", 1).should == 1
    "blablabla".rindex("la", 2).should == 1
    "blablabla".rindex("la", 3).should == 1
    "blablabla".rindex("la", 4).should == 4

    "blablabla".rindex("lab", 1).should == 1
    "blablabla".rindex("lab", 2).should == 1
    "blablabla".rindex("lab", 3).should == 1
    "blablabla".rindex("lab", 4).should == 4

    "blablabla".rindex("ab", 2).should == 2
    "blablabla".rindex("ab", 3).should == 2
    "blablabla".rindex("ab", 4).should == 2
    "blablabla".rindex("ab", 5).should == 5
    
    "blablabla".rindex("", 0).should == 0
    "blablabla".rindex("", 1).should == 1
    "blablabla".rindex("", 2).should == 2
    "blablabla".rindex("", 7).should == 7
    "blablabla".rindex("", 8).should == 8
    "blablabla".rindex("", 9).should == 9
    "blablabla".rindex("", 10).should == 9
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    ["bl", "bla", "blab", "la", "lab", "ab", ""].each do |needle|
      (-str.length .. -1).each do |offset|
        str.rindex(needle, offset).should ==
        str.rindex(needle, offset + str.length)
      end
    end
  end

  it "returns nil if the substring isn't found" do
    "blablabla".rindex("B").should == nil
    "blablabla".rindex("z").should == nil
    "blablabla".rindex("BLA").should == nil
    "blablabla".rindex("blablablabla").should == nil
        
    "hello".rindex("lo", 0).should == nil
    "hello".rindex("lo", 1).should == nil
    "hello".rindex("lo", 2).should == nil

    "hello".rindex("llo", 0).should == nil
    "hello".rindex("llo", 1).should == nil

    "hello".rindex("el", 0).should == nil
    "hello".rindex("ello", 0).should == nil
    
    "hello".rindex("", -6).should == nil
    "hello".rindex("", -7).should == nil

    "hello".rindex("h", -6).should == nil
  end
  
  it "calls to_int on start_offset" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    
    "str".rindex("st", obj).should == 0
  end
end

describe "String#rindex(regexp [, start_offset])" do
  it "behaves the same as String#rindex(string) for escaped string regexps" do
    ["blablabla", "hello cruel world...!"].each do |str|
      ["", "b", "bla", "lab", "o c", "d."].each do |needle|
        regexp = Regexp.new(Regexp.escape(needle))
        str.rindex(regexp).should == str.rindex(needle)
        
        0.upto(str.size + 1) do |start|
          str.rindex(regexp, start).should == str.rindex(needle, start)
        end
        
        (-str.size - 1).upto(-1) do |start|
          str.rindex(regexp, start).should == str.rindex(needle, start)
        end
      end
    end
  end
  
  it "returns the index of the first match from the end of string of regexp" do
    "blablabla".rindex(/.{0}/).should == 9
    "blablabla".rindex(/.{1}/).should == 8
    "blablabla".rindex(/.{2}/).should == 7
    "blablabla".rindex(/.{6}/).should == 3
    "blablabla".rindex(/.{9}/).should == 0

    "blablabla".rindex(/.*/).should == 9
    "blablabla".rindex(/.+/).should == 8

    "blablabla".rindex(/bla|a/).should == 8
    
    "blablabla".rindex(/\A/).should == 0
    "blablabla".rindex(/\Z/).should == 9
    "blablabla".rindex(/\z/).should == 9
    "blablabla\n".rindex(/\Z/).should == 10
    "blablabla\n".rindex(/\z/).should == 10

    "blablabla".rindex(/^/).should == 0
    "\nblablabla".rindex(/^/).should == 1
    "b\nlablabla".rindex(/^/).should == 2
    "blablabla".rindex(/$/).should == 9
    
    "blablabla".rindex(/.l./).should == 6
  end
  
  it "starts the search at the given offset" do
    "blablabla".rindex(/.{0}/, 5).should == 5
    "blablabla".rindex(/.{1}/, 5).should == 5
    "blablabla".rindex(/.{2}/, 5).should == 5
    "blablabla".rindex(/.{3}/, 5).should == 5
    "blablabla".rindex(/.{4}/, 5).should == 5

    "blablabla".rindex(/.{0}/, 3).should == 3
    "blablabla".rindex(/.{1}/, 3).should == 3
    "blablabla".rindex(/.{2}/, 3).should == 3
    "blablabla".rindex(/.{5}/, 3).should == 3
    "blablabla".rindex(/.{6}/, 3).should == 3

    "blablabla".rindex(/.l./, 0).should == 0
    "blablabla".rindex(/.l./, 1).should == 0
    "blablabla".rindex(/.l./, 2).should == 0
    "blablabla".rindex(/.l./, 3).should == 3
    
    "blablablax".rindex(/.x/, 10).should == 8
    "blablablax".rindex(/.x/, 9).should == 8
    "blablablax".rindex(/.x/, 8).should == 8

    "blablablax".rindex(/..x/, 10).should == 7
    "blablablax".rindex(/..x/, 9).should == 7
    "blablablax".rindex(/..x/, 8).should == 7
    "blablablax".rindex(/..x/, 7).should == 7
    
    "blablabla\n".rindex(/\Z/, 9).should == 9
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    ["bl", "bla", "blab", "la", "lab", "ab", ""].each do |needle|
      (-str.length .. -1).each do |offset|
        str.rindex(needle, offset).should ==
        str.rindex(needle, offset + str.length)
      end
    end
  end

  it "returns nil if the substring isn't found" do
    "blablabla".rindex(/.{10}/).should == nil
    "blablablax".rindex(/.x/, 7).should == nil
    "blablablax".rindex(/..x/, 6).should == nil
    
    "blablabla".rindex(/\Z/, 5).should == nil
    "blablabla".rindex(/\z/, 5).should == nil
    "blablabla\n".rindex(/\z/, 9).should == nil
  end

  it "supports \\G which matches at the given start offset" do
    "helloYOU.".rindex(/YOU\G/, 8).should == 5
    "helloYOU.".rindex(/YOU\G/).should == nil

    idx = "helloYOUall!".index("YOU")
    re = /YOU.+\G.+/
    # The # marks where \G will match.
    [
      ["helloYOU#all.", nil],
      ["helloYOUa#ll.", idx],
      ["helloYOUal#l.", idx],
      ["helloYOUall#.", idx],
      ["helloYOUall.#", nil]
    ].each do |spec, res|
      start = spec.index("#")
      str = spec.delete("#")

      str.rindex(re, start).should == res
    end
  end
  
  it "calls to_int on start_offset" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    
    "str".rindex(/../, obj).should == 1
  end
end

describe "String#rjust(integer, padstr)" do
  it "returns a new integer with length of integer and self right justified and padded with padstr (default: whitespace)" do
    "hello".rjust(20).should         == "               hello"
    "hello".rjust(20, '1234').should == "123412341234123hello"
  end

  it "returns self if self is longer than integer" do
    "hello".rjust(5).should == "hello"
    "hello".rjust(1).should == "hello"
  end
  
  it "raises an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".rjust(10, '')
    end
  end

  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "1234" end
    
    "hello".rjust(20, padstr).should == "123412341234123hello"
  end
  
  it "raises a TypeError when padstr can't be converted" do
    should_raise(TypeError) do
      "hello".rjust(20, :sym)
    end
    
    should_raise(TypeError) do
      "hello".rjust(20, ?c)
    end
    
    should_raise(TypeError) do
      "hello".rjust(20, Object.new)
    end
  end
end

describe "String#rstrip" do
  it "returns a copy of self with trailing whitespace removed" do
   "  hello  ".rstrip.should == "  hello"
   "  hello world  ".rstrip.should == "  hello world"
   "  hello world\n\r\t\n\r".rstrip.should == "  hello world"
   "hello".rstrip.should == "hello"
  end
end

describe "String#rstrip!" do
  it "modifies self in place" do
    a = "  hello  "
    a.rstrip!.should == "  hello"
    a.should == "  hello"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.rstrip!.should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.rstrip!
    end
  end
end

describe "String#scan(pattern)" do
  it "returns an array containing all matches" do
    "cruel world".scan(/\w+/).should == ["cruel", "world"]
    "cruel world".scan(/.../).should == ["cru", "el ", "wor"]
    
    # Edge case
    "hello".scan(//).should == ["", "", "", "", "", ""]
    "".scan(//).should == [""]
  end
  
  it "stores groups as arrays in the returned arrays" do
    "cruel world".scan(/(...)/).should == [["cru"], ["el "], ["wor"]]
    "cruel world".scan(/(..)(..)/).should == [["cr", "ue"], ["l ", "wo"]]
  end
  
  it "scans for occurences of pattern if pattern is a string" do
    "one two one two".scan('one').should == ["one", "one"]
  end
  
  it "raises a TypeError if pattern can't be converted to a Regexp" do
    should_raise(TypeError) do
      "cruel world".scan(5)
    end

    should_raise(TypeError) do
      "cruel world".scan(:test)
    end
    
    should_raise(TypeError) do
      "cruel world".scan(Object.new)
    end
  end
end

describe "String#scan(pattern) { block }" do
  it "passes matches to the block" do
    a = []
    "cruel world".scan(/\w+/) { |w| a << w }
    a.should == ["cruel", "world"]
  end
  
  it "passes groups as arguments to the block" do
    a = []
    "cruel world".scan(/(..)(..)/) { |x, y| a << [x, y] }
    a.should == [["cr", "ue"], ["l ", "wo"]]
  end
end

describe "String#size" do
  it "is an alias of String#length" do
    "".length.should == "".size
    "one".length.should == "one".size
    "two".length.should == "two".size
    "three".length.should == "three".size
    "four".length.should == "four".size
  end
end

describe "String#slice" do
  it "is an alias of String#[]" do
    # TODO:
  end
end

describe "String#slice!(fixnum)" do
  it "deletes and return the char at the given position" do
    a = "hello"
    a.slice!(1).should == ?e
    a.should == "hllo"
  end
  
  it "returns nil if the given position is out of self" do
    a = "hello"
    a.slice(10).should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1)
    end
  end
  
  it "doesn't raise a TypeError if self is frozen but the given position is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10)
  end
end

describe "String#slice!(fixnum, fixnum)" do
  it "deletes and return the chars at the defined position" do
    a = "hello"
    a.slice!(1, 2).should == "el"
    a.should == "hlo"
  end

  it "returns nil if the given position is out of self" do
    a = "hello"
    a.slice(10, 3).should == nil
    a.should == "hello"
  end
  
  it "returns nil if the length is negative" do
    a = "hello"
    a.slice(4, -3).should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1, 2)
    end
  end
  
  it "doesn't raise a TypeError if self is frozen but the given position is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10, 3)
  end
end

describe "String#slice!(range)" do
  it "deletes and return the chars between the given range" do
    a = "hello"
    a.slice!(1..3).should == "ell"
    a.should == "ho"
    
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

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1..3)
    end
  end
  
  it "doesn't raise a TypeError if self is frozen but the given range is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10..20).should == nil
  end
end

describe "String#slice!(regexp)" do
  it "deletes the first match from self" do
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
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      s = "this is a string"
      s.freeze
      s.slice!(/s.*t/)
    end
  end
  
  it "doesn't raise a TypeError if self is frozen but there is no match" do
    s = "this is a string"
    s.freeze
    s.slice!(/zzz/).should == nil
  end
end

describe "String#slice!(other)" do
  it "removes the first occurence of other from the self" do
    c = "hello hello"
    c.slice!('llo').should == "llo"
    c.should == "he hello"
  end
  
  it "returns nil if self does not contain other" do
    a = "hello"
    a.slice!('zzz').should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      s = "hello hello"
      s.freeze
      s.slice!('llo')
    end
  end
  
  it "doesn't raise a TypeError if self is frozen but self does not contain other" do
    s = "this is a string"
    s.freeze
    s.slice!('zzz').should == nil
  end
end

describe "String#split(string [, limit])" do
  it "returns an array of substrings based on the given delimeter" do
    "mellow yellow".split("ello").should == ["m", "w y", "w"]
  end
  
  it "suppresses trailing null fields when no limit is given" do
    "1,2,,3,4,,".split(',').should == ["1", "2", "", "3", "4"]
  end
  
  it "doesn't suppress trailing null fields when limit is negative" do
    "1,2,,3,4,,".split(',', -1).should == ["1", "2", "", "3", "4", "", ""]
  end
  
  it "returns at most fields as specified by limit" do
    "1,2,,3,4,,".split(',', 4).should == ["1", "2", "", "3,4,,"]
  end
  
  it "splits self on whitespace if string is $; (default value: nil)" do
    " now's  the time".split.should == ["now's", "the", "time"]
  end
  
  it "ignores leading and continuous whitespace when string is a single space" do
    " now's  the time".split(' ').should == ["now's", "the", "time"]
  end
end

describe "String#split(regexp [, limit])" do
  it "divides self where the pattern matches" do
    " now's  the time".split(/ /).should == ["", "now's", "", "the", "time"]
    "1, 2.34,56, 7".split(/,\s*/).should == ["1", "2.34", "56", "7"]
  end
  
  it "splits self into individual characters when regexp matches a zero-length string" do
    "hello".split(//).should == ["h", "e", "l", "l", "o"]
    "hi mom".split(/\s*/).should == ["h", "i", "m", "o", "m"]
  end
  
  it "returns at most fields as specified by limit" do
    "hello".split(//, 3).should == ["h", "e", "llo"]
  end
end

describe "String#squeeze([other_strings])" do
  it "returns new string where runs of the same character are replaced by a single character when no args are given" do
    "yellow moon".squeeze.should == "yelow mon"
  end
  
  it "onlies squeeze chars that are in the intersection of all other_strings given" do
    "woot squeeze cheese".squeeze("eost", "queo").should == "wot squeze chese"
    "  now   is  the".squeeze(" ").should == " now is the"
  end
  
  it "squeezes the chars that are in the given sequence" do
    "putters shoot balls".squeeze("m-z").should == "puters shot balls"
  end
end

describe "String#squeeze!([other_strings])" do
  it "modifies self in place" do
    a = "yellow moon"
    a.squeeze!.should == "yelow mon"
    a.should == "yelow mon"
  end
  
  it "returns nil if no modifications were made" do
    a = "squeeze"
    a.squeeze!("u", "sq").should == nil
    a.should == "squeeze"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "yellow moon"
      a.freeze
      a.squeeze!
    end
  end
end

describe "String#strip" do
  it "returns a new string with leading and trailing whitespace removed" do
    "   hello   ".strip.should == "hello"
    "   hello world   ".strip.should == "hello world"
    "\tgoodbye\r\n".strip.should == "goodbye"
    "  goodbye \000".strip.should == "goodbye"
  end
end

describe "String#strip!" do
  it "modifies self in place" do
    a = "   hello   "
    a.strip!.should == "hello"
    a.should == "hello"
  end
  
  it "returns nil when no modifications where made" do
    a = "hello"
    a.strip!.should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.strip!
    end
  end
end

describe "String#sub(pattern, replacement)" do
  it "returns a copy of self with all occurences of pattern replaced with replacement" do
    "hello".sub(/[aeiou]/, '*').should == "h*llo"
  end
  
  it "doesn't interpret regexp metacharacters if pattern is a string" do
    "12345".sub('\d', 'a').should == "12345"
    '\d'.sub('\d', 'a').should == "a"
  end
  
  it "interpolates successive groups in the match with \1, \2 and so on" do
    "hello".sub(/([aeiou])/, '<\1>').should == "h<e>llo"
  end
  
  it "inherits the tainting in the original string or the replacement" do
    a = "hello"
    a.taint
    a.sub(/./, 'a').tainted?.should == true
    
    b = "hello"
    c = 'a'
    c.taint
    b.sub(/./, c).tainted?.should == true
  end
  
  it "tries to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".sub(/hello/, replacement).should == "hello_replacement"
  end
  
  it "raises a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) do
      "hello".sub(/[aeiou]/, :woot)
    end

    should_raise(TypeError) do
      "hello".sub(/[aeiou]/, 5)
    end
  end
end

describe "String#sub(pattern) { block }" do
  it "returns a copy of self with all occurences of pattern replaced with the block's return value" do
    "hello".sub(/./) { |s| s[0].to_s + ' ' }.should == "104 ello"
  end
  
  it "allows the use of variables such as $1, $2, $`, $& and $' in the block" do
    "hello".sub(/([aeiou])/) { "<#$1>" }.should == "h<e>llo"
    "hello".sub(/([aeiou])/) { "<#$&>" }.should == "h<e>llo"
  end
  
  it "converts the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".sub(/hello/) { replacement }.should == "hello_replacement"
  end
  
# TODO: This should raise a RuntimeError, but does not
#  it "raises a RuntimeError" do
#    str = "a" * 0x20
#    str.sub(/\z/) {
#      dest = nil
#      ObjectSpace.each_object(String) {|o|
#         dest = o if o.length == 0x20+30
#      }
#      dest
#    }
#  end
end

describe "String#sub!(pattern, replacement)" do
  it "modifies self in place" do
    a = "hello"
    a.sub!(/[aeiou]/, '*').should == "h*llo"
    a.should == "h*llo"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.sub!(/z/, '*').should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.sub!(/[aeiou]/, '*')
    end
  end
end

describe "String#succ" do
  it "returns the successor to self by increasing the rightmost alphanumeric" do
    "abcd".succ.should == "abce"
    "THX1138".succ.should == "THX1139"
    
    "1999zzz".succ.should == "2000aaa"

    "<<koala>>".succ.should == "<<koalb>>"
    "==a??".succ.should == "==b??"
  end
  
  it "also increases non-alphanumerics if there are no alphanumerics" do
    "***".succ.should == "**+"
  end
  
  it "increments a digit with a digit" do
    "98".succ.should == "99"
  end
  
  it "adds an additional character if there is no carry" do
    "ZZZ9999".succ.should == "AAAA0000"
  end
end

describe "String#succ!" do
  it "it should modify self in place" do
    a = "abcd"
    a.succ!.should == "abce"
    a.should == "abce"
    
    b = "THX1138"
    b.succ!.should == "THX1139"
    b.should == "THX1139"
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcd"
      a.freeze
      a.succ!
    end
  end
end

describe "String#sum(n)" do
  it "returns a basic n-bit checksum of the characters in self" do
    "ruby".sum.should == 450
    "ruby".sum(8).should == 194
    "rubinius".sum(23).should == 881
  end
end

describe "String#swapcase" do
  it "returns a new string with all uppercase chars from self converted to lowercase and vice versa" do
   "Hello".swapcase.should == "hELLO"
   "cYbEr_PuNk11".swapcase.should == "CyBeR_pUnK11"
   "+++---111222???".swapcase.should == "+++---111222???"
  end
end

describe "String#swapcase!" do
  it "modifies self in place" do
    a = "cYbEr_PuNk11"
    a.swapcase!.should == "CyBeR_pUnK11"
    a.should == "CyBeR_pUnK11"
  end
  
  it "returns nil if no modifications were made" do
    a = "+++---111222???"
    a.swapcase!.should == nil
    a.should == "+++---111222???"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "cYbEr_PuNk11"
      a.freeze
      a.swapcase!
    end
  end
end

describe "String#to_f" do
  it "treats leading characters of self as a floating point number" do
   "123.45e1".to_f.should == 1234.5
   "45.67 degrees".to_f.should == 45.67
  end

  it "takes an optional sign" do
    "-45.67 degrees".to_f.should == -45.67
    "+45.67 degrees".to_f.should == 45.67
  end
  
  it "returns 0.0 if the conversion fails" do
    "bad".to_f.should == 0.0
    "thx1138".to_f.should == 0.0
  end
end

describe "String#to_s" do
  it "returns self" do
    a = "a string"
    a.equal?(a.to_s).should == true
  end
end

describe "String#to_str" do
  it "is an alias of to_s" do
    # TODO
  end
end

describe "String#to_sym" do
  it "returns the symbol corresponding to self" do
    "Koala".to_sym.should == :Koala
    'cat'.to_sym.should == :cat
    '@cat'.to_sym.should == :@cat
    
    'cat and dog'.to_sym.should == :"cat and dog"
  end
  
  it "raises an ArgumentError when self can't be converted to symbol" do
    should_raise(ArgumentError) do
      "".to_sym
    end
  end
end

describe "String#tr_s(from_strin, to_string)" do
  it "returns a string processed according to tr with duplicate characters removed" do
    "hello".tr_s('l', 'r').should == "hero"
    "hello".tr_s('el', '*').should == "h*o"
    "hello".tr_s('el', 'hx').should == "hhxo"
  end
  
  it "accepts c1-c2 notation to denote ranges of characters" do
    "hello".tr_s('a-y', 'b-z').should == "ifmp"
    "123456789".tr_s("2-5","abcdefg").should == "1abcd6789"
  end

  it "doesn't translate chars negated with a ^ in from_string" do
    "hello".tr_s('^aeiou', '*').should == "*e*o"
    "123456789".tr_s("^345", "abc").should == "c345c"
    "abcdefghijk".tr_s("^d-g", "9131").should == "1defg1"
  end
  
  it "pads to_str with it's last char if it is shorter than from_string" do
    "this".tr_s("this", "x").should == "x"
  end
end

describe "String#tr_s!(from_string, to_string)" do
  it "modifies self in place" do
    s = "hello"
    s.tr_s!('l', 'r').should == "hero"
    s.should == "hero"
  end
  
  it "returns nil if no modification was made" do
    s = "hello"
    s.tr!('za', 'yb').should == nil
    s.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.tr_s!('l', 'r')
    end
  end
end

describe "String#tr(from_string, to_string)" do
  it "returns a new string with the characters from from_string replaced by the ones in to_string" do
    "hello".tr('aeiou', '*').should == "h*ll*"
    "hello".tr('el', 'ip').should == "hippo"
    "Lisp".tr("Lisp", "Ruby").should == "Ruby"
  end
  
  it "accepts c1-c2 notation to denote ranges of characters" do
    "hello".tr('a-y', 'b-z').should == "ifmmp"
    "123456789".tr("2-5","abcdefg").should == "1abcd6789"
  end
  
  it "doesn't translate chars negated with a ^ in from_string" do
    "hello".tr('^aeiou', '*').should == "*e**o"
    "123456789".tr("^345", "abc").should == "cc345cccc"
    "abcdefghijk".tr("^d-g", "9131").should == "111defg1111"
  end
  
  it "pads to_str with it's last char if it is shorter than from_string" do
    "this".tr("this", "x").should == "xxxx"
  end
end

describe "String#tr!(from_string, to_string)" do
  it "modifies self in place" do
    s = "abcdefghijklmnopqR"
    s.tr!("cdefg", "12")
    s.should == "ab12222hijklmnopqR"
  end
  
  it "returns nil if no modification was made" do
    s = "hello"
    s.tr!('za', 'yb').should == nil
    s.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcdefghijklmnopqR"
      a.freeze
      a.tr!("cdefg", "12")
    end
  end
end

describe "String#unpack(format)" do
  specify "returns an array by decoding self according to the format string" do
    "abc \0\0abc \0\0".unpack('A6Z6').should == ["abc", "abc "]
    "abc \0\0".unpack('a3a3').should == ["abc", " \000\000"]
    "abc \0abc \0".unpack('Z*Z*').should == ["abc ", "abc "]
    "aa".unpack('b8B8').should == ["10000110", "01100001"]
    "aaa".unpack('h2H2c').should == ["16", "61", 97]
    "\xfe\xff\xfe\xff".unpack('sS').should == [-2, 65534]
    "now=20is".unpack('M*').should == ["now is"]
    "whole".unpack('xax2aX2aX1aX2a').should == ["h", "e", "l", "l", "o"]
  end
end

describe "String#upto(other_string) { block }" do
  it "passes successive values, starting at self and ending at other_string, to the block" do
    a = []
    "*+".upto("*3") { |s| a << s }
    a.should == ["*+", "*,", "*-", "*.", "*/", "*0", "*1", "*2", "*3"]
  end

  it "calls the block once even when start eqals stop" do
    a = []
    "abc".upto("abc") { |s| a << s }
    a.should == ["abc"]
  end

  # This is weird but MRI behaves like that
  it "upto calls block with self even if self is less than stop but stop length is less than self length" do
    a = []
    "25".upto("5") { |s| a << s }
    a.should == ["25"]
  end

  it "upto doesn't call block if stop is less than self and stop length is less than self length" do
    a = []
    "25".upto("1") { |s| a << s }
    a.should == []
  end

  it "doesn't call the block if self is greater than stop" do
    a = []
    "5".upto("2") { |s| a << s }
    a.should == []
  end

  it "stops iterating as soon as the current value's character count gets higher than stop's" do
    a = []
    "0".upto("A") { |s| a << s }
    a.should == ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
  end

  it "returns self" do
    "abc".upto("abd") { }.should == "abc"
    "5".upto("2") { |i| i }.should == "5"
  end

  it "tries to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "abd" end

    a = []
    "abc".upto(other) { |s| a << s }
    a.should == ["abc", "abd"]
  end

  it "raises a TypeError if other can't be converted to a string" do
    should_raise(TypeError) do
      "abc".upto(123)
    end
    
    should_raise(TypeError) do
      "abc".upto(:def) { }
    end
    
    should_raise(TypeError) do
      "abc".upto(Object.new)
    end
  end

  it "raises a LocalJumpError if other is a string but no block was given" do
    should_raise(LocalJumpError) { "abc".upto("def") }
  end
end