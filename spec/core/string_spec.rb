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
  it "ignores unused arguments" do
    ("" % [1, 2, 3]).should == ""
    ("%s" % [1, 2, 3]).should == "1"
    ("%2$s" % [1, 2, 3]).should == "2"
  end
  
  it "ignores trailing percent signs" do
    ("%" % "x").should == "%"
    ("foo%" % "x").should == "foo%"
  end

  it "replaces trailing absolute argument specifier with percent sign" do
    ("hello %1$" % "foo").should == "hello %"
  end
  
  it "raises when given invalid argument specifiers" do
    should_raise(ArgumentError) { "%1" % "x" }
    should_raise(ArgumentError) { "%+" % "x" }
    should_raise(ArgumentError) { "%-" % "x" }
    should_raise(ArgumentError) { "%#" % "x" }
    should_raise(ArgumentError) { "%0" % "x" }
    should_raise(ArgumentError) { "%*" % "x" }
    should_raise(ArgumentError) { "%_" % "x" }
  end
    
  it "raises when there are less arguments than format specifiers" do
    ("foo" % []).should == "foo"
    should_raise(ArgumentError) { "%s" % [] }
    should_raise(ArgumentError) { "%s %s" % [1] }
  end
  
  it "raises when absolute and relative argument numbers are mixed" do
    should_raise(ArgumentError) { "%s %1$s" % "foo" }
    should_raise(ArgumentError) { "%1$s %s" % "foo" }

    should_raise(ArgumentError) { "%s %2$s" % ["foo", "bar"] }
    should_raise(ArgumentError) { "%2$s %s" % ["foo", "bar"] }
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
  
  it "doesn't call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2] end
    def obj.to_s() "obj" end
    should_raise(ArgumentError) { "%s %s" % obj }
    ("%s" % obj).should == "obj"
  end
  
  it "formats multiple expressions" do
    ("%b %x %d %s" % [10, 10, 10, 10]).should == "1010 a 10 10"
  end
  
  it "formats expressions mid string" do
    ("hello %s!" % "world").should == "hello world!"
  end
  
  it "formats %% into %" do
    ("%d%% %s" % [10, "of chickens!"]).should == "10% of chickens!"
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
  end
    
  it "supports character formats using %c" do
    ("%c" % 10).should == "\n"
    ("%2$c" % [10, 11, 14]).should == "\v"
    ("%-4c" % 10).should == "\n   "
    ("%*c" % [10, 3]).should == "         \003"
    
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
  
  it "supports unsigned formats using %u" do
    ("%u" % 10).should == "10"
    ("% u" % 10).should == " 10"
    ("%1$u" % [10, 20]).should == "10"
    ("%+u" % 10).should == "+10"
    ("%-7u" % 10).should == "10     "
    ("%04u" % 10).should == "0010"
    ("%*u" % [10, 4]).should == "         4"
    
    ("%u" % -431647880).should == "..3863319416"
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

describe "String#*(Integer)" do
  it "should return a new string containing integer copies of self" do
    ("cool" * 3).should == "coolcoolcool"
  end
  
  it "should try to convert the given argument to an integer using to_int" do
    ("cool" * 3.1).should == "coolcoolcool"
    ("a" * 3.999).should == "aaa"
    
    a = Object.new
    def a.to_int() 4; end
    
    ("a" * a).should == "aaaa"
  end
  
  it "should raise an ArgumentError when given integer is negative" do
    should_raise(ArgumentError) do
      "cool" * -3
    end
    
    should_raise(ArgumentError) do
      "cool" * -3.14
    end
  end
  
  it "should raise a RangeError when given integer is a Bignum" do
    should_raise(RangeError) do
      "cool" * 9999999999
    end
  end
end

describe "String#+(String)" do
  it "should return a new string containing the given String concatenated to self" do
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end
  
  it "should convert the given String using to_str" do
    c = Object.new
    def c.to_str() "aaa" end
    
    ("a" + c).should == "aaaa"
  end
end

describe "String#<<(String)" do
  it "should return self after concatenating the given argument to self" do
    a = 'hello ' << 'world'
    a.should == "hello world"
  end
  
  it "should convert the given argument to a String using to_str before concatenating" do
    obj = Object.new
    def obj.to_str() "world!" end
    
    a = 'hello ' << obj
    a.should == 'hello world!'
  end
  
  it "should raise a TypeError if the given argument can't be converted to a String" do
    should_raise(TypeError) do
      a = 'hello ' << :world
    end

    should_raise(TypeError) do
      a = 'hello ' << Object.new
    end
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a << "test"
    end
  end
end

describe "String#<<(Fixnum)" do
  it "should convert the given Fixnum to a char before concatenating" do
    b = 'hello ' << 'world' << 33
    b.should == "hello world!"
  end
  
  it "should raise a TypeError when the given Fixnum is not between 0 and 255" do
    should_raise(TypeError) do
      "hello world" << 333
    end
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a << 33
    end
  end
end

describe "String#<=>(other_string)" do
  specify "should return -1 when self is less than other" do
    ("this" <=> "those").should == -1
  end

  specify "should return 0 when self is equal to other" do
    ("yep" <=> "yep").should == 0
  end

  specify "should return 1 when self is greater than other" do
    ("yoddle" <=> "griddle").should == 1
  end
  
  specify "should consider string that comes lexicographically first to be less if strings have same size" do
    ("aba" <=> "abc").should == -1
    ("abc" <=> "aba").should == 1
  end

  specify "should consider shorter string to be less if longer string starts with shorter one" do
    ("abc" <=> "abcd").should == -1
    ("abcd" <=> "abc").should == 1
  end

  specify "should compare shorter string with corresponding number of first chars of longer string" do
    ("abx" <=> "abcd").should == 1
    ("abcd" <=> "abx").should == -1
  end
end

describe "String#<=>(obj)" do
  specify "should return nil if obj does not respond to to_str" do
    ("abc" <=> 1).should == nil
    ("abc" <=> :abc).should == nil
    ("abc" <=> Object.new).should == nil
  end
  
  specify "should return nil if obj does not respond to <=>" do
    obj = Object.new
    def obj.to_str() "" end
    
    ("abc" <=> obj).should == nil
  end
  
  specify "should compare the obj and self by calling <=> on obj and turning the result around" do
    obj = Object.new
    def obj.to_str() "" end
    def obj.<=>(arg) 1  end
    
    ("abc" <=> obj).should == -1
    ("xyz" <=> obj).should == -1
  end
end

describe "String#==(other_string)" do
  it "should return true if self <=> string returns 0" do
    ('hello' == 'hello').should == true
  end
  
  it "should return false if self <=> string does not return 0" do
    ("more" == "MORE").should == false
    ("less" == "greater").should == false
  end
end

describe "String#==(obj)" do
  it "should return false if obj does not respond to to_str" do
    ('hello' == 5).should == false
    ('hello' == :hello).should == false
    ('hello' == Object.new).should == false
  end
  
  it "should return obj == self if obj responds to to_str" do
    obj = Object.new
    def obj.to_str() "world!" end
    def obj.==(other) true end
    
    ('hello' == obj).should == true
    ('world!' == obj).should == true 
  end
end

describe "String#=~(regexp)" do
  it "should return the position the match starts" do
    ("rudder" =~ /udder/).should == 1
    ("boat" =~ /[^fl]oat/).should == 0
  end
  
  it "should return nil if there is no match" do
    ("bean" =~ /bag/).should == nil
    ("true" =~ /false/).should == nil
  end
end

describe "String#=~(obj)" do
  it "should raise a TypeError if a obj is a string" do
    should_raise(TypeError) do
      "some string" =~ "another string"
    end
  end
  
  it "should invoke obj.=~ with self" do
    obj = Object.new
    def obj.=~(object) true end
    
    ("w00t" =~ obj).should == true
  end
end

describe "String#[fixnum]" do
  it "should return the character code of the character at fixnum" do
    "hello"[0].should == 104
    "hello"[-1].should == 111
  end
  
  it "should return nil if fixnum falls outside of self" do
    "hello"[20].should == nil
    "hello"[-20].should == nil
  end
end

describe "String#[fixnum, fixnum]" do
  it "should return substring starting at the offset (first fixnum) with the given length (second fixnum)" do
    "hello there"[1,3].should == "ell"
    "hello there"[-3,2].should == "er"
  end
  
  it "should return nil if the offset falls outside of self" do
    "hello there"[20,3].should == nil
    "hello there"[-20,3].should == nil
  end
  
  it "should return nil if the length is negative" do
    "hello there"[4,-3].should == nil
    "hello there"[-4,-3].should == nil
  end
end

describe "String#[range]" do
  it "should return substring given by the offsets of the range" do
    "hello there"[1..3].should == "ell"
    "hello there"[-4..-2].should == "her"
    "hello there"[5..-1].should == " there"
  end
  
  it "should return nil if the beginning of the range falls outside of self" do
    "hello there"[12..-1].should == nil
    "hello there"[20..25].should == nil
  end
  
  it "should return an empty string if the beginning if the range is negative and greater than the end" do
    "hello there"[-2..-4].should == ""
    "hello there"[-5..-6].should == ""
  end
end

describe "String#[regexp]" do
  it "should return the matching portion of self" do
    "hello there"[/[aeiou](.)\1/].should == "ell"
  end
  
  it "should return nil if there is no match" do
    "hello there"[/xyz/].should == nil
  end
end

describe "String#[regexp, fixnum]" do
  it "should return the capture matching the fixnum" do
    "hello there"[/[aeiou](.)\1/, 0].should == "ell"
    "hello there"[/[aeiou](.)\1/, 1].should == "l"
  end
  
  it "should return nil if there is no match" do
    "hello there"[/[aeiou](.)\1/, 2].should == nil
  end
end

describe "String#[other_string]" do
  it "should return other_string if it occurs in self" do
    "hello there"["lo"].should == "lo"
  end
  
  it "should return nil if there is no match" do
    "hello there"["bye"].should == nil
  end
end

describe "String#[fixnum] = fixnum" do
  it "should set the code of the character at fixnum (first) to fixnum (second)" do
    a = "hello"
    (a[0] = ?b).should == ?b
    a.should == "bello"
    
    b = "hello"
    (b[-1] = ?a).should == ?a
    b.should == "hella"
  end
 
  it "should raise an IndexError if fixnum falls outside of self" do
    should_raise(IndexError) do
      "hello"[20] = ?a
    end
    
    should_raise(IndexError) do
      "hello"[-20] = ?a
    end
  end

  it "should not update self with the given char if fixnum falls outside of self" do
    a = "hello"
    (a[20] = ?a) rescue nil
    a.should == "hello"

    a = "hello"
    (a[-20] = ?a) rescue nil
    a.should == "hello"
  end
  
  it "should raise a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) do
      a[0] = ?b
    end
  end
end

describe "String#[fixnum] = other_str" do
  it "should replace the char at fixnum with other_str" do
    a = "hello"
    (a[0] = "bam").should == "bam"
    a.should == "bamello"
  end

  it "should raise an IndexError if fixnum falls outside of self" do
    should_raise(IndexError) do
      "hello"[20] = "bam"
    end
    
    should_raise(IndexError) do
      "hello"[-20] = "bam"
    end
  end

  it "should raise a TypeError when self is frozen" do
    a = "hello"
    a.freeze
    
    should_raise(TypeError) do
      a[0] = "bam"
    end
  end
  
  it "should try to convert other_str to a String using to_str" do
    other_str = Object.new
    def other_str.to_str() "-test-" end
    
    a = "abc"
    a[1] = other_str
    a.should == "a-test-c"
  end
  
  it "should raise a TypeError if other_str can't be converted to a String" do
    should_raise(TypeError) do
      "test"[1] = :test
    end

    should_raise(TypeError) do
      "test"[1] = Object.new
    end
  end
end

# TODO: Add more String#[]= specs

describe "String#capitalize" do
  it "should return a copy of self with the first character converted to uppercase and the remainder to lowercase" do
    "hello".capitalize.should == "Hello"
    "HELLO".capitalize.should == "Hello"
    "123ABC".capitalize.should == "123abc"
  end
end

describe "String#capitalize!" do
  it "should capitalize self in place" do
    a = "hello"
    a.capitalize!.should == "Hello"
    a.should == "Hello"
  end
  
  it "should return nil when no changes are made" do
    a = "Hello"
    a.capitalize!.should == nil
    a.should == "Hello"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "Hello"
      a.freeze
      a.capitalize!
    end
  end
end

describe "String#casecmp" do
  it "should be a case-insensitive version of String#<=>" do
    "abcdef".casecmp("abcde").should == 1
    "aBcDeF".casecmp("abcdef").should == 0
    "abcdef".casecmp("abcdefg").should == -1
    "abcdef".casecmp("ABCDEF").should == 0
  end
end

describe "String#center(integer, padstr)" do
  it "should return a new string of length integer with self centered and padded with padstr" do
    "one".center(9, '.').should       == "...one..."
    "hello".center(20, '123').should  == "1231231hello12312312"
    "middle".center(13, '-').should   == "---middle----"
  end
  
  it "should pad with whitespace if no padstr is given" do
    "two".center(5).should    == " two "
    "hello".center(20).should == "       hello        "
  end
  
  it "should try to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "123" end
    
    "hello".center(20, padstr).should == "1231231hello12312312"
  end
  
  it "should raise an ArgumentError when padstr can't be converted to a string" do
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
  
  it "should return self if it's longer than or as long as integer chars" do
    "hello".center(4).should == "hello"
    "this".center(3).should == "this"
    "radiology".center(8, '-').should == "radiology"
  end
  
  it "should raise an ArgumentError if padstr length is 0" do
    should_raise(ArgumentError) do
      "hello".center(10, "")
    end
  end
end

describe "String#chomp(separator)" do
  it "should return a new string with the given record separator removed" do
    "hello".chomp("llo").should == "he"
  end

  it "should remove carriage return chars multiple times when seperator is an empty string" do
    "hello".chomp("").should == "hello"
    "hello\n".chomp("").should == "hello"
    "hello\r\n".chomp("").should == "hello"
    "hello\r\n\r\n\n\n\r\n".chomp("").should == "hello"
  end
  
  it "should remove carriage return chars when separator is \\n" do
    "hello".chomp("\n").should == "hello"
    "hello\n".chomp("\n").should == "hello"
    "hello\r\n".chomp("\n").should == "hello"
    "hello\n\r".chomp("\n").should == "hello\n"
    "hello\r".chomp("\n").should == "hello"
    "hello \n there".chomp("\n").should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp("\n").should == "hello\r\n\r\n\n\n"
  end
  
  it "should remove carriage return chars when separator is $/ (default)" do
    "hello".chomp.should == "hello"
    "hello\n".chomp.should == "hello"
    "hello\r\n".chomp.should == "hello"
    "hello\n\r".chomp.should == "hello\n"
    "hello\r".chomp.should == "hello"
    "hello \n there".chomp.should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp.should == "hello\r\n\r\n\n\n"
  end
  
  it "should return self if the separator is nil" do
    "hello\n\n".chomp(nil).should == "hello\n\n"
  end
  
  it "should try to convert separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() "llo" end
    
    "hello".chomp(separator).should == "he"
  end
  
  it "should raise a TypeError if separator can't be converted to a string" do
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
  it "should modify self in place" do
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
  
  it "should return nil if no modifications were made" do
     v = "four"
     v.chomp!.should == nil
     v.should == "four"
    
    "".chomp!.should == nil
    "line".chomp!.should == nil
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "string\n\r"
      a.freeze
      a.chomp!
    end
  end
end

describe "String#concat(String)" do
  it "should be an alias of String#<<" do
    (a = "abc") << "xyz"
    (b = "abc").concat("xyz")
    a.should == b
  end
end

describe "String#delete(*String)" do
  it "should return a new string with the chars from other_string removed" do
    "hello".delete("lo").should == "he"
  end
  
  it "should delete only the intersection of characters in other_strings" do
    "hello".delete("l", "lo").should == "heo"
  end

  it "should negate strings starting with ^" do
    "hello".delete("aeiou", "^e").should == "hell"
  end

  it "should delete all chars in a sequence" do
    "hello".delete("ej-m").should == "ho"
  end

  it "should try to convert each other_string given to a string using to_str" do
    other_string = Object.new
    def other_string.to_str() "lo" end
    
    other_string2 = Object.new
    def other_string2.to_str() "o" end
    
    "hello world".delete(other_string, other_string2).should == "hell wrld"
  end
  
  it "should raise a TypeError when one other_string can't be converted to a string" do
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
  it "should modify self in place" do
    a = "hello"
    a.delete!("aeiou", "^e").should == "hell"
    a.should == "hell"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.delete!("z").should == nil
    a.should == "hello"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.delete!("aeiou", "^e")
    end
  end
end

describe "String#downcase" do
  it "should return a copy of self with all uppercase letters downcase'd" do
    "hELLO".downcase.should == "hello"
    "hello".downcase.should == "hello"
  end
  
  it "should be locale insensitive (only replacing A-Z)" do
    "ÄÖÜ".downcase.should == "ÄÖÜ"
  end
end

describe "String#downcase!" do
  it "should modify self in place" do
    a = "HeLlO"
    a.downcase!.should == "hello"
    a.should == "hello"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.downcase!.should == nil
    a.should == "hello"
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "HeLlO"
      a.freeze
      a.downcase!
    end
  end
end

describe "String#dump" do
  it "should produce a version of self with all nonprinting charaters replaced by \\nnn notation" do
    ("\000".."A").to_a.to_s.dump.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end
  
  it "should ignore the $KCODE setting" do
    old_kcode = $KCODE

    $KCODE = "NONE"
    "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""

    $KCODE = "UTF-8"
    "äöü".dump.should == "\"\\303\\244\\303\\266\\303\\274\""
    
    $KCODE = old_kcode
  end
end

describe "String#each_byte" do
  it "should pass each byte in self to the given block" do
    a = []
    "hello".each_byte { |c| a << c }
    a.should == [104, 101, 108, 108, 111]
  end
  
  it "should return self" do
    ("hello".each_byte {}).should == "hello"
  end
end

describe "String#each(separator)" do
  it "should split self using the supplied record separator (default: $/) and pass each substring to the block" do
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
  
  it "should taint substrings that are passed to the block if self is tainted" do
    a = "one\ntwo\r\nthree"
    b = []
    a.taint
    a.each { |s| b << s.tainted? }
    b.should == [true, true, true]
  end
  
  it "should pass self as a whole to the block if the separator is nil" do
    a = []
    "one\ntwo\r\nthree".each(nil) { |s| a << s }
    a.should == ["one\ntwo\r\nthree"]
  end
  
  it "should append multiple successive newlines together when the separotar is an empty string" do
    a = []
    "hello\n\n\nworld".each('') { |s| a << s }
    a.should == ["hello\n\n\n", "world"]
  end
  
  it "should return self" do
    ("hello\nworld".each {}).should == "hello\nworld"
  end

  it "should try to convert each separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() 'l' end
    
    a = []
    "hello\nworld".each(separator) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
  end
  
  it "should raise a TypeError when the separator can't be converted to a string" do
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
  it "should be an alias of String#each" do
    a = []
    "one\ntwo\r\nthree".each { |s| a << s }

    b = []
    "one\ntwo\r\nthree".each_line { |s| b << s }

    a.should == b
  end
end

describe "String#empty?" do
  it "should return true if the string has a length of zero" do
    "hello".empty?.should == false
    " ".empty?.should == false
    "".empty?.should == true
  end
end

describe "String#eql?" do
  it "should return true if two strings have the same length and content" do
    "hello".eql?("hello").should == true
    "hello".eql?("hell").should == false
    "1".eql?(1).should == false
  end
end

describe "String#gsub(pattern, replacement)" do
  it "should return a copy of self with all occurences of pattern replaced with replacement" do
    "hello".gsub(/[aeiou]/, '*').should == "h*ll*"
  end
  
  it "should not interpret regexp metacharacters if pattern is a string" do
    "12345".gsub('\d', 'a').should == "12345"
    '\d'.gsub('\d', 'a').should == "a"
  end
  
  it "should interpolate successive groups in the match with \\1, \\2 and so on" do
    "hello".gsub(/([aeiou])/, '<\1>').should == "h<e>ll<o>"
  end
  
  it "should inherit the tainting in the original string or the replacement" do
    a = "hello"
    a.taint
    a.gsub(/./, 'a').tainted?.should == true
    
    b = "hello"
    c = 'a'
    c.taint
    b.gsub(/./, c).tainted?.should == true
  end
  
  it "should try to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".gsub(/hello/, replacement).should == "hello_replacement"
  end
  
  it "should raise a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) do
      "hello".gsub(/[aeiou]/, :woot)
    end

    should_raise(TypeError) do
      "hello".gsub(/[aeiou]/, 5)
    end
  end
end

describe "String#gsub(pattern) { block }" do
  it "should return a copy of self with all occurences of pattern replaced with the block's return value" do
    "hello".gsub(/./) { |s| s[0].to_s + ' ' }.should == "104 101 108 108 111 "
  end
  
  it "should allow the use of variables such as $1, $2, $`, $& and $' in the block" do
    "hello".gsub(/([aeiou])/) { "<#$1>" }.should == "h<e>ll<o>"
    "hello".gsub(/([aeiou])/) { "<#$&>" }.should == "h<e>ll<o>"
  end
  
  it "should convert the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".gsub(/hello/) { replacement }.should == "hello_replacement"
  end
  
# TODO: This should raise a RuntimeError, but does not
#  it "should raise a RuntimeError" do
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
  it "should modify self in place" do
    a = "hello"
    a.gsub!(/[aeiou]/, '*').should == "h*ll*"
    a.should == "h*ll*"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.gsub!(/z/, '*').should == nil
    a.should == "hello"
  end
  
  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.gsub!(/[aeiou]/, '*')
    end
  end
end

describe "String#hash" do
  it "should return a hash based on a string's length and content" do
    "abc".hash.should == "abc".hash
    "abc".hash.should_not == "cba".hash
  end
end

describe "String#hex" do
  it "should treat leading characters of self as a string of hex digits" do
    "0a".hex.should == 10
    "0x".hex.should == 0
  end
  
  it "should take an optional sign" do
    "-1234".hex.should == -4660
    "+1234".hex.should == 4660
  end
  
  it "should take an optional 0x" do
    "0x0a".hex.should == 10
  end
  
  it "should return 0 on error" do
    "wombat".hex.should == 0
  end
end

describe "String#include?(other)" do
  it "should return true if self contains other" do
    "hello".include?("lo").should == true
    "hello".include?("ol").should == false
  end
  
  it "should try to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "lo" end
    
    "hello".include?(other).should == true
  end
  
  it "should raise a TypeError if other can't be converted to string" do
    should_raise(TypeError) do
      "hello".include?(:lo)
    end
    
    should_raise(TypeError) do
      "hello".include?(Object.new)
    end
  end
end

describe "String#include?(fixnum)" do
  it "should return true if self contains the given char" do
    "hello".include?(?h).should == true
    "hello".include?(?z).should == false
  end
end

describe "String#index(fixnum [, offset])" do
  it "should return the index of the first occurence of the given character" do
    "hello".index(?e).should == 1
  end
  
  it "should start the search at the given offset" do
    "hello".index(?o, -2).should == 4
  end
  
  it "should return nil if no occurence is found" do
    "hello".index(?z).should == nil
    "hello".index(?e, -2).should == nil
  end
end

describe "String#index(substring [, offset])" do
  it "should return the index of the first occurence of the given substring" do
    "hello".index('e').should == 1
    "hello".index('lo').should == 3
  end
  
  it "should start the search at the given offset" do
    "hello".index('o', -3).should == 4
  end
  
  it "should return nil if no occurence is found" do
    "hello".index('z').should == nil
    "hello".index('e', -2).should == nil
    "a-b-c".split("-").should == ["a", "b", "c"]
  end
  
  it "should raise a TypeError if no string was given" do
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
  it "should return the index of the first match with the given regexp" do
    "hello".index(/[aeiou]/).should == 1
  end
  
  it "should start the search at the given offset" do
    "hello".index(/[aeiou]/, -3).should == 4
  end
  
  it "should return nil if no occurence is found" do
    "hello".index(/z/).should == nil
    "hello".index(/e/, -2).should == nil
  end
end

describe "String#insert(index, other)" do
  it "should insert other before the character at the given index" do
    "abcd".insert(0, 'X').should == "Xabcd"
    "abcd".insert(3, 'X').should == "abcXd"
    "abcd".insert(4, 'X').should == "abcdX"
  end
  
  it "should modify self in place" do
    a = "abcd"
    a.insert(4, 'X').should == "abcdX"
    a.should == "abcdX"
  end
  
  it "should insert after the given character on an negative count" do
    "abcd".insert(-3, 'X').should == "abXcd"
    "abcd".insert(-1, 'X').should == "abcdX"
  end
  
  it "should raise an IndexError if the index is out of string" do
    should_raise(IndexError) do
      "abcd".insert(5, 'X')
    end
    
    should_raise(IndexError) do
      "abcd".insert(-6, 'X')
    end
  end
  
  it "should convert other to a string using to_str" do
    other = Object.new
    def other.to_str() "XYZ" end
    
    "abcd".insert(-3, other).should == "abXYZcd"
  end
  
  it "should raise a TypeError if other can't be converted to string" do
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
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcd"
      a.freeze
      a.insert(4, 'X')
    end
  end
end

describe "String#inspect" do
  it "should produce a version of self with all nonprinting charaters replaced by \\nnn notation" do
    ("\000".."A").to_a.to_s.dump.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end
  
  it "should produce different output based on $KCODE" do
    old_kcode = $KCODE

    $KCODE = "NONE"
    "äöü".inspect.should == "\"\\303\\244\\303\\266\\303\\274\""

    $KCODE = "UTF-8"
    "äöü".inspect.should == "\"äöü\""
    
    $KCODE = old_kcode
  end
end

describe "String#length" do
  it "should return the length of self" do
    "".length.should == 0
    "one".length.should == 3
    "two".length.should == 3
    "three".length.should == 5
    "four".length.should == 4
  end
end

describe "String#ljust(integer, padstr)" do
  it "should return a new integer with length of integer and self left justified and padded with padstr (default: whitespace)" do
    "hello".ljust(20).should         == "hello               "
    "hello".ljust(20, '1234').should == "hello123412341234123"
  end

  it "should return self if self is longer than integer" do
    "hello".ljust(5).should == "hello"
    "hello".ljust(1).should == "hello"
  end
  
  it "should raise an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".ljust(10, '')
    end
  end
  
  it "should try to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "1234" end
    
    "hello".ljust(20, padstr).should == "hello123412341234123"
  end
  
  it "should raise a TypeError when padstr can't be converted" do
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
  it "should return a copy of self with leading whitespace removed" do
   "  hello  ".lstrip.should == "hello  "
   "  hello world  ".lstrip.should == "hello world  "
   "\n\r\t\n\rhello world  ".lstrip.should == "hello world  "
   "hello".lstrip.should == "hello"
  end
end

describe "String#lstrip!" do
  it "should modify self in place" do
    a = "  hello  "
    a.lstrip!.should == "hello  "
    a.should == "hello  "
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.lstrip!.should == nil
    a.should == "hello"
  end
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.lstrip!
    end
  end
end

describe "String#match(pattern)" do
  it "should match the pattern against self" do
    'hello'.match(/(.)\1/)[0].should == 'll'
  end

  it "should convert pattern to a regexp if it isn't already one" do
    'hello'.match('(.)\1')[0].should == 'll'
  end
  
  it "should return nil if there's no match" do
    'hello'.match('xx').should == nil
  end
  
  it "should raise a TypeError if pattern is not a regexp or a string" do
    should_raise TypeError do
      'hello'.match(10)
    end
    
    should_raise TypeError do
      'hello'.match(:ell)
    end
  end
end

describe "String#next" do
  it "should be an alias of String#succ" do
    "abcd".succ.should == "abcd".next
    "98".succ.should == "98".next
    "ZZZ9999".succ.should == "ZZZ9999".next
  end
end

describe "String#next!" do
  it "should be an alias of String#succ!" do
    a = "THX1138"
    b = "THX1138"
    a.succ!.should == b.next!
    a.should == b
  end
end

describe "String#oct" do
  it "should treat leading characters of self as octal digits" do
    "123".oct.should == 83
    "0377bad".oct.should == 255
  end
  
  it "should take an optional sign" do
    "-377".oct.should == -255
    "+377".oct.should == 255
  end
  
  it "should return 0 if the conversion fails" do
    "bad".oct.should == 0
  end
end

describe "String#replace(other)" do
  it "should replace the content of self with other" do
    a = "some string"
    a.replace("another string")
    a.should == "another string"
  end
  
  it "should replace the taintedness of self with that of other" do
    a = "an untainted string"
    b = "a tainted string"
    b.taint
    a.replace(b)
    a.tainted?.should == true
  end
  
  it "should try to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "an object converted to a string" end
    
    "hello".replace(other).should == "an object converted to a string"
  end
  
  it "should raise a TypeError if other can't be converted to string" do
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
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.replace("world")
    end
  end
end

describe "String#reverse" do
  it "should return a new string with the characters of self in reverse order" do
    "stressed".reverse.should == "desserts"
    "m".reverse.should == "m"
    "".reverse.should == ""
  end
end

describe "String#reverse!" do
  it "should reverse self in place" do
    a = "stressed"
    a.reverse!.should == "desserts"
    a.should == "desserts"
    
    b = ""
    b.reverse!.should == ""
    b.should == ""
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.reverse!
    end
  end
end

describe "String#rindex(fixnum [, stop_offset])" do
  it "should return the index of the last occurence of the given character" do
    "hello".rindex(?e).should == 1
    "hello".rindex(?l).should == 3
  end
  
  it "should stop the search at the given offset" do
    "blablabla".rindex(?a, -2).should == 5
  end
  
  it "should return nil if no occurence is found" do
    "hello".rindex(?z).should == nil
    "hello".rindex(?o, 2).should == nil
  end
end

describe "String#rindex(substring [, stop_offset])" do
  it "should return the index of the last occurence of the given substring" do
    "hello".rindex('e').should == 1
    "hello".rindex('l').should == 3
  end
  
  it "should stop the search at the given offset" do
    "omg omg omg".rindex('o', -3).should == 8
  end
  
  it "should return nil if no occurence is found" do
    "hello".rindex('z').should == nil
    "hello".rindex('o', 2).should == nil
  end
  
  it "should raise a TypeError if no string was given" do
    should_raise(TypeError) do
      "hello".rindex(:sym)
    end
    
    should_raise(TypeError) do
      "hello".rindex(Object.new)
    end
  end
end

describe "String#rjust(integer, padstr)" do
  it "should return a new integer with length of integer and self right justified and padded with padstr (default: whitespace)" do
    "hello".rjust(20).should         == "               hello"
    "hello".rjust(20, '1234').should == "123412341234123hello"
  end

  it "should return self if self is longer than integer" do
    "hello".rjust(5).should == "hello"
    "hello".rjust(1).should == "hello"
  end
  
  it "should raise an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".rjust(10, '')
    end
  end

  it "should try to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "1234" end
    
    "hello".rjust(20, padstr).should == "123412341234123hello"
  end
  
  it "should raise a TypeError when padstr can't be converted" do
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
  it "should return a copy of self with trailing whitespace removed" do
   "  hello  ".rstrip.should == "  hello"
   "  hello world  ".rstrip.should == "  hello world"
   "  hello world\n\r\t\n\r".rstrip.should == "  hello world"
   "hello".rstrip.should == "hello"
  end
end

describe "String#rstrip!" do
  it "should modify self in place" do
    a = "  hello  "
    a.rstrip!.should == "  hello"
    a.should == "  hello"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.rstrip!.should == nil
    a.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.rstrip!
    end
  end
end

describe "String#scan(pattern)" do
  it "should return an array containing all matches" do
    "cruel world".scan(/\w+/).should == ["cruel", "world"]
    "cruel world".scan(/.../).should == ["cru", "el ", "wor"]
    
    # Edge case
    "hello".scan(//).should == ["", "", "", "", "", ""]
    "".scan(//).should == [""]
  end
  
  it "should store groups as arrays in the returned arrays" do
    "cruel world".scan(/(...)/).should == [["cru"], ["el "], ["wor"]]
    "cruel world".scan(/(..)(..)/).should == [["cr", "ue"], ["l ", "wo"]]
  end
  
  it "should scan for occurences of pattern if pattern is a string" do
    "one two one two".scan('one').should == ["one", "one"]
  end
  
  it "should raise a TypeError if pattern can't be converted to a Regexp" do
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
  it "should pass matches to the block" do
    a = []
    "cruel world".scan(/\w+/) { |w| a << w }
    a.should == ["cruel", "world"]
  end
  
  it "should pass groups as arguments to the block" do
    a = []
    "cruel world".scan(/(..)(..)/) { |x, y| a << [x, y] }
    a.should == [["cr", "ue"], ["l ", "wo"]]
  end
end

describe "String#size" do
  it "should be an alias of String#length" do
    "".length.should == "".size
    "one".length.should == "one".size
    "two".length.should == "two".size
    "three".length.should == "three".size
    "four".length.should == "four".size
  end
end

describe "String#slice" do
  it "should be an alias of String#[]" do
    # TODO:
  end
end

describe "String#slice!(fixnum)" do
  it "should delete and return the char at the given position" do
    a = "hello"
    a.slice!(1).should == ?e
    a.should == "hllo"
  end
  
  it "should return nil if the given position is out of self" do
    a = "hello"
    a.slice(10).should == nil
    a.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1)
    end
  end
  
  it "should not raise a TypeError if self is frozen but the given position is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10)
  end
end

describe "String#slice!(fixnum, fixnum)" do
  it "should delete and return the chars at the defined position" do
    a = "hello"
    a.slice!(1, 2).should == "el"
    a.should == "hlo"
  end

  it "should return nil if the given position is out of self" do
    a = "hello"
    a.slice(10, 3).should == nil
    a.should == "hello"
  end
  
  it "should return nil if the length is negative" do
    a = "hello"
    a.slice(4, -3).should == nil
    a.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1, 2)
    end
  end
  
  it "should not raise a TypeError if self is frozen but the given position is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10, 3)
  end
end

describe "String#slice!(range)" do
  it "should delete and return the chars between the given range" do
    a = "hello"
    a.slice!(1..3).should == "ell"
    a.should == "ho"
    
    # Edge Case?
    "hello".slice!(-3..-9).should == ""
  end
  
  it "should return nil if the given range is out of self" do
    a = "hello"
    a.slice!(-6..-9).should == nil
    a.should == "hello"
    
    b = "hello"
    b.slice!(10..20).should == nil
    b.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.slice!(1..3)
    end
  end
  
  it "should not raise a TypeError if self is frozen but the given range is out of self" do
    s = "hello"
    s.freeze
    s.slice!(10..20).should == nil
  end
end

describe "String#slice!(regexp)" do
  it "should delete the first match from self" do
    s = "this is a string"
    s.slice!(/s.*t/).should == 's is a st'
    s.should == 'thiring'
    
    c = "hello hello"
    c.slice!(/llo/).should == "llo"
    c.should == "he hello"
  end
  
  it "should return nil if there was no match" do
    s = "this is a string"
    s.slice!(/zzz/).should == nil
    s.should == "this is a string"
  end
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      s = "this is a string"
      s.freeze
      s.slice!(/s.*t/)
    end
  end
  
  it "should not raise a TypeError if self is frozen but there is no match" do
    s = "this is a string"
    s.freeze
    s.slice!(/zzz/).should == nil
  end
end

describe "String#slice!(other)" do
  it "should remove the first occurence of other from the self" do
    c = "hello hello"
    c.slice!('llo').should == "llo"
    c.should == "he hello"
  end
  
  it "should return nil if self does not contain other" do
    a = "hello"
    a.slice!('zzz').should == nil
    a.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      s = "hello hello"
      s.freeze
      s.slice!('llo')
    end
  end
  
  it "should not raise a TypeError if self is frozen but self does not contain other" do
    s = "this is a string"
    s.freeze
    s.slice!('zzz').should == nil
  end
end

describe "String#split(string [, limit])" do
  it "should return an array of substrings based on the given delimeter" do
    "mellow yellow".split("ello").should == ["m", "w y", "w"]
  end
  
  it "should suppress trailing null fields when no limit is given" do
    "1,2,,3,4,,".split(',').should == ["1", "2", "", "3", "4"]
  end
  
  it "should not suppress trailing null fields when limit is negative" do
    "1,2,,3,4,,".split(',', -1).should == ["1", "2", "", "3", "4", "", ""]
  end
  
  it "should return at most fields as specified by limit" do
    "1,2,,3,4,,".split(',', 4).should == ["1", "2", "", "3,4,,"]
  end
  
  it "should split self on whitespace if string is $; (default value: nil)" do
    " now's  the time".split.should == ["now's", "the", "time"]
  end
  
  it "should ignore leading and continuous whitespace when string is a single space" do
    " now's  the time".split(' ').should == ["now's", "the", "time"]
  end
end

describe "String#split(regexp [, limit])" do
  it "should divide self where the pattern matches" do
    " now's  the time".split(/ /).should == ["", "now's", "", "the", "time"]
    "1, 2.34,56, 7".split(/,\s*/).should == ["1", "2.34", "56", "7"]
  end
  
  it "should split self into individual characters when regexp matches a zero-length string" do
    "hello".split(//).should == ["h", "e", "l", "l", "o"]
    "hi mom".split(/\s*/).should == ["h", "i", "m", "o", "m"]
  end
  
  it "should return at most fields as specified by limit" do
    "hello".split(//, 3).should == ["h", "e", "llo"]
  end
end

describe "String#squeeze([other_strings])" do
  it "should return new string where runs of the same character are replaced by a single character when no args are given" do
    "yellow moon".squeeze.should == "yelow mon"
  end
  
  it "should only squeeze chars that are in the intersection of all other_strings given" do
    "woot squeeze cheese".squeeze("eost", "queo").should == "wot squeze chese"
    "  now   is  the".squeeze(" ").should == " now is the"
  end
  
  it "should squeeze the chars that are in the given sequence" do
    "putters shoot balls".squeeze("m-z").should == "puters shot balls"
  end
end

describe "String#squeeze!([other_strings])" do
  it "should modify self in place" do
    a = "yellow moon"
    a.squeeze!.should == "yelow mon"
    a.should == "yelow mon"
  end
  
  it "should return nil if no modifications were made" do
    a = "squeeze"
    a.squeeze!("u", "sq").should == nil
    a.should == "squeeze"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "yellow moon"
      a.freeze
      a.squeeze!
    end
  end
end

describe "String#strip" do
  it "should return a new string with leading and trailing whitespace removed" do
    "   hello   ".strip.should == "hello"
    "   hello world   ".strip.should == "hello world"
    "\tgoodbye\r\n".strip.should == "goodbye"
    "  goodbye \000".strip.should == "goodbye"
  end
end

describe "String#strip!" do
  it "should modify self in place" do
    a = "   hello   "
    a.strip!.should == "hello"
    a.should == "hello"
  end
  
  it "should return nil when no modifications where made" do
    a = "hello"
    a.strip!.should == nil
    a.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "  hello  "
      a.freeze
      a.strip!
    end
  end
end

describe "String#sub(pattern, replacement)" do
  it "should return a copy of self with all occurences of pattern replaced with replacement" do
    "hello".sub(/[aeiou]/, '*').should == "h*llo"
  end
  
  it "should not interpret regexp metacharacters if pattern is a string" do
    "12345".sub('\d', 'a').should == "12345"
    '\d'.sub('\d', 'a').should == "a"
  end
  
  it "should interpolate successive groups in the match with \1, \2 and so on" do
    "hello".sub(/([aeiou])/, '<\1>').should == "h<e>llo"
  end
  
  it "should inherit the tainting in the original string or the replacement" do
    a = "hello"
    a.taint
    a.sub(/./, 'a').tainted?.should == true
    
    b = "hello"
    c = 'a'
    c.taint
    b.sub(/./, c).tainted?.should == true
  end
  
  it "should try to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".sub(/hello/, replacement).should == "hello_replacement"
  end
  
  it "should raise a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) do
      "hello".sub(/[aeiou]/, :woot)
    end

    should_raise(TypeError) do
      "hello".sub(/[aeiou]/, 5)
    end
  end
end

describe "String#sub(pattern) { block }" do
  it "should return a copy of self with all occurences of pattern replaced with the block's return value" do
    "hello".sub(/./) { |s| s[0].to_s + ' ' }.should == "104 ello"
  end
  
  it "should allow the use of variables such as $1, $2, $`, $& and $' in the block" do
    "hello".sub(/([aeiou])/) { "<#$1>" }.should == "h<e>llo"
    "hello".sub(/([aeiou])/) { "<#$&>" }.should == "h<e>llo"
  end
  
  it "should convert the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".sub(/hello/) { replacement }.should == "hello_replacement"
  end
  
# TODO: This should raise a RuntimeError, but does not
#  it "should raise a RuntimeError" do
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
  it "should modify self in place" do
    a = "hello"
    a.sub!(/[aeiou]/, '*').should == "h*llo"
    a.should == "h*llo"
  end
  
  it "should return nil if no modifications were made" do
    a = "hello"
    a.sub!(/z/, '*').should == nil
    a.should == "hello"
  end
  
  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.sub!(/[aeiou]/, '*')
    end
  end
end

describe "String#succ" do
  it "should return the successor to self by increasing the rightmost alphanumeric" do
    "abcd".succ.should == "abce"
    "THX1138".succ.should == "THX1139"
    
    "1999zzz".succ.should == "2000aaa"

    "<<koala>>".succ.should == "<<koalb>>"
    "==a??".succ.should == "==b??"
  end
  
  it "should also increase non-alphanumerics if there are no alphanumerics" do
    "***".succ.should == "**+"
  end
  
  it "should increment a digit with a digit" do
    "98".succ.should == "99"
  end
  
  it "should add an additional character if there is no carry" do
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
  
  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcd"
      a.freeze
      a.succ!
    end
  end
end

describe "String#sum(n)" do
  it "should return a basic n-bit checksum of the characters in self" do
    "ruby".sum.should == 450
    "ruby".sum(8).should == 194
    "rubinius".sum(23).should == 881
  end
end

describe "String#swapcase" do
  it "should return a new string with all uppercase chars from self converted to lowercase and vice versa" do
   "Hello".swapcase.should == "hELLO"
   "cYbEr_PuNk11".swapcase.should == "CyBeR_pUnK11"
   "+++---111222???".swapcase.should == "+++---111222???"
  end
end

describe "String#swapcase!" do
  it "should modify self in place" do
    a = "cYbEr_PuNk11"
    a.swapcase!.should == "CyBeR_pUnK11"
    a.should == "CyBeR_pUnK11"
  end
  
  it "should return nil if no modifications were made" do
    a = "+++---111222???"
    a.swapcase!.should == nil
    a.should == "+++---111222???"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "cYbEr_PuNk11"
      a.freeze
      a.swapcase!
    end
  end
end

describe "String#to_f" do
  it "should treat leading characters of self as a floating point number" do
   "123.45e1".to_f.should == 1234.5
   "45.67 degrees".to_f.should == 45.67
  end

  it "should take an optional sign" do
    "-45.67 degrees".to_f.should == -45.67
    "+45.67 degrees".to_f.should == 45.67
  end
  
  it "should return 0.0 if the conversion fails" do
    "bad".to_f.should == 0.0
    "thx1138".to_f.should == 0.0
  end
end

describe "String#to_s" do
  it "should return self" do
    a = "a string"
    a.equal?(a.to_s).should == true
  end
end

describe "String#to_str" do
  it "should be an alias of to_s" do
    # TODO
  end
end

describe "String#to_sym" do
  it "should return the symbol corresponding to self" do
    "Koala".to_sym.should == :Koala
    'cat'.to_sym.should == :cat
    '@cat'.to_sym.should == :@cat
    
    'cat and dog'.to_sym.should == :"cat and dog"
  end
  
  it "should raise an ArgumentError when self can't be converted to symbol" do
    should_raise(ArgumentError) do
      "".to_sym
    end
  end
end

describe "String#tr_s(from_strin, to_string)" do
  it "should return a string processed according to tr with duplicate characters removed" do
    "hello".tr_s('l', 'r').should == "hero"
    "hello".tr_s('el', '*').should == "h*o"
    "hello".tr_s('el', 'hx').should == "hhxo"
  end
  
  it "should accept c1-c2 notation to denote ranges of characters" do
    "hello".tr_s('a-y', 'b-z').should == "ifmp"
    "123456789".tr_s("2-5","abcdefg").should == "1abcd6789"
  end

  it "should not translate chars negated with a ^ in from_string" do
    "hello".tr_s('^aeiou', '*').should == "*e*o"
    "123456789".tr_s("^345", "abc").should == "c345c"
    "abcdefghijk".tr_s("^d-g", "9131").should == "1defg1"
  end
  
  it "should pad to_str with it's last char if it is shorter than from_string" do
    "this".tr_s("this", "x").should == "x"
  end
end

describe "String#tr_s!(from_string, to_string)" do
  it "should modify self in place" do
    s = "hello"
    s.tr_s!('l', 'r').should == "hero"
    s.should == "hero"
  end
  
  it "should return nil if no modification was made" do
    s = "hello"
    s.tr!('za', 'yb').should == nil
    s.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a.tr_s!('l', 'r')
    end
  end
end

describe "String#tr(from_string, to_string)" do
  it "should return a new string with the characters from from_string replaced by the ones in to_string" do
    "hello".tr('aeiou', '*').should == "h*ll*"
    "hello".tr('el', 'ip').should == "hippo"
    "Lisp".tr("Lisp", "Ruby").should == "Ruby"
  end
  
  it "should accept c1-c2 notation to denote ranges of characters" do
    "hello".tr('a-y', 'b-z').should == "ifmmp"
    "123456789".tr("2-5","abcdefg").should == "1abcd6789"
  end
  
  it "should not translate chars negated with a ^ in from_string" do
    "hello".tr('^aeiou', '*').should == "*e**o"
    "123456789".tr("^345", "abc").should == "cc345cccc"
    "abcdefghijk".tr("^d-g", "9131").should == "111defg1111"
  end
  
  it "should pad to_str with it's last char if it is shorter than from_string" do
    "this".tr("this", "x").should == "xxxx"
  end
end

describe "String#tr!(from_string, to_string)" do
  it "should modify self in place" do
    s = "abcdefghijklmnopqR"
    s.tr!("cdefg", "12")
    s.should == "ab12222hijklmnopqR"
  end
  
  it "should return nil if no modification was made" do
    s = "hello"
    s.tr!('za', 'yb').should == nil
    s.should == "hello"
  end

  it "should raise a TypeError if self is frozen" do
    should_raise(TypeError) do
      a = "abcdefghijklmnopqR"
      a.freeze
      a.tr!("cdefg", "12")
    end
  end
end

describe "String#unpack(format)" do
  specify "should return an array by decoding self according to the format string" do
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
  it "should pass successive values, starting at self and ending at other_string, to the block" do
    a = []
    "*+".upto("*3") { |s| a << s }
    a.should == ["*+", "*,", "*-", "*.", "*/", "*0", "*1", "*2", "*3"]
  end

  it "should call the block once even when start eqals stop" do
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

  it "should not call the block if self is greater than stop" do
    a = []
    "5".upto("2") { |s| a << s }
    a.should == []
  end

  it "should stop iterating as soon as the current value's character count gets higher than stop's" do
    a = []
    "0".upto("A") { |s| a << s }
    a.should == ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]
  end

  it "should return self" do
    "abc".upto("abd") { }.should == "abc"
    "5".upto("2") { |i| i }.should == "5"
  end

  it "should try to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "abd" end

    a = []
    "abc".upto(other) { |s| a << s }
    a.should == ["abc", "abd"]
  end

  it "should raise a TypeError if other can't be converted to a string" do
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

  it "should raise a LocalJumpError if other is a string but no block was given" do
    should_raise(LocalJumpError) { "abc".upto("def") }
  end
end