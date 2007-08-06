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
class MyRange < Range; end

# TODO: Add specs using MyRegexp

# TODO: Add specs to verify that methods operate on bytes instead of
# chars independently of encoding and $KCODE all over the place.

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
  
  it "ignores percent signs at end of string / before newlines, null bytes" do
    ("%" % []).should == "%"
    ("foo%" % []).should == "foo%"
    ("%\0x hello" % []).should == "%\0x hello"
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
    
    # Star precision before star width:
    should_raise(ArgumentError) { "%.**d" % [5, 10, 1] }

    # Precision before flags and width:
    should_raise(ArgumentError) { "%.5+05d" % 5 }
    should_raise(ArgumentError) { "%.5 5d" % 5 }

    # Overriding a star width with a numeric one:
    should_raise(ArgumentError) { "%*1s" % [5, 1] }

    # Width before flags:
    should_raise(ArgumentError) { "%5+0d" % 1 }
    should_raise(ArgumentError) { "%5 0d" % 1 }

    # Specifying width multiple times:
    should_raise(ArgumentError) { "%50+30+20+10+5d" % 5 }
    should_raise(ArgumentError) { "%50 30 20 10 5d" % 5 }

    # Specifying the precision multiple times with negative star arguments:
    should_raise(ArgumentError) { "%.*.*.*.*f" % [-1, -1, -1, 5, 1] }
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
    
    w = Object.new
    w.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    w.should_receive(:method_missing, :with => [:to_int], :returning => 10)
    p = Object.new
    p.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    p.should_receive(:method_missing, :with => [:to_int], :returning => 5)

    ("%*.*f" % [w, p, 1]).should == "   1.00000"
  end
  
  it "doesn't call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2] end
    def obj.to_s() "obj" end
    should_raise(ArgumentError) { "%s %s" % obj }
    ("%s" % obj).should == "obj"
  end
  
  it "doesn't return subclass instances when called on a subclass" do
    universal = Object.new
    def universal.to_int() 0 end
    def universal.to_str() "0" end
    def universal.to_f() 0.0 end

    [
      "", "foo",
      "%b", "%B", "%c", "%d", "%e", "%E",
      "%f", "%g", "%G", "%i", "%o", "%p",
      "%s", "%u", "%x", "%X"
    ].each do |format|
      (MyString.new(format) % universal).class.should == String
    end
  end

  it "always taints the result when the format string is tainted" do
    universal = Object.new
    def universal.to_int() 0 end
    def universal.to_str() "0" end
    def universal.to_f() 0.0 end
    
    [
      "", "foo",
      "%b", "%B", "%c", "%d", "%e", "%E",
      "%f", "%g", "%G", "%i", "%o", "%p",
      "%s", "%u", "%x", "%X"
    ].each do |format|
      subcls_format = MyString.new(format)
      subcls_format.taint
      format.taint
      
      (format % universal).tainted?.should == true
      (subcls_format % universal).tainted?.should == true
    end
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
  end
  
  it "uses argument % 256" do
    ("%c" % [256 * 3 + 64]).should == ("%c" % 64)
    ("%c" % -200).should == ("%c" % 56)
  end
  
  it "calls to_int on argument for %c formats" do
    obj = Object.new
    def obj.to_int() 65 end
    ("%c" % obj).should == ("%c" % obj.to_int)

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 65)
    ("%c" % obj).should == "A"
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
  end
  
  it "calls inspect on arguments for %p format" do
    obj = Object.new
    obj.should_receive(:inspect, :returning => "obj")
    ("%p" % obj).should == "obj"
    
    obj = Object.new
    class << obj; undef :inspect; end
    obj.should_receive(:method_missing, :with => [:inspect], :returning => "obj")
    ("%p" % obj).should == "obj"    
  end
  
  it "taints result for %p when argument.inspect is tainted" do
    obj = Object.new
    obj.should_receive(:inspect, :returning => "x".taint)
    
    ("%p" % obj).tainted?.should == true
    
    obj = Object.new; obj.taint
    obj.should_receive(:inspect, :returning => "x")
    
    ("%p" % obj).tainted?.should == false
  end
  
  it "supports string formats using %s" do
    ("%s" % 10).should == "10"
    ("%1$s" % [10, 8]).should == "10"
    ("%-5s" % 10).should == "10   "
    ("%*s" % [10, 9]).should == "         9"
  end
  
  it "calls to_s on arguments for %s format" do
    obj = Object.new
    obj.should_receive(:to_s, :returning => "obj")
    ("%s" % obj).should == "obj"

    obj = Object.new
    class << obj; undef :to_s; end
    obj.should_receive(:method_missing, :with => [:to_s], :returning => "obj")
    ("%s" % obj).should == "obj"
  end
  
  it "taints result for %s when argument is tainted" do
    ("%s" % "x".taint).tainted?.should == true
    ("%s" % Object.new.taint).tainted?.should == true
    ("%s" % 5.0.taint).tainted?.should == true
  end

  # MRI crashes on this one.
  # See http://groups.google.com/group/ruby-core-google/t/c285c18cd94c216d
  failure :mri do
    it "raises ArgumentError for huge precisions for %s" do
      should_raise(ArgumentError) do
        "%.25555555555555555555555555555555555555s" % "hello world"
      end
    end
  end
  
  # Note: %u has been changed to an alias for %d in MRI 1.9 trunk.
  # Let's wait a bit for it to cool down and see if it will
  # be changed for 1.8 as well.
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

    # Something's odd for MRI here. For details see
    # http://groups.google.com/group/ruby-core-google/msg/408e2ebc8426f449
    ("%u" % -(2 ** 64 + 5)).should == "..79228162495817593519834398715"
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
  
  %w(b d i o u x X).each do |f|
    format = "%" + f
    
    it "behaves as if calling Kernel#Integer for #{format} argument" do
      (format % "10").should == (format % 10)
      (format % nil).should == (format % 0)
      (format % "0x42").should == (format % 0x42)
      (format % "0b1101").should == (format % 0b1101)
      (format % "0b1101_0000").should == (format % 0b1101_0000)
      (format % "0777").should == (format % 0777)
      (format % "0_7_7_7").should == (format % 0777)
      
      should_raise(ArgumentError) { format % "" }
      should_raise(ArgumentError) { format % "x" }
      should_raise(ArgumentError) { format % "5x" }
      should_raise(ArgumentError) { format % "08" }
      should_raise(ArgumentError) { format % "0b2" }
      should_raise(ArgumentError) { format % "123__456" }
      
      obj = Object.new
      obj.should_receive(:to_i, :returning => 5)
      (format % obj).should == (format % 5)

      obj = Object.new
      obj.should_receive(:to_int, :returning => 5)
      (format % obj).should == (format % 5)

      obj = Object.new
      def obj.to_int() 4 end
      def obj.to_i() 0 end
      (format % obj).should == (format % 4)

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => 65)
      (format % obj).should == (format % 65)

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_int], :returning => false)
      obj.should_receive(:respond_to?, :with => [:to_i], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_i], :returning => 65)
      (format % obj).should == (format % 65)
      
      obj = Object.new
      def obj.respond_to?(*) true end
      def obj.method_missing(name, *)
        name == :to_int ? 4 : 0
      end
      (format % obj).should == (format % 4)
    end
    
    it "doesn't taint the result for #{format} when argument is tainted" do
      (format % "5".taint).tainted?.should == false
    end
  end
  
  %w(e E f g G).each do |f|
    format = "%" + f
    
    it "behaves as if calling Kernel#Float for #{format} arguments" do
      (format % 10).should == (format % 10.0)
      (format % "-10.4e-20").should == (format % -10.4e-20)
      (format % ".5").should == (format % 0.5)
      (format % "-.5").should == (format % -0.5)
      (format % "10_1_0.5_5_5").should == (format % 1010.555)
      (format % "0777").should == (format % 777)

      should_raise(ArgumentError) { format % "" }
      should_raise(ArgumentError) { format % "x" }
      should_raise(ArgumentError) { format % "." }
      should_raise(ArgumentError) { format % "10." }
      should_raise(ArgumentError) { format % "5x" }
      should_raise(ArgumentError) { format % "0xA" }
      should_raise(ArgumentError) { format % "0b1" }
      should_raise(ArgumentError) { format % "10e10.5" }
      should_raise(ArgumentError) { format % "10__10" }
      should_raise(ArgumentError) { format % "10.10__10" }
      
      obj = Object.new
      obj.should_receive(:to_f, :returning => 5.0)
      (format % obj).should == (format % 5.0)

      obj = Object.new
      obj.should_receive(:respond_to?, :with => [:to_f], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_f], :returning => 3.14)
      (format % obj).should == (format % 3.14)
    end
    
    it "doesn't taint the result for #{format} when argument is tainted" do
      (format % "5".taint).tainted?.should == false
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
    
    a = Object.new
    a.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    a.should_receive(:method_missing, :with => [:to_int], :returning => 4)
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
  
  it "always taints the result when self is tainted" do
    ["", "OK", MyString.new(""), MyString.new("OK")].each do |str|
      str.taint

      [0, 1, 2].each do |arg|
        (str * arg).tainted?.should == true
      end
    end
  end
end

describe "String#+(other)" do
  it "returns a new string containing the given string concatenated to self" do
    ("" + "").should == ""
    ("" + "Hello").should == "Hello"
    ("Hello" + "").should == "Hello"
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end
  
  it "converts its argument to a string using to_str" do
    c = Object.new
    def c.to_str() "aaa" end
    
    ("a" + c).should == "aaaa"

    c = Object.new
    c.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    c.should_receive(:method_missing, :with => [:to_str], :returning => "aaa")
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
  
  it "always taints the result when self or other is tainted" do
    strs = ["", "OK", MyString.new(""), MyString.new("OK")]
    strs += strs.map { |s| s.dup.taint }
    
    strs.each do |str|
      strs.each do |other|
        (str + other).tainted?.should == (str.tainted? | other.tainted?)
      end
    end
  end
end

describe "String#<<(other)" do
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
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "world!")
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
  
  it "taints self if other is tainted" do
    x = "x"
    (x << "".taint).tainted?.should == true

    x = "x"
    (x << "y".taint).tainted?.should == true
  end
end

describe "String#<<(fixnum)" do
  it "converts the given Fixnum to a char before concatenating" do
    b = 'hello ' << 'world' << 33
    b.should == "hello world!"
    b << 0
    b.should == "hello world!\x00"
  end

  it "doesn't use fixnum % 256" do
    should_raise(TypeError) { "" << (256 * 3 + 64) }
    should_raise(TypeError) { "" << -200 }
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

# Note: This is inconsistent with Array#<=> which calls to_str instead of
# just using it as an indicator.
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
  
  it "compares its argument and self by calling <=> on obj and turning the result around if obj responds to to_str" do
    obj = Object.new
    def obj.to_str() "" end
    def obj.<=>(arg) 1  end
    
    ("abc" <=> obj).should == -1
    ("xyz" <=> obj).should == -1
    
    obj = Object.new
    other = "abc"
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:respond_to?, :with => [:<=>], :returning => true)
    obj.should_receive(:method_missing, :with => [:<=>, other], :returning => -1)
    (other <=> obj).should == +1
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
    
    obj = Object.new
    class << obj; undef :==; end
    other = "abc"
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:==, other], :returning => true)
    (other == obj).should == true
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
  
  it "sets $~ to MatchData when there is a match and nil when there's none" do
    'hello' =~ /./
    $~[0].should == 'h'
    
    'hello' =~ /not/
    $~.should == nil
  end
end

# Let's keep this file sane and not duplicate all this two times.
["[]", "slice"].each do |cmd|
  describe "String##{cmd}(idx)" do
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

  describe "String##{cmd}(idx, length)" do
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

  describe "String##{cmd}(range)" do
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

  describe "String##{cmd}(regexp)" do
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

  describe "String##{cmd}(regexp, idx)" do
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

  describe "String##{cmd}(other_str)" do
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
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
    str[obj] = ?!
    str.should == "cell!"
  end
  
  it "sets the code to char % 256" do
    str = "Hello"
    
    str[0] = ?a + 256 * 3
    str[0].should == ?a
    str[0] = -200
    str[0].should == 56
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

  it "taints self if other_str is tainted" do
    a = "hello"
    a[0] = "".taint
    a.tainted?.should == true
    
    a = "hello"
    a[0] = "x".taint
    a.tainted?.should == true
  end

  it "raises an IndexError without changing self if idx is outside of self" do
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
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -1)
    str[obj] = "e vator"
    str.should == "hi elle vator"
  end
  
  it "tries to convert other_str to a String using to_str" do
    other_str = Object.new
    def other_str.to_str() "-test-" end
    
    a = "abc"
    a[1] = other_str
    a.should == "a-test-c"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "ROAR")

    a = "abc"
    a[1] = obj
    a.should == "aROARc"
  end
  
  it "raises a TypeError if other_str can't be converted to a String" do
    should_raise(TypeError) { "test"[1] = :test }
    should_raise(TypeError) { "test"[1] = Object.new }
    should_raise(TypeError) { "test"[1] = nil }
  end
end

describe "String#[idx, count] = other_str" do
  it "starts at idx and overwrites count characters before inserting the rest of other_str" do
    a = "hello"
    a[0, 2] = "xx"
    a.should == "xxllo"
    a = "hello"
    a[0, 2] = "jello"
    a.should == "jellollo"
  end
 
  it "counts negative idx values from end of the string" do
    a = "hello"
    a[-1, 0] = "bob"
    a.should == "hellbobo"
    a = "hello"
    a[-5, 0] = "bob"
    a.should == "bobhello"
  end
 
  it "overwrites and deletes characters if count is more than the length of other_str" do
    a = "hello"
    a[0, 4] = "x"
    a.should == "xo"
    a = "hello"
    a[0, 5] = "x"
    a.should == "x"
  end
 
  it "deletes characters if other_str is an empty string" do
    a = "hello"
    a[0, 2] = ""
    a.should == "llo"
  end
 
  it "deletes characters up to the maximum length of the existing string" do
    a = "hello"
    a[0, 6] = "x"
    a.should == "x"
    a = "hello"
    a[0, 100] = ""
    a.should == ""
  end
 
  it "appends other_str to the end of the string if idx == the length of the string" do
    a = "hello"
    a[5, 0] = "bob"
    a.should == "hellobob"
  end
  
  it "taints self if other_str is tainted" do
    a = "hello"
    a[0, 0] = "".taint
    a.tainted?.should == true
    
    a = "hello"
    a[1, 4] = "x".taint
    a.tainted?.should == true
  end
 
  it "raises an IndexError if |idx| is greater than the length of the string" do
    should_raise(IndexError) { "hello"[6, 0] = "bob" }
    should_raise(IndexError) { "hello"[-6, 0] = "bob" }
  end
 
  it "raises an IndexError if count < 0" do
    should_raise(IndexError) { "hello"[0, -1] = "bob" }
    should_raise(IndexError) { "hello"[1, -1] = "bob" }
  end
 
  it "raises a TypeError if other_str is a type other than String" do
    should_raise(TypeError) { "hello"[0, 2] = nil }
    should_raise(TypeError) { "hello"[0, 2] = :bob }
    should_raise(TypeError) { "hello"[0, 2] = 33 }
  end
end

# TODO: Add missing String#[]= specs:
#   String#[range] = obj
#   String#[re] = obj
#   String#[re, idx] = obj
#   String#[str] = obj

describe "String#capitalize" do
  it "returns a copy of self with the first character converted to uppercase and the remainder to lowercase" do
    "".capitalize.should == ""
    "h".capitalize.should == "H"
    "H".capitalize.should == "H"
    "hello".capitalize.should == "Hello"
    "HELLO".capitalize.should == "Hello"
    "123ABC".capitalize.should == "123abc"
  end

  it "taints resulting string when self is tainted" do
    "".taint.capitalize.tainted?.should == true
    "hello".taint.capitalize.tainted?.should == true
  end

  it "is locale insensitive (only upcases a-z and only downcases A-Z)" do
    "ÄÖÜ".capitalize.should == "ÄÖÜ"
    "ärger".capitalize.should == "ärger"
    "BÄR".capitalize.should == "BÄr"
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("hello").capitalize.class.should == MyString
    MyString.new("Hello").capitalize.class.should == MyString
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
    "H".capitalize!.should == nil
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
  
  # Broken in MRI 1.8.4
  it "doesn't consider non-ascii characters equal that aren't" do
    # -- Latin-1 --
    upper_a_tilde  = "\xC3"
    upper_a_umlaut = "\xC4"
    lower_a_tilde  = "\xE3"
    lower_a_umlaut = "\xE4"

    lower_a_tilde.casecmp(lower_a_umlaut).should_not == 0
    lower_a_umlaut.casecmp(lower_a_tilde).should_not == 0
    upper_a_tilde.casecmp(upper_a_umlaut).should_not == 0
    upper_a_umlaut.casecmp(upper_a_tilde).should_not == 0
    
    # -- UTF-8 --
    upper_a_tilde  = "\xC3\x83"
    upper_a_umlaut = "\xC3\x84"
    lower_a_tilde  = "\xC3\xA3"
    lower_a_umlaut = "\xC3\xA4"
    
    lower_a_tilde.casecmp(lower_a_umlaut).should_not == 0
    lower_a_umlaut.casecmp(lower_a_tilde).should_not == 0
    upper_a_tilde.casecmp(upper_a_umlaut).should_not == 0
    upper_a_umlaut.casecmp(upper_a_tilde).should_not == 0
  end
  
  it "doesn't do case mapping for non-ascii characters" do
    # -- Latin-1 --
    upper_a_tilde  = "\xC3"
    upper_a_umlaut = "\xC4"
    lower_a_tilde  = "\xE3"
    lower_a_umlaut = "\xE4"
    
    upper_a_tilde.casecmp(lower_a_tilde).should == -1
    upper_a_umlaut.casecmp(lower_a_umlaut).should == -1
    lower_a_tilde.casecmp(upper_a_tilde).should == 1
    lower_a_umlaut.casecmp(upper_a_umlaut).should == 1

    # -- UTF-8 --
    upper_a_tilde  = "\xC3\x83"
    upper_a_umlaut = "\xC3\x84"
    lower_a_tilde  = "\xC3\xA3"
    lower_a_umlaut = "\xC3\xA4"

    upper_a_tilde.casecmp(lower_a_tilde).should == -1
    upper_a_umlaut.casecmp(lower_a_umlaut).should == -1
    lower_a_tilde.casecmp(upper_a_tilde).should == 1
    lower_a_umlaut.casecmp(upper_a_umlaut).should == 1
  end
  
  it "ignores subclass differences" do
    str = "abcdef"
    my_str = MyString.new(str)
    
    str.casecmp(my_str).should == 0
    my_str.casecmp(str).should == 0
    my_str.casecmp(my_str).should == 0
  end
end

describe "String#center(length, padstr)" do
  it "returns a new string of specified length with self centered and padded with padstr" do
    "one".center(9, '.').should       == "...one..."
    "hello".center(20, '123').should  == "1231231hello12312312"
    "middle".center(13, '-').should   == "---middle----"

    "".center(1, "abcd").should == "a"
    "".center(2, "abcd").should == "aa"
    "".center(3, "abcd").should == "aab"
    "".center(4, "abcd").should == "abab"
    "".center(6, "xy").should == "xyxxyx"
    "".center(11, "12345").should == "12345123451"

    "|".center(2, "abcd").should == "|a"
    "|".center(3, "abcd").should == "a|a"
    "|".center(4, "abcd").should == "a|ab"
    "|".center(5, "abcd").should == "ab|ab"
    "|".center(6, "xy").should == "xy|xyx"
    "|".center(7, "xy").should == "xyx|xyx"
    "|".center(11, "12345").should == "12345|12345"
    "|".center(12, "12345").should == "12345|123451"

    "||".center(3, "abcd").should == "||a"
    "||".center(4, "abcd").should == "a||a"
    "||".center(5, "abcd").should == "a||ab"
    "||".center(6, "abcd").should == "ab||ab"
    "||".center(8, "xy").should == "xyx||xyx"
    "||".center(12, "12345").should == "12345||12345"
    "||".center(13, "12345").should == "12345||123451"
  end
  
  it "pads with whitespace if no padstr is given" do
    "two".center(5).should    == " two "
    "hello".center(20).should == "       hello        "
  end
  
  it "returns self if it's longer than or as long as the specified length" do
    "".center(0).should == ""
    "".center(-1).should == ""
    "hello".center(4).should == "hello"
    "hello".center(-1).should == "hello"
    "this".center(3).should == "this"
    "radiology".center(8, '-').should == "radiology"
  end

  it "taints result when self or padstr is tainted" do
    "x".taint.center(4).tainted?.should == true
    "x".taint.center(0).tainted?.should == true
    "".taint.center(0).tainted?.should == true
    "x".taint.center(4, "*").tainted?.should == true
    "x".center(4, "*".taint).tainted?.should == true
  end
  
  it "tries to convert length to an integer using to_int" do
    "_".center(3.8, "^").should == "^_^"
    
    obj = Object.new
    def obj.to_int() 3 end
      
    "_".center(obj, "o").should == "o_o"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 3)
    "_".center(obj, "~").should == "~_~"
  end
  
  it "raises a TypeError when length can't be converted to an integer" do
    should_raise(TypeError) { "hello".center("x") }
    should_raise(TypeError) { "hello".center("x", "y") }
    should_raise(TypeError) { "hello".center([]) }
    should_raise(TypeError) { "hello".center(Object.new) }
  end
  
  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "123" end
    
    "hello".center(20, padstr).should == "1231231hello12312312"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "k")

    "hello".center(7, obj).should == "khellok"
  end
  
  it "raises a TypeError when padstr can't be converted to a string" do
    should_raise(TypeError) { "hello".center(20, ?o) }
    should_raise(TypeError) { "hello".center(20, :llo) }
    should_raise(TypeError) { "hello".center(20, Object.new) }
  end
  
  it "raises an ArgumentError if padstr is empty" do
    should_raise(ArgumentError) { "hello".center(10, "") }
    should_raise(ArgumentError) { "hello".center(0, "") }
  end
  
  it "returns subclass instances when called on subclasses" do
    MyString.new("").center(10).class.should == MyString
    MyString.new("foo").center(10).class.should == MyString
    MyString.new("foo").center(10, MyString.new("x")).class.should == MyString
    
    "".center(10, MyString.new("x")).class.should == String
    "foo".center(10, MyString.new("x")).class.should == String
  end
end

describe "String#chomp(separator)" do
  it "returns a new string with the given record separator removed" do
    "hello".chomp("llo").should == "he"
    "hellollo".chomp("llo").should == "hello"
  end

  it "removes carriage return (except \\r) chars multiple times when separator is an empty string" do
    "".chomp("").should == ""
    "hello".chomp("").should == "hello"
    "hello\n".chomp("").should == "hello"
    "hello\nx".chomp("").should == "hello\nx"
    "hello\r\n".chomp("").should == "hello"
    "hello\r\n\r\n\n\n\r\n".chomp("").should == "hello"

    "hello\r".chomp("").should == "hello\r"
    "hello\n\r".chomp("").should == "hello\n\r"
    "hello\r\r\r\n".chomp("").should == "hello\r\r"
  end
  
  it "removes carriage return chars (\\n, \\r, \\r\\n) when separator is \\n" do
    "hello".chomp("\n").should == "hello"
    "hello\n".chomp("\n").should == "hello"
    "hello\r\n".chomp("\n").should == "hello"
    "hello\n\r".chomp("\n").should == "hello\n"
    "hello\r".chomp("\n").should == "hello"
    "hello \n there".chomp("\n").should == "hello \n there"
    "hello\r\n\r\n\n\n\r\n".chomp("\n").should == "hello\r\n\r\n\n\n"
    
    "hello\n\r".chomp("\r").should == "hello\n"
    "hello\n\r\n".chomp("\r\n").should == "hello\n"
  end
  
  it "returns self if the separator is nil" do
    "hello\n\n".chomp(nil).should == "hello\n\n"
  end
  
  it "returns an empty string when called on an empty string" do
    "".chomp("\n").should == ""
    "".chomp("\r").should == ""
    "".chomp("").should == ""
    "".chomp(nil).should == ""
  end
  
  it "uses $/ as the separator when none is given" do
    ["", "x", "x\n", "x\r", "x\r\n", "x\n\r\r\n", "hello"].each do |str|
      ["", "llo", "\n", "\r", nil].each do |sep|
        begin
          expected = str.chomp(sep)

          old_rec_sep, $/ = $/, sep

          str.chomp.should == expected
        ensure
          $/ = old_rec_sep
        end
      end
    end
  end
  
  it "taints result when self is tainted" do
    "hello".taint.chomp("llo").tainted?.should == true
    "hello".taint.chomp("").tainted?.should == true
    "hello".taint.chomp(nil).tainted?.should == true
    "hello".taint.chomp.tainted?.should == true
    "hello\n".taint.chomp.tainted?.should == true
    
    "hello".chomp("llo".taint).tainted?.should == false
  end
  
  it "tries to convert separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() "llo" end
    
    "hello".chomp(separator).should == "he"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "k")

    "hark".chomp(obj).should == "har"
  end
  
  it "raises a TypeError if separator can't be converted to a string" do
    should_raise(TypeError) { "hello".chomp(?o) }
    should_raise(TypeError) { "hello".chomp(:llo) }
    should_raise(TypeError) { "hello".chomp(Object.new) }
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("hello\n").chomp.class.should == MyString
    MyString.new("hello").chomp.class.should == MyString
    MyString.new("").chomp.class.should == MyString
  end
end

describe "String#chomp!(seperator)" do
  it "modifies self in place and returns self" do
    s = "one\n"
    s.chomp!.equal?(s).should == true
    s.should == "one"
    
    t = "two\r\n"
    t.chomp!.equal?(t).should == true
    t.should == "two"
    
    u = "three\r"
    u.chomp!
    u.should == "three"
    
    v = "four\n\r"
    v.chomp!
    v.should == "four\n"
    
    w = "five\n\n"
    w.chomp!(nil)
    w.should == "five\n\n"
    
    x = "six"
    x.chomp!("ix")
    x.should == "s"
    
    y = "seven\n\n\n\n"
    y.chomp!("")
    y.should == "seven"
  end
  
  it "returns nil if no modifications were made" do
     v = "four"
     v.chomp!.should == nil
     v.should == "four"
    
    "".chomp!.should == nil
    "line".chomp!.should == nil
    
    "hello\n".chomp!("x").should == nil
    "hello".chomp!("").should == nil
    "hello".chomp!(nil).should == nil
  end

  it "raises a TypeError when self is frozen" do
    a = "string\n\r"
    a.freeze

    should_raise(TypeError) { a.chomp! }

    a.chomp!(nil) # ok, no change
    a.chomp!("x") # ok, no change
  end
end

describe "String#chop" do
  it "returns a new string with the last character removed" do
    "hello\n".chop.should == "hello"
    "hello\x00".chop.should == "hello"
    "hello".chop.should == "hell"
    
    ori_str = ""
    256.times { |i| ori_str << i }
    
    str = ori_str
    256.times do |i|
      str = str.chop
      str.should == ori_str[0, 255 - i]
    end
  end
  
  it "removes both characters if the string ends with \\r\\n" do
    "hello\r\n".chop.should == "hello"
    "hello\r\n\r\n".chop.should == "hello\r\n"
    "hello\n\r".chop.should == "hello\n"
    "hello\n\n".chop.should == "hello\n"
    "hello\r\r".chop.should == "hello\r"
    
    "\r\n".chop.should == ""
  end
  
  it "returns an empty string when applied to an empty string" do
    "".chop.should == ""
  end

  it "taints result when self is tainted" do
    "hello".taint.chop.tainted?.should == true
    "".taint.chop.tainted?.should == true
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("hello\n").chop.class.should == MyString
    MyString.new("hello").chop.class.should == MyString
    MyString.new("").chop.class.should == MyString
  end
end

describe "String#chop!" do
  it "behaves just like chop, but in-place" do
    ["hello\n", "hello\r\n", "hello", ""].each do |base|
      str = base.dup
      str.chop!
      
      str.should == base.chop
    end
  end

  it "returns self if modifications were made" do
    ["hello", "hello\r\n"].each do |s|
      s.chop!.equal?(s).should == true
    end
  end

  it "returns nil when called on an empty string" do
    "".chop!.should == nil
  end
  
  it "raises a TypeError when self is frozen" do
    a = "string\n\r"
    a.freeze
    should_raise(TypeError) { a.chop! }

    a = ""
    a.freeze
    a.chop! # ok, no change
  end
end

describe "String#concat(other)" do
  it "is an alias of String#<<" do
    ["xyz", 42].each do |arg|
      (a = "abc") << arg
      (b = "abc").concat(arg).equal?(b).should == true
      
      a.should == b
    end
  end

  it "taints self if other is tainted" do
    a = "x"
    (a << "".taint).tainted?.should == true

    a = "x"
    (a << "y".taint).tainted?.should == true
  end

  it "doesn't use fixnum % 256" do
    should_raise(TypeError) { "".concat(256 * 3 + 64) }
    should_raise(TypeError) { "".concat(-200) }
  end
  
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    should_raise(TypeError) { s.concat("") }
    should_raise(TypeError) { s.concat("foo") }
    should_raise(TypeError) { s.concat(0) }
  end
end

describe "String#count(*sets)" do
  it "counts occurrences of chars from the intersection of the specified sets" do
    s = "hello\nworld\x00\x00"

    s.count(s).should == s.size
    s.count("lo").should == 5
    s.count("eo").should == 3
    s.count("l").should == 3
    s.count("\n").should == 1
    s.count("\x00").should == 2
    
    s.count("").should == 0
    "".count("").should == 0

    s.count("l", "lo").should == s.count("l")
    s.count("l", "lo", "o").should == s.count("")
    s.count("helo", "hel", "h").should == s.count("h")
    s.count("helo", "", "x").should == 0
  end

  it "raises ArgumentError when given no arguments" do
    should_raise(ArgumentError) { "hell yeah".count }
  end

  it "negates sets starting with ^" do
    s = "^hello\nworld\x00\x00"
    
    s.count("^").should == 1 # no negation, counts ^

    s.count("^leh").should == 9
    s.count("^o").should == 12

    s.count("helo", "^el").should == s.count("ho")
    s.count("aeiou", "^e").should == s.count("aiou")
    
    "^_^".count("^^").should == 1
    "oa^_^o".count("a^").should == 3
  end

  it "counts all chars in a sequence" do
    s = "hel-[()]-lo012^"
    
    s.count("\x00-\xFF").should == s.size
    s.count("ej-m").should == 3
    s.count("e-h").should == 2

    # no sequences
    s.count("-").should == 2
    s.count("e-").should == s.count("e") + s.count("-")
    s.count("-h").should == s.count("h") + s.count("-")

    s.count("---").should == s.count("-")
    
    # see an ASCII table for reference
    s.count("--2").should == s.count("-./012")
    s.count("(--").should == s.count("()*+,-")
    s.count("A-a").should == s.count("A-Z[\\]^_`a")
    
    # empty sequences (end before start)
    s.count("h-e").should == 0
    s.count("^h-e").should == s.size

    # negated sequences
    s.count("^e-h").should == s.size - s.count("e-h")
    s.count("^^-^").should == s.size - s.count("^")
    s.count("^---").should == s.size - s.count("-")

    "abcdefgh".count("a-ce-fh").should == 6
    "abcdefgh".count("he-fa-c").should == 6
    "abcdefgh".count("e-fha-c").should == 6

    "abcde".count("ac-e").should == 4
    "abcde".count("^ac-e").should == 1
  end

  it "tries to convert each set arg to a string using to_str" do
    other_string = Object.new
    def other_string.to_str() "lo" end

    other_string2 = Object.new
    def other_string2.to_str() "o" end

    s = "hello world"
    s.count(other_string, other_string2).should == s.count("o")
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "k")
    s = "hacker kimono"
    s.count(obj).should == s.count("k")
  end
 
  it "raises a TypeError when a set arg can't be converted to a string" do
    should_raise(TypeError) { "hello world".count(?o) }
    should_raise(TypeError) { "hello world".count(:o) }
    should_raise(TypeError) { "hello world".count(Object.new) }
  end
end

describe "String#crypt" do
  # Note: MRI's documentation just says that the C stdlib function crypt() is
  # called.
  #
  # I'm not sure if crypt() is guaranteed to produce the same result across
  # different platforms. It seems that there is one standard UNIX implementation
  # of crypt(), but that alternative implementations are possible. See
  # http://www.unix.org.ua/orelly/networking/puis/ch08_06.htm
  it "returns a cryptographic hash of self by applying the UNIX crypt algorithm with the specified salt" do
    "".crypt("aa").should == "aaQSqAReePlq6"
    "nutmeg".crypt("Mi").should == "MiqkFWCm1fNJI"
    "ellen1".crypt("ri").should == "ri79kNd7V6.Sk"
    "Sharon".crypt("./").should == "./UY9Q7TvYJDg"
    "norahs".crypt("am").should == "amfIADT2iqjA."
    "norahs".crypt("7a").should == "7azfT5tIdyh0I"
    
    # Only uses first 8 chars of string
    "01234567".crypt("aa").should == "aa4c4gpuvCkSE"
    "012345678".crypt("aa").should == "aa4c4gpuvCkSE"
    "0123456789".crypt("aa").should == "aa4c4gpuvCkSE"
    
    # Only uses first 2 chars of salt
    "hello world".crypt("aa").should == "aayPz4hyPS1wI"
    "hello world".crypt("aab").should == "aayPz4hyPS1wI"
    "hello world".crypt("aabc").should == "aayPz4hyPS1wI"
    
    # Maps null bytes in salt to ..
    "hello".crypt("\x00\x00").should == "..dR0/E99ehpU"
  end
  
  it "raises an ArgumentError when the salt is shorter than two characters" do
    should_raise(ArgumentError) { "hello".crypt("") }
    should_raise(ArgumentError) { "hello".crypt("f") }
  end

  it "converts the salt arg to a string via to_str" do
    obj = Object.new
    def obj.to_str() "aa" end
    
    "".crypt(obj).should == "aaQSqAReePlq6"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "aa")
    "".crypt(obj).should == "aaQSqAReePlq6"
  end

  it "raises a type error when the salt arg can't be converted to a string" do
    should_raise(TypeError) { "".crypt(5) }
    should_raise(TypeError) { "".crypt(Object.new) }
  end
  
  it "taints the result if either salt or self is tainted" do
    tainted_salt = "aa"
    tainted_str = "hello"
    
    tainted_salt.taint
    tainted_str.taint
    
    "hello".crypt("aa").tainted?.should == false
    tainted_str.crypt("aa").tainted?.should == true
    "hello".crypt(tainted_salt).tainted?.should == true
    tainted_str.crypt(tainted_salt).tainted?.should == true
  end
  
  it "doesn't return subclass instances" do
    MyString.new("hello").crypt("aa").class.should == String
    "hello".crypt(MyString.new("aa")).class.should == String
    MyString.new("hello").crypt(MyString.new("aa")).class.should == String
  end
end

describe "String#delete(*sets)" do
  it "returns a new string with the chars from the intersection of sets removed" do
    s = "hello"
    s.delete("lo").should == "he"
    s.should == "hello"
    
    "hello".delete("l", "lo").should == "heo"
    
    "hell yeah".delete("").should == "hell yeah"
  end
  
  it "raises ArgumentError when given no arguments" do
    should_raise(ArgumentError) { "hell yeah".delete }
  end

  it "negates sets starting with ^" do
    "hello".delete("aeiou", "^e").should == "hell"
    "hello".delete("^leh").should == "hell"
    "hello".delete("^o").should == "o"
    "hello".delete("^").should == "hello"
    "^_^".delete("^^").should == "^^"
    "oa^_^o".delete("a^").should == "o_o"
  end

  it "deletes all chars in a sequence" do
    "hello".delete("\x00-\xFF").should == ""
    "hello".delete("ej-m").should == "ho"
    "hello".delete("e-h").should == "llo"
    "hel-lo".delete("e-").should == "hllo"
    "hel-lo".delete("-h").should == "ello"
    "hel-lo".delete("---").should == "hello"
    "hel-012".delete("--2").should == "hel"
    "hel-()".delete("(--").should == "hel"
    "hello".delete("h-e").should == "hello"
    "hello".delete("^h-e").should == ""
    "hello".delete("^e-h").should == "he"
    "hello^".delete("^^-^").should == "^"
    "hel--lo".delete("^---").should == "--"

    "abcdefgh".delete("a-ce-fh").should == "dg"
    "abcdefgh".delete("he-fa-c").should == "dg"
    "abcdefgh".delete("e-fha-c").should == "dg"
    
    "abcde".delete("ac-e").should == "b"
    "abcde".delete("^ac-e").should == "acde"
    
    "ABCabc[]".delete("A-a").should == "bc"
  end
  
  it "taints result when self is tainted" do
    "hello".taint.delete("e").tainted?.should == true
    "hello".taint.delete("a-z").tainted?.should == true

    "hello".delete("e".taint).tainted?.should == false
  end

  it "tries to convert each set arg to a string using to_str" do
    other_string = Object.new
    def other_string.to_str() "lo" end
    
    other_string2 = Object.new
    def other_string2.to_str() "o" end
    
    "hello world".delete(other_string, other_string2).should == "hell wrld"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "o")
    "hello world".delete(obj).should == "hell wrld"
  end
  
  it "raises a TypeError when one set arg can't be converted to a string" do
    should_raise(TypeError) { "hello world".delete(?o) }
    should_raise(TypeError) { "hello world".delete(:o) }
    should_raise(TypeError) { "hello world".delete(Object.new) }
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("oh no!!!").delete("!").class.should == MyString
  end
end

describe "String#delete!(*sets)" do
  it "modifies self in place and returns self" do
    a = "hello"
    a.delete!("aeiou", "^e").equal?(a).should == true
    a.should == "hell"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.delete!("z").should == nil
    a.should == "hello"
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze

    should_raise(TypeError) { a.delete!("") }
    should_raise(TypeError) { a.delete!("aeiou", "^e") }
  end
end

describe "String#downcase" do
  it "returns a copy of self with all uppercase letters downcased" do
    "hELLO".downcase.should == "hello"
    "hello".downcase.should == "hello"
  end
  
  it "is locale insensitive (only replacing A-Z)" do
    "ÄÖÜ".downcase.should == "ÄÖÜ"

    str = Array.new(256) { |c| c.chr }.join
    expected = Array.new(256) do |i|
      c = i.chr
      c.between?("A", "Z") ? c.downcase : c
    end.join
    
    str.downcase.should == expected
  end
  
  it "taints result when self is tainted" do
    "".taint.downcase.tainted?.should == true
    "x".taint.downcase.tainted?.should == true
    "X".taint.downcase.tainted?.should == true
  end
  
  it "returns a subclass instance for subclasses" do
    MyString.new("FOObar").downcase.class.should == MyString
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

  it "taints result when self is tainted" do
    "".taint.dump.tainted?.should == true
    "x".taint.dump.tainted?.should == true
  end
  
  it "returns a subclass instance for subclasses" do
    MyString.new("hi!").dump.class.should == MyString
  end
end

describe "String#each(separator)" do
  it "splits self using the supplied record separator and passes each substring to the block" do
    a = []
    "one\ntwo\r\nthree".each("\n") { |s| a << s }
    a.should == ["one\n", "two\r\n", "three"]
    
    b = []
    "hello\nworld".each('l') { |s| b << s }
    b.should == [ "hel", "l", "o\nworl", "d" ]
    
    c = []
    "hello\n\n\nworld".each("\n") { |s| c << s }
    c.should == ["hello\n", "\n", "\n", "world"]
  end
  
  it "taints substrings that are passed to the block if self is tainted" do
    "one\ntwo\r\nthree".taint.each { |s| s.tainted?.should == true }

    "x.y.".each(".".taint) { |s| s.tainted?.should == false }
  end
  
  it "passes self as a whole to the block if the separator is nil" do
    a = []
    "one\ntwo\r\nthree".each(nil) { |s| a << s }
    a.should == ["one\ntwo\r\nthree"]
  end
  
  it "appends multiple successive newlines together when the separator is an empty string" do
    a = []
    "hello\nworld\n\n\nand\nuniverse\n\n\n\n\n".each('') { |s| a << s }
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
          str.each(sep) { |x| expected << x }

          old_rec_sep, $/ = $/, sep

          actual = []
          str.each { |x| actual << x }

          actual.should == expected
        ensure
          $/ = old_rec_sep
        end
      end
    end
  end
  
  it "yields subclass instances for subclasses" do
    a = []
    MyString.new("hello\nworld").each { |s| a << s.class }
    a.should == [MyString, MyString]
  end
  
  it "returns self" do
    s = "hello\nworld"
    (s.each {}).equal?(s).should == true
  end

  it "tries to convert the separator to a string using to_str" do
    separator = Object.new
    def separator.to_str() 'l' end
    
    a = []
    "hello\nworld".each(separator) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "l")
    
    a = []
    "hello\nworld".each(obj) { |s| a << s }
    a.should == [ "hel", "l", "o\nworl", "d" ]
  end
  
  it "raises a RuntimeError if the string is modified while substituting" do
    str = "hello\nworld"
    should_raise(RuntimeError) { str.each_line { str[0] = 'x' } }
  end
  
  it "raises a TypeError when the separator can't be converted to a string" do
    should_raise(TypeError) { "hello world".each(?o) }
    should_raise(TypeError) { "hello world".each(:o) }
    should_raise(TypeError) { "hello world".each(Object.new) }
  end
end

describe "String#each_byte" do
  it "passes each byte in self to the given block" do
    a = []
    "hello\x00".each_byte { |c| a << c }
    a.should == [104, 101, 108, 108, 111, 0]
  end

  it "keeps iterating from the old position (to new string end) when self changes" do
    r = ""
    s = "hello world"
    s.each_byte do |c|
      r << c
      s.insert(0, "<>") if r.size < 3
    end
    r.should == "h><>hello world"

    r = ""
    s = "hello world"
    s.each_byte { |c| s.slice!(-1); r << c }
    r.should == "hello "

    r = ""
    s = "hello world"
    s.each_byte { |c| s.slice!(0); r << c }
    r.should == "hlowrd"

    r = ""
    s = "hello world"
    s.each_byte { |c| s.slice!(0..-1); r << c }
    r.should == "h"
  end
  
  it "returns self" do
    s = "hello"
    (s.each_byte {}).equal?(s).should == true
  end
end

describe "String#each_line(separator)" do
  it "is an alias of String#each" do
    [
      "", "x", "x\ny", "x\ry", "x\r\ny", "x\n\r\r\ny",
      "hello hullo bello", MyString.new("hello\nworld")
    ].each do |str|
      [
        [""], ["llo"], ["\n"], ["\r"], [nil],
        [], [MyString.new("\n")]
      ].each do |args|
        begin
          expected = []
          str.each(*args) { |x| expected << x << x.class }

          actual = []
          actual_cls = []
          r = str.each_line(*args) { |x| actual << x << x.class }
          r.equal?(str).should == true

          actual.should == expected
        end
      end
    end
  end
end

describe "String#empty?" do
  it "returns true if the string has a length of zero" do
    "hello".empty?.should == false
    " ".empty?.should == false
    "\x00".empty?.should == false
    "".empty?.should == true
    MyString.new("").empty?.should == true
  end
end

describe "String#eql?" do
  it "returns true if two strings have the same length and content" do
    "hello".eql?("hello").should == true
    "hello".eql?("hell").should == false
    "1".eql?(1).should == false
    
    MyString.new("hello").eql?("hello").should == true
    "hello".eql?(MyString.new("hello")).should == true
    MyString.new("hello").eql?(MyString.new("hello")).should == true
  end
end

describe "String#gsub(pattern, replacement)" do
  it "returns a copy of self with all occurrences of pattern replaced with replacement" do
    "hello".gsub(/[aeiou]/, '*').should == "h*ll*"

    str = "hello homely world. hah!"
    str.gsub(/\Ah\S+\s*/, "huh? ").should == "huh? homely world. hah!"

    "hello".gsub(//, ".").should == ".h.e.l.l.o."
  end
  
  it "ignores a block if supplied" do
    "food".gsub(/f/, "g") { "w" }.should == "good"
  end

  it "supports \\G which matches at the beginning of the remaining (non-matched) string" do
    str = "hello homely world. hah!"
    str.gsub(/\Gh\S+\s*/, "huh? ").should == "huh? huh? world. hah!"
  end
  
  it "supports /i for ignoring case" do
    str = "Hello. How happy are you?"
    str.gsub(/h/i, "j").should == "jello. jow jappy are you?"
    str.gsub(/H/i, "j").should == "jello. jow jappy are you?"
  end
  
  it "doesn't interpret regexp metacharacters if pattern is a string" do
    "12345".gsub('\d', 'a').should == "12345"
    '\d'.gsub('\d', 'a').should == "a"
  end
  
  it "replaces \\1 sequences with the regexp's corresponding capture" do
    str = "hello"
    
    str.gsub(/([aeiou])/, '<\1>').should == "h<e>ll<o>"
    str.gsub(/(.)/, '\1\1').should == "hheelllloo"

    str.gsub(/.(.?)/, '<\0>(\1)').should == "<he>(e)<ll>(l)<o>()"

    str.gsub(/.(.)+/, '\1').should == "o"

    str = "ABCDEFGHIJKLabcdefghijkl"
    re = /#{"(.)" * 12}/
    str.gsub(re, '\1').should == "Aa"
    str.gsub(re, '\9').should == "Ii"
    # Only the first 9 captures can be accessed in MRI
    str.gsub(re, '\10').should == "A0a0"
  end

  it "treats \\1 sequences without corresponding captures as empty strings" do
    str = "hello!"
    
    str.gsub("", '<\1>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub("h", '<\1>').should == "<>ello!"

    str.gsub(//, '<\1>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub(/./, '\1\2\3').should == ""
    str.gsub(/.(.{20})?/, '\1').should == ""
  end

  it "replaces \\& and \\0 with the complete match" do
    str = "hello!"
    
    str.gsub("", '<\0>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub("", '<\&>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub("he", '<\0>').should == "<he>llo!"
    str.gsub("he", '<\&>').should == "<he>llo!"
    str.gsub("l", '<\0>').should == "he<l><l>o!"
    str.gsub("l", '<\&>').should == "he<l><l>o!"
    
    str.gsub(//, '<\0>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub(//, '<\&>').should == "<>h<>e<>l<>l<>o<>!<>"
    str.gsub(/../, '<\0>').should == "<he><ll><o!>"
    str.gsub(/../, '<\&>').should == "<he><ll><o!>"
    str.gsub(/(.)./, '<\0>').should == "<he><ll><o!>"
  end

  it "replaces \\` with everything before the current match" do
    str = "hello!"
    
    str.gsub("", '<\`>').should == "<>h<h>e<he>l<hel>l<hell>o<hello>!<hello!>"
    str.gsub("h", '<\`>').should == "<>ello!"
    str.gsub("l", '<\`>').should == "he<he><hel>o!"
    str.gsub("!", '<\`>').should == "hello<hello>"
    
    str.gsub(//, '<\`>').should == "<>h<h>e<he>l<hel>l<hell>o<hello>!<hello!>"
    str.gsub(/../, '<\`>').should == "<><he><hell>"
  end

  it "replaces \\' with everything after the current match" do
    str = "hello!"
    
    str.gsub("", '<\\\'>').should == "<hello!>h<ello!>e<llo!>l<lo!>l<o!>o<!>!<>"
    str.gsub("h", '<\\\'>').should == "<ello!>ello!"
    str.gsub("ll", '<\\\'>').should == "he<o!>o!"
    str.gsub("!", '<\\\'>').should == "hello<>"
    
    str.gsub(//, '<\\\'>').should == "<hello!>h<ello!>e<llo!>l<lo!>l<o!>o<!>!<>"
    str.gsub(/../, '<\\\'>').should == "<llo!><o!><>"
  end
  
  it "replaces \\+ with the last paren that actually matched" do
    str = "hello!"
    
    str.gsub(/(.)(.)/, '\+').should == "el!"
    str.gsub(/(.)(.)+/, '\+').should == "!"
    str.gsub(/(.)()/, '\+').should == ""
    str.gsub(/(.)(.{20})?/, '<\+>').should == "<h><e><l><l><o><!>"

    str = "ABCDEFGHIJKLabcdefghijkl"
    re = /#{"(.)" * 12}/
    str.gsub(re, '\+').should == "Ll"
  end

  it "treats \\+ as an empty string if there was no captures" do
    "hello!".gsub(/./, '\+').should == ""
  end
  
  it "maps \\\\ in replacement to \\" do
    "hello".gsub(/./, '\\\\').should == '\\' * 5
  end

  it "leaves unknown \\x escapes in replacement untouched" do
    "hello".gsub(/./, '\\x').should == '\\x' * 5
    "hello".gsub(/./, '\\y').should == '\\y' * 5
  end

  it "leaves \\ at the end of replacement untouched" do
    "hello".gsub(/./, 'hah\\').should == 'hah\\' * 5
  end
  
  it "taints the result if the original string or replacement is tainted" do
    hello = "hello"
    hello_t = "hello"
    a = "a"
    a_t = "a"
    empty = ""
    empty_t = ""
    
    hello_t.taint; a_t.taint; empty_t.taint
    
    hello_t.gsub(/./, a).tainted?.should == true
    hello_t.gsub(/./, empty).tainted?.should == true

    hello.gsub(/./, a_t).tainted?.should == true
    hello.gsub(/./, empty_t).tainted?.should == true
    hello.gsub(//, empty_t).tainted?.should == true
    
    hello.gsub(//.taint, "foo").tainted?.should == false
  end

  it "tries to convert pattern to a string using to_str" do
    pattern = Object.new
    def pattern.to_str() "." end
    
    "hello.".gsub(pattern, "!").should == "hello!"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => ".")

    "hello.".gsub(obj, "!").should == "hello!"
  end

  it "raises a TypeError when pattern can't be converted to a string" do
    should_raise(TypeError) { "hello".gsub(:woot, "x") }
    should_raise(TypeError) { "hello".gsub(?e, "x") }
  end
  
  it "tries to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".gsub(/hello/, replacement).should == "hello_replacement"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "ok")
    "hello".gsub(/hello/, obj).should == "ok"
  end
  
  it "raises a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) { "hello".gsub(/[aeiou]/, :woot) }
    should_raise(TypeError) { "hello".gsub(/[aeiou]/, ?f) }
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("").gsub(//, "").class.should == MyString
    MyString.new("").gsub(/foo/, "").class.should == MyString
    MyString.new("foo").gsub(/foo/, "").class.should == MyString
    MyString.new("foo").gsub("foo", "").class.should == MyString
  end
  
  it "sets $~ to MatchData of last match and nil when there's none" do
    'hello.'.gsub('hello', 'x')
    $~[0].should == 'hello'

    'hello.'.gsub('not', 'x')
    $~.should == nil

    'hello.'.gsub(/.(.)/, 'x')
    $~[0].should == 'o.'

    'hello.'.gsub(/not/, 'x')
    $~.should == nil
  end
end

describe "String#gsub(pattern) { block }" do
  it "returns a copy of self with all occurrences of pattern replaced with the block's return value" do
    "hello".gsub(/./) { |s| s.succ + ' ' }.should == "i f m m p "
    "hello!".gsub(/(.)(.)/) { |*a| a.inspect }.should == '["he"]["ll"]["o!"]'
  end
  
  it "sets $~ for access from the block" do
    str = "hello"
    str.gsub(/([aeiou])/) { "<#{$~[1]}>" }.should == "h<e>ll<o>"
    str.gsub(/([aeiou])/) { "<#{$1}>" }.should == "h<e>ll<o>"
    str.gsub("l") { "<#{$~[0]}>" }.should == "he<l><l>o"
    
    offsets = []
    
    str.gsub(/([aeiou])/) do
       md = $~
       md.string.should == str
       offsets << md.offset(0)
       str
    end.should == "hhellollhello"
    
    offsets.should == [[1, 2], [4, 5]]
  end
  
  it "restores $~ after leaving the block" do
    [/./, "l"].each do |pattern|
      old_md = nil
      "hello".gsub(pattern) do
        old_md = $~
        "ok".match(/./)
        "x"
      end
    
      $~.should == old_md
      $~.string.should == "hello"
    end
  end
  
  it "sets $~ to MatchData of last match and nil when there's none for access from outside" do
    'hello.'.gsub('l') { 'x' }
    $~.begin(0).should == 3
    $~[0].should == 'l'

    'hello.'.gsub('not') { 'x' }
    $~.should == nil

    'hello.'.gsub(/.(.)/) { 'x' }
    $~[0].should == 'o.'

    'hello.'.gsub(/not/) { 'x' }
    $~.should == nil
  end
  
  it "raises a RuntimeError if the string is modified while substituting" do
    str = "hello"
    should_raise(RuntimeError) { str.gsub(//) { str[0] = 'x' } }
  end
  
  it "doesn't interpolate special sequences like \\1 for the block's return value" do
    repl = '\& \0 \1 \` \\\' \+ \\\\ foo'
    "hello".gsub(/(.+)/) { repl }.should == repl
  end
  
  it "converts the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".gsub(/hello/) { replacement }.should == "hello_replacement"
    
    obj = Object.new
    class << obj; undef :to_s; end
    obj.should_receive(:method_missing, :with => [:to_s], :returning => "ok")
    
    "hello".gsub(/.+/) { obj }.should == "ok"
  end
  
  it "taints the result if the original string or replacement is tainted" do
    hello = "hello"
    hello_t = "hello"
    a = "a"
    a_t = "a"
    empty = ""
    empty_t = ""
    
    hello_t.taint; a_t.taint; empty_t.taint
    
    hello_t.gsub(/./) { a }.tainted?.should == true
    hello_t.gsub(/./) { empty }.tainted?.should == true

    hello.gsub(/./) { a_t }.tainted?.should == true
    hello.gsub(/./) { empty_t }.tainted?.should == true
    hello.gsub(//) { empty_t }.tainted?.should == true
    
    hello.gsub(//.taint) { "foo" }.tainted?.should == false
  end  
end

describe "String#gsub!(pattern, replacement)" do
  it "modifies self in place and returns self" do
    a = "hello"
    a.gsub!(/[aeiou]/, '*').equal?(a).should == true
    a.should == "h*ll*"
  end

  it "taints self if replacement is tainted" do
    a = "hello"
    a.gsub!(/./.taint, "foo").tainted?.should == false
    a.gsub!(/./, "foo".taint).tainted?.should == true
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.gsub!(/z/, '*').should == nil
    a.gsub!(/z/, 'z').should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    s.gsub!(/ROAR/, "x") # ok
    should_raise(TypeError) { s.gsub!(/e/, "e") }
    should_raise(TypeError) { s.gsub!(/[aeiou]/, '*') }
  end
end

describe "String#gsub!(pattern) { block }" do
  it "modifies self in place and returns self" do
    a = "hello"
    a.gsub!(/[aeiou]/) { '*' }.equal?(a).should == true
    a.should == "h*ll*"
  end

  it "taints self if block's result is tainted" do
    a = "hello"
    a.gsub!(/./.taint) { "foo" }.tainted?.should == false
    a.gsub!(/./) { "foo".taint }.tainted?.should == true
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.gsub!(/z/) { '*' }.should == nil
    a.gsub!(/z/) { 'z' }.should == nil
    a.should == "hello"
  end
  
  # MRI 1.8 raises a RuntimeError here which is inconsistent
  # with the non-block form of gsub! (and everything else)
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    s.gsub!(/ROAR/) { "x" } # ok
    should_raise(TypeError) { s.gsub!(/e/) { "e" } }
    should_raise(TypeError) { s.gsub!(/[aeiou]/) { '*' } }
  end
end

describe "String#hash" do
  it "returns a hash based on a string's length and content" do
    "abc".hash.should == "abc".hash
    "abc".hash.should_not == "cba".hash
  end
end

# TODO: Move actual results to String#to_int() and spec in terms of it
describe "String#hex" do
  it "treats leading characters of self as a string of hex digits" do
    "0a".hex.should == 10
    "0o".hex.should == 0
    "0x".hex.should == 0
    "A_BAD_BABE".hex.should == 0xABADBABE
    "a__b".hex.should == 0xab
    "0b1010".hex.should == "b1010".hex
    "0d500".hex.should == "d500".hex
    "abcdefG".hex.should == 0xabcdef
  end
  
  it "takes an optional sign" do
    "-1234".hex.should == -4660
    "+1234".hex.should == 4660
  end
  
  it "takes an optional 0x" do
    "0x0a".hex.should == 10
    "0x-1".hex.should == 0
    "0x0x1".hex.should == 0
  end
  
  it "returns 0 on error" do
    "".hex.should == 0
    "+-5".hex.should == 0
    "wombat".hex.should == 0
  end
end

describe "String#include?(other_str)" do
  it "returns true if self contains other_str" do
    "hello".include?("lo").should == true
    "hello".include?("ol").should == false
  end
  
  it "ignores subclass differences" do
    "hello".include?(MyString.new("lo")).should == true
    MyString.new("hello").include?("lo").should == true
    MyString.new("hello").include?(MyString.new("lo")).should == true
  end
  
  it "tries to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "lo" end
    "hello".include?(other).should == true

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "o")
    "hello".include?(obj).should == true
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) { "hello".include?(:lo) }
    should_raise(TypeError) { "hello".include?(Object.new) }
  end
end

describe "String#include?(fixnum)" do
  it "returns true if self contains the given char" do
    "hello".include?(?h).should == true
    "hello".include?(?z).should == false
    "hello".include?(0).should == false
  end
  
  it "uses fixnum % 256" do
    "hello".include?(?h + 256 * 3).should == true
  end
  
  it "doesn't try to convert fixnum to an Integer using to_int" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    should_raise(TypeError) { "hello".include?(obj) }
  end
end

describe "String#index(obj [, start_offset])" do
  it "raises a TypeError if obj isn't a String, Fixnum or Regexp" do
    should_raise(TypeError) { "hello".index(:sym) }    
    should_raise(TypeError) { "hello".index(Object.new) }
  end

  it "doesn't try to convert obj to an Integer via to_int" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    should_raise(TypeError) { "hello".index(obj) }
  end

  it "tries to convert obj to a string via to_str" do
    obj = Object.new
    obj.should_receive(:to_str, :returning => "lo")
    "hello".index(obj).should == "hello".index("lo")
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "o")
    "hello".index(obj).should == "hello".index("o")
  end
end

describe "String#index(fixnum [, start_offset])" do
  it "returns the index of the first occurrence of the given character" do
    "hello".index(?e).should == 1
    "hello".index(?l).should == 2
  end
  
  it "doesn't use fixnum % 256" do
    "hello".index(?e + 256 * 3).should == nil
    "hello".index(-(256 - ?e)).should == nil
  end
  
  it "starts the search at the given offset" do
    "blablabla".index(?b, 0).should == 0
    "blablabla".index(?b, 1).should == 3
    "blablabla".index(?b, 2).should == 3
    "blablabla".index(?b, 3).should == 3
    "blablabla".index(?b, 4).should == 6
    "blablabla".index(?b, 5).should == 6
    "blablabla".index(?b, 6).should == 6

    "blablabla".index(?a, 0).should == 2
    "blablabla".index(?a, 2).should == 2
    "blablabla".index(?a, 3).should == 5
    "blablabla".index(?a, 4).should == 5
    "blablabla".index(?a, 5).should == 5
    "blablabla".index(?a, 6).should == 8
    "blablabla".index(?a, 7).should == 8
    "blablabla".index(?a, 8).should == 8
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    [?a, ?b].each do |needle|
      (-str.length .. -1).each do |offset|
        str.index(needle, offset).should ==
        str.index(needle, offset + str.length)
      end
    end

    "blablabla".index(?b, -9).should == 0
  end
  
  it "returns nil if offset + self.length is < 0 for negative offsets" do
    "blablabla".index(?b, -10).should == nil
    "blablabla".index(?b, -20).should == nil
  end
  
  it "returns nil if the character isn't found" do
    "hello".index(0).should == nil
    
    "hello".index(?H).should == nil
    "hello".index(?z).should == nil
    "hello".index(?e, 2).should == nil

    "blablabla".index(?b, 7).should == nil
    "blablabla".index(?b, 10).should == nil

    "blablabla".index(?a, 9).should == nil
    "blablabla".index(?a, 20).should == nil
  end
  
  it "converts start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 1)
    "ROAR".index(?R, obj).should == 3
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    "ROAR".index(?R, obj).should == 3
  end
end

describe "String#index(substring [, start_offset])" do
  it "behaves the same as String#index(char) for one-character strings" do
    ["blablabla", "hello cruel world...!"].each do |str|
      str.split("").uniq.each do |str|
        chr = str[0]
        str.index(str).should == str.index(chr)
        
        0.upto(str.size + 1) do |start|
          str.index(str, start).should == str.index(chr, start)
        end
        
        (-str.size - 1).upto(-1) do |start|
          str.index(str, start).should == str.index(chr, start)
        end
      end
    end
  end
  
  it "returns the index of the first occurrence of the given substring" do
    "blablabla".index("").should == 0
    "blablabla".index("b").should == 0
    "blablabla".index("bla").should == 0
    "blablabla".index("blabla").should == 0
    "blablabla".index("blablabla").should == 0

    "blablabla".index("l").should == 1
    "blablabla".index("la").should == 1
    "blablabla".index("labla").should == 1
    "blablabla".index("lablabla").should == 1

    "blablabla".index("a").should == 2
    "blablabla".index("abla").should == 2
    "blablabla".index("ablabla").should == 2
  end  
  
  it "doesn't set $~" do
    $~ = nil
    
    'hello.'.index('ll')
    $~.should == nil
  end

  it "ignores string subclasses" do
    "blablabla".index(MyString.new("bla")).should == 0
    MyString.new("blablabla").index("bla").should == 0
    MyString.new("blablabla").index(MyString.new("bla")).should == 0
  end
  
  it "starts the search at the given offset" do
    "blablabla".index("bl", 0).should == 0
    "blablabla".index("bl", 1).should == 3
    "blablabla".index("bl", 2).should == 3
    "blablabla".index("bl", 3).should == 3

    "blablabla".index("bla", 0).should == 0
    "blablabla".index("bla", 1).should == 3
    "blablabla".index("bla", 2).should == 3
    "blablabla".index("bla", 3).should == 3

    "blablabla".index("blab", 0).should == 0
    "blablabla".index("blab", 1).should == 3
    "blablabla".index("blab", 2).should == 3
    "blablabla".index("blab", 3).should == 3

    "blablabla".index("la", 1).should == 1
    "blablabla".index("la", 2).should == 4
    "blablabla".index("la", 3).should == 4
    "blablabla".index("la", 4).should == 4

    "blablabla".index("lab", 1).should == 1
    "blablabla".index("lab", 2).should == 4
    "blablabla".index("lab", 3).should == 4
    "blablabla".index("lab", 4).should == 4

    "blablabla".index("ab", 2).should == 2
    "blablabla".index("ab", 3).should == 5
    "blablabla".index("ab", 4).should == 5
    "blablabla".index("ab", 5).should == 5
    
    "blablabla".index("", 0).should == 0
    "blablabla".index("", 1).should == 1
    "blablabla".index("", 2).should == 2
    "blablabla".index("", 7).should == 7
    "blablabla".index("", 8).should == 8
    "blablabla".index("", 9).should == 9
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    ["bl", "bla", "blab", "la", "lab", "ab", ""].each do |needle|
      (-str.length .. -1).each do |offset|
        str.index(needle, offset).should ==
        str.index(needle, offset + str.length)
      end
    end
  end

  it "returns nil if the substring isn't found" do
    "blablabla".index("B").should == nil
    "blablabla".index("z").should == nil
    "blablabla".index("BLA").should == nil
    "blablabla".index("blablablabla").should == nil
    "blablabla".index("", 10).should == nil
        
    "hello".index("he", 1).should == nil
    "hello".index("he", 2).should == nil
  end
  
  it "converts start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 1)
    "RWOARW".index("RW", obj).should == 4
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    "RWOARW".index("RW", obj).should == 4
  end
end

describe "String#index(regexp [, start_offset])" do
  it "behaves the same as String#index(string) for escaped string regexps" do
    ["blablabla", "hello cruel world...!"].each do |str|
      ["", "b", "bla", "lab", "o c", "d."].each do |needle|
        regexp = Regexp.new(Regexp.escape(needle))
        str.index(regexp).should == str.index(needle)
        
        0.upto(str.size + 1) do |start|
          str.index(regexp, start).should == str.index(needle, start)
        end
        
        (-str.size - 1).upto(-1) do |start|
          str.index(regexp, start).should == str.index(needle, start)
        end
      end
    end
  end
  
  it "returns the index of the first match of regexp" do
    "blablabla".index(/bla/).should == 0
    "blablabla".index(/BLA/i).should == 0

    "blablabla".index(/.{0}/).should == 0
    "blablabla".index(/.{6}/).should == 0
    "blablabla".index(/.{9}/).should == 0

    "blablabla".index(/.*/).should == 0
    "blablabla".index(/.+/).should == 0

    "blablabla".index(/lab|b/).should == 0
    
    "blablabla".index(/\A/).should == 0
    "blablabla".index(/\Z/).should == 9
    "blablabla".index(/\z/).should == 9
    "blablabla\n".index(/\Z/).should == 9
    "blablabla\n".index(/\z/).should == 10

    "blablabla".index(/^/).should == 0
    "\nblablabla".index(/^/).should == 0
    "b\nablabla".index(/$/).should == 1
    "bl\nablabla".index(/$/).should == 2
    
    "blablabla".index(/.l./).should == 0
  end

  it "sets $~ to MatchData of match and nil when there's none" do
    'hello.'.index(/.(.)/)
    $~[0].should == 'he'

    'hello.'.index(/not/)
    $~.should == nil
  end
  
  it "starts the search at the given offset" do
    "blablabla".index(/.{0}/, 5).should == 5
    "blablabla".index(/.{1}/, 5).should == 5
    "blablabla".index(/.{2}/, 5).should == 5
    "blablabla".index(/.{3}/, 5).should == 5
    "blablabla".index(/.{4}/, 5).should == 5

    "blablabla".index(/.{0}/, 3).should == 3
    "blablabla".index(/.{1}/, 3).should == 3
    "blablabla".index(/.{2}/, 3).should == 3
    "blablabla".index(/.{5}/, 3).should == 3
    "blablabla".index(/.{6}/, 3).should == 3

    "blablabla".index(/.l./, 0).should == 0
    "blablabla".index(/.l./, 1).should == 3
    "blablabla".index(/.l./, 2).should == 3
    "blablabla".index(/.l./, 3).should == 3
    
    "xblaxbla".index(/x./, 0).should == 0
    "xblaxbla".index(/x./, 1).should == 4
    "xblaxbla".index(/x./, 2).should == 4
    
    "blablabla\n".index(/\Z/, 9).should == 9
  end
  
  it "starts the search at offset + self.length if offset is negative" do
    str = "blablabla"
    
    ["bl", "bla", "blab", "la", "lab", "ab", ""].each do |needle|
      (-str.length .. -1).each do |offset|
        str.index(needle, offset).should ==
        str.index(needle, offset + str.length)
      end
    end
  end

  it "returns nil if the substring isn't found" do
    "blablabla".index(/BLA/).should == nil

    "blablabla".index(/.{10}/).should == nil
    "blaxbla".index(/.x/, 3).should == nil
    "blaxbla".index(/..x/, 2).should == nil
  end

  it "supports \\G which matches at the given start offset" do
    "helloYOU.".index(/\GYOU/, 5).should == 5
    "helloYOU.".index(/\GYOU/).should == nil

    re = /\G.+YOU/
    # The # marks where \G will match.
    [
      ["#hi!YOUall.", 0],
      ["h#i!YOUall.", 1],
      ["hi#!YOUall.", 2],
      ["hi!#YOUall.", nil]
    ].each do |spec, res|
      start = spec.index("#")
      str = spec.delete("#")

      str.index(re, start).should == res
    end
  end
  
  it "converts start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 1)
    "RWOARW".index(/R./, obj).should == 4

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    "RWOARW".index(/R./, obj).should == 4
  end
end

describe "String#initialize([str])" do
  it "is a private method" do
    "".private_methods.map { |m| m.to_s }.include?("initialize").should == true    
  end
  
  it "can be used to reinitialize self to str" do
    "some string".instance_eval do
      initialize("another string").equal?(self).should == true
      self.should == "another string"

      initialize("").should == ""
    end
  end
  
  it "doesn't change self when str isn't supplied" do
    "some string".instance_eval do
      initialize().should == "some string"
    end
  end
  
  it "replaces the taint status of self with that of str" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.instance_eval { initialize(b) }
    a.tainted?.should == true
  end
    
  it "raises a TypeError if self is frozen" do
    a = "hello".freeze

    a.instance_eval { initialize(a) } # ok, no change
    should_raise(TypeError) { a.instance_eval { initialize("world") } }
  end
end

describe "String#initialize_copy(other)" do
  it "is a private method" do
    "".private_methods.map { |m| m.to_s }.include?("initialize_copy").should == true
  end
  
  it "is an alias for replace()" do
    a = "some string"
    a.instance_eval do
      initialize_copy("another string").equal?(a).should == true
    end
    
    a.should == "another string"
  end
  
  it "replaces the taint status of self with that of other" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.instance_eval { initialize_copy(b) }
    a.tainted?.should == true
  end
    
  it "raises a TypeError if self is frozen" do
    a = "hello".freeze

    a.instance_eval { initialize_copy(a) } # ok, no change
    should_raise(TypeError) { a.instance_eval { initialize_copy("") } }
    should_raise(TypeError) { a.instance_eval { initialize_copy("world") } }
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
    "abcd".insert(-5, 'X').should == "Xabcd"
    "abcd".insert(-3, 'X').should == "abXcd"
    "abcd".insert(-1, 'X').should == "abcdX"
  end
  
  it "raises an IndexError if the index is beyond string" do
    should_raise(IndexError) { "abcd".insert(5, 'X') }
    should_raise(IndexError) { "abcd".insert(-6, 'X') }
  end
  
  it "converts index to an integer using to_int" do
    other = Object.new
    def other.to_int() -3 end
    "abcd".insert(other, "XYZ").should == "abXYZcd"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => -3)
    "abcd".insert(obj, "XYZ").should == "abXYZcd"
  end
  
  it "converts other to a string using to_str" do
    other = Object.new
    def other.to_str() "XYZ" end
    "abcd".insert(-3, other).should == "abXYZcd"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "X")
    "abcd".insert(-3, obj).should == "abXcd"
  end

  it "taints self if string to insert is tainted" do
    str = "abcd"
    str.insert(0, "T".taint).tainted?.should == true

    str = "abcd"
    other = Object.new
    def other.to_str() "T".taint end
    str.insert(0, other).tainted?.should == true
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) { "abcd".insert(-6, ?e) }
    should_raise(TypeError) { "abcd".insert(-6, :sym) }
    should_raise(TypeError) { "abcd".insert(-6, Object.new) }
  end
  
  it "raises a TypeError if self is frozen" do
    str = "abcd".freeze
    should_raise(TypeError) { str.insert(4, '') }
    should_raise(TypeError) { str.insert(4, 'X') }
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
  
  it "taints the result if self is tainted" do
    "foo".taint.inspect.tainted?.should == true
    "foo\n".taint.inspect.tainted?.should == true
  end
end

describe "String#length" do
  it "returns the length of self" do
    "".length.should == 0
    "\x00".length.should == 1
    "one".length.should == 3
    "two".length.should == 3
    "three".length.should == 5
    "four".length.should == 4
  end
end

describe "String#ljust(length, padstr)" do
  it "returns a new string of specified length with self left justified and padded with padstr" do
    "hello".ljust(20, '1234').should == "hello123412341234123"

    "".ljust(1, "abcd").should == "a"
    "".ljust(2, "abcd").should == "ab"
    "".ljust(3, "abcd").should == "abc"
    "".ljust(4, "abcd").should == "abcd"
    "".ljust(6, "abcd").should == "abcdab"

    "OK".ljust(3, "abcd").should == "OKa"
    "OK".ljust(4, "abcd").should == "OKab"
    "OK".ljust(6, "abcd").should == "OKabcd"
    "OK".ljust(8, "abcd").should == "OKabcdab"
  end
  
  it "pads with whitespace if no padstr is given" do
    "hello".ljust(20).should == "hello               "
  end

  it "returns self if it's longer than or as long as the specified length" do
    "".ljust(0).should == ""
    "".ljust(-1).should == ""
    "hello".ljust(4).should == "hello"
    "hello".ljust(-1).should == "hello"
    "this".ljust(3).should == "this"
    "radiology".ljust(8, '-').should == "radiology"
  end

  it "taints result when self or padstr is tainted" do
    "x".taint.ljust(4).tainted?.should == true
    "x".taint.ljust(0).tainted?.should == true
    "".taint.ljust(0).tainted?.should == true
    "x".taint.ljust(4, "*").tainted?.should == true
    "x".ljust(4, "*".taint).tainted?.should == true
  end

  it "tries to convert length to an integer using to_int" do
    "^".ljust(3.8, "_^").should == "^_^"
    
    obj = Object.new
    def obj.to_int() 3 end
      
    "o".ljust(obj, "_o").should == "o_o"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 3)
    "~".ljust(obj, "_~").should == "~_~"
  end
  
  it "raises a TypeError when length can't be converted to an integer" do
    should_raise(TypeError) { "hello".ljust("x") }
    should_raise(TypeError) { "hello".ljust("x", "y") }
    should_raise(TypeError) { "hello".ljust([]) }
    should_raise(TypeError) { "hello".ljust(Object.new) }
  end

  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "123" end
    
    "hello".ljust(10, padstr).should == "hello12312"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "k")

    "hello".ljust(7, obj).should == "hellokk"
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
  
  it "raises an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".ljust(10, '')
    end
  end
  
  it "returns subclass instances when called on subclasses" do
    MyString.new("").ljust(10).class.should == MyString
    MyString.new("foo").ljust(10).class.should == MyString
    MyString.new("foo").ljust(10, MyString.new("x")).class.should == MyString
    
    "".ljust(10, MyString.new("x")).class.should == String
    "foo".ljust(10, MyString.new("x")).class.should == String
  end
end

describe "String#lstrip" do
  it "returns a copy of self with leading whitespace removed" do
   "  hello  ".lstrip.should == "hello  "
   "  hello world  ".lstrip.should == "hello world  "
   "\n\r\t\n\v\r hello world  ".lstrip.should == "hello world  "
   "hello".lstrip.should == "hello"
   "\x00hello".lstrip.should == "\x00hello"
  end
  
  it "taints the result when self is tainted" do
    "".taint.lstrip.tainted?.should == true
    "ok".taint.lstrip.tainted?.should == true
    "   ok".taint.lstrip.tainted?.should == true
  end
end

describe "String#lstrip!" do
  it "modifies self in place and returns self" do
    a = "  hello  "
    a.lstrip!.equal?(a).should == true
    a.should == "hello  "
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.lstrip!.should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError if self is frozen" do
    "hello".freeze.lstrip! # ok, nothing changed
    "".freeze.lstrip! # ok, nothing changed

    should_raise(TypeError) { "  hello  ".freeze.lstrip! }
  end
end

describe "String#match(pattern)" do
  it "matches the pattern against self" do
    'hello'.match(/(.)\1/)[0].should == 'll'
  end
  
  it "tries to convert pattern to a string via to_str" do
    obj = Object.new
    obj.should_receive(:to_str, :returning => ".")
    "hello".match(obj)[0].should == "h"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => ".")
    "hello".match(obj)[0].should == "h"    
  end
  
  it "raises a TypeError if pattern is not a regexp or a string" do
    should_raise(TypeError) { 'hello'.match(10) }
    should_raise(TypeError) { 'hello'.match(:ell) }
  end

  it "converts string patterns to regexps without escaping" do
    'hello'.match('(.)\1')[0].should == 'll'
  end
  
  it "returns nil if there's no match" do
    'hello'.match('xx').should == nil
  end

  it "matches \\G at the start of the string" do
    'hello'.match(/\Gh/)[0].should == 'h'
    'hello'.match(/\Go/).should == nil
  end

  it "sets $~ to MatchData of match or nil when there is none" do
    'hello'.match(/./)
    $~[0].should == 'h'
    Regexp.last_match[0].should == 'h'

    'hello'.match(/X/)
    $~.should == nil
    Regexp.last_match.should == nil
  end
end

describe "String#next" do
  it "is an alias of String#succ" do
    "".succ.should == ""
    "\xFF".succ.should == "\x01\x00"
    "abcd".succ.should == "abcd".next
    "98".succ.should == "98".next
    "ZZZ9999".succ.should == "ZZZ9999".next
  end
  
  it "taints the result if self is tainted" do
    "".taint.succ.tainted?.should == true
    "x".taint.succ.tainted?.should == true
  end
end

describe "String#next!" do
  it "is an alias of String#succ!" do
    a = "THX1138"
    b = "THX1138"
    a.succ!.should == b.next!
    a.should == b
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "".freeze.succ! }
    should_raise(TypeError) { "a".freeze.succ! }
  end
end

# Note: We can't completely spec this in terms of to_int() because hex()
# allows the base to be changed by a base specifier in the string.
# See http://groups.google.com/group/ruby-core-google/browse_frm/thread/b53e9c2003425703
describe "String#oct" do
  it "treats leading characters of self as a string of oct digits" do
    "0".oct.should == 0
    "77".oct.should == 077
    "78".oct.should == 7
    "077".oct.should == 077
    "0o".oct.should == 0

    "755_333".oct.should == 0755_333
    "7__3".oct.should == 073
    "5678".oct.should == 0567

    "0b1010".oct.should == 0b1010
    "0xFF".oct.should == 0xFF
    "0d500".oct.should == 500
  end
  
  it "takes an optional sign" do
    "-1234".oct.should == -01234
    "+1234".oct.should == 01234
  end
  
  it "returns 0 on error" do
    "".oct.should == 0
    "+-5".oct.should == 0
    "wombat".oct.should == 0
  end
end

describe "String#replace(other)" do
  it "replaces the content of self with other and returns self" do
    a = "some string"
    a.replace("another string").equal?(a).should == true
    a.should == "another string"
  end
  
  it "replaces the taint status of self with that of other" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.replace(b)
    a.tainted?.should == true
  end
  
  it "tries to convert other to string using to_str" do
    other = Object.new
    def other.to_str() "an object converted to a string" end
    "hello".replace(other).should == "an object converted to a string"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "X")
    "hello".replace(obj).should == "X"
  end
  
  it "raises a TypeError if other can't be converted to string" do
    should_raise(TypeError) { "hello".replace(123) }
    should_raise(TypeError) { "hello".replace(:test) }
    should_raise(TypeError) { "hello".replace(Object.new) }
  end
  
  it "raises a TypeError if self is frozen" do
    a = "hello".freeze

    a.replace(a) # ok, no change
    should_raise(TypeError) { a.replace("") }
    should_raise(TypeError) { a.replace("world") }
  end
end

describe "String#reverse" do
  it "returns a new string with the characters of self in reverse order" do
    "stressed".reverse.should == "desserts"
    "m".reverse.should == "m"
    "".reverse.should == ""
  end
  
  it "taints the result if self is tainted" do
    "".taint.reverse.tainted?.should == true
    "m".taint.reverse.tainted?.should == true
  end
end

describe "String#reverse!" do
  it "reverses self in place and always returns self" do
    a = "stressed"
    a.reverse!.equal?(a).should == true
    a.should == "desserts"
    
    "".reverse!.should == ""
  end

  it "raises a TypeError if self is frozen" do
    "".freeze.reverse! # ok, no change
    should_raise(TypeError) { "anna".freeze.reverse! }
    should_raise(TypeError) { "hello".freeze.reverse! }
  end
end

describe "String#rindex(obj [, start_offset])" do
  it "raises a TypeError if obj isn't a String, Fixnum or Regexp" do
    should_raise(TypeError) { "hello".rindex(:sym) }    
    should_raise(TypeError) { "hello".rindex(Object.new) }
  end

  it "doesn't try to convert obj to an integer via to_int" do
    obj = Object.new
    obj.should_not_receive(:to_int)
    should_raise(TypeError) { "hello".rindex(obj) }
  end

  # Note: MRI doesn't call to_str, but should do so because index() does it.
  # See http://groups.google.com/group/ruby-core-google/t/3f2d4129febd2a66
  it "tries to convert obj to a string via to_str" do
    obj = Object.new
    obj.should_receive(:to_str, :returning => "lo")
    "hello".rindex(obj).should == "hello".rindex("lo")

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "o")
    "hello".rindex(obj).should == "hello".rindex("o")
  end
end

describe "String#rindex(fixnum [, start_offset])" do
  it "returns the index of the last occurrence of the given character" do
    "hello".rindex(?e).should == 1
    "hello".rindex(?l).should == 3
  end
  
  it "doesn't use fixnum % 256" do
    "hello".rindex(?e + 256 * 3).should == nil
    "hello".rindex(-(256 - ?e)).should == nil
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
  
  it "tries to convert start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    "str".rindex(?s, obj).should == 0
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 5)
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
  end  
  
  it "doesn't set $~" do
    $~ = nil
    
    'hello.'.rindex('ll')
    $~.should == nil
  end
  
  it "ignores string subclasses" do
    "blablabla".rindex(MyString.new("bla")).should == 6
    MyString.new("blablabla").rindex("bla").should == 6
    MyString.new("blablabla").rindex(MyString.new("bla")).should == 6
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
  
  it "tries to convert start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    "str".rindex("st", obj).should == 0
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 5)
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
    "blablabla".rindex(/bla/).should == 6
    "blablabla".rindex(/BLA/i).should == 6

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
  
  it "sets $~ to MatchData of match and nil when there's none" do
    'hello.'.rindex(/.(.)/)
    $~[0].should == 'o.'

    'hello.'.rindex(/not/)
    $~.should == nil
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
    "blablabla".rindex(/BLA/).should == nil
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
  
  it "tries to convert start_offset to an integer via to_int" do
    obj = Object.new
    obj.should_receive(:to_int, :returning => 5)
    "str".rindex(/../, obj).should == 1
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 5)
    "str".rindex(/../, obj).should == 1
  end
end

describe "String#rjust(length, padstr)" do
  it "returns a new string of specified length with self right justified and padded with padstr" do
    "hello".rjust(20, '1234').should == "123412341234123hello"

    "".rjust(1, "abcd").should == "a"
    "".rjust(2, "abcd").should == "ab"
    "".rjust(3, "abcd").should == "abc"
    "".rjust(4, "abcd").should == "abcd"
    "".rjust(6, "abcd").should == "abcdab"

    "OK".rjust(3, "abcd").should == "aOK"
    "OK".rjust(4, "abcd").should == "abOK"
    "OK".rjust(6, "abcd").should == "abcdOK"
    "OK".rjust(8, "abcd").should == "abcdabOK"
  end
  
  it "pads with whitespace if no padstr is given" do
    "hello".rjust(20).should == "               hello"
  end

  it "returns self if it's longer than or as long as the specified length" do
    "".rjust(0).should == ""
    "".rjust(-1).should == ""
    "hello".rjust(4).should == "hello"
    "hello".rjust(-1).should == "hello"
    "this".rjust(3).should == "this"
    "radiology".rjust(8, '-').should == "radiology"
  end

  it "taints result when self or padstr is tainted" do
    "x".taint.rjust(4).tainted?.should == true
    "x".taint.rjust(0).tainted?.should == true
    "".taint.rjust(0).tainted?.should == true
    "x".taint.rjust(4, "*").tainted?.should == true
    "x".rjust(4, "*".taint).tainted?.should == true
  end

  it "tries to convert length to an integer using to_int" do
    "^".rjust(3.8, "^_").should == "^_^"
    
    obj = Object.new
    def obj.to_int() 3 end
      
    "o".rjust(obj, "o_").should == "o_o"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 3)
    "~".rjust(obj, "~_").should == "~_~"
  end
  
  it "raises a TypeError when length can't be converted to an integer" do
    should_raise(TypeError) { "hello".rjust("x") }
    should_raise(TypeError) { "hello".rjust("x", "y") }
    should_raise(TypeError) { "hello".rjust([]) }
    should_raise(TypeError) { "hello".rjust(Object.new) }
  end

  it "tries to convert padstr to a string using to_str" do
    padstr = Object.new
    def padstr.to_str() "123" end
    
    "hello".rjust(10, padstr).should == "12312hello"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "k")

    "hello".rjust(7, obj).should == "kkhello"
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
  
  it "raises an ArgumentError when padstr is empty" do
    should_raise(ArgumentError) do
      "hello".rjust(10, '')
    end
  end
  
  it "returns subclass instances when called on subclasses" do
    MyString.new("").rjust(10).class.should == MyString
    MyString.new("foo").rjust(10).class.should == MyString
    MyString.new("foo").rjust(10, MyString.new("x")).class.should == MyString
    
    "".rjust(10, MyString.new("x")).class.should == String
    "foo".rjust(10, MyString.new("x")).class.should == String
  end
end


describe "String#rstrip" do
  it "returns a copy of self with trailing whitespace removed" do
   "  hello  ".rstrip.should == "  hello"
   "  hello world  ".rstrip.should == "  hello world"
   "  hello world \n\r\t\n\v\r".rstrip.should == "  hello world"
   "hello".rstrip.should == "hello"
   "hello\x00".rstrip.should == "hello"
  end
  
  it "taints the result when self is tainted" do
    "".taint.rstrip.tainted?.should == true
    "ok".taint.rstrip.tainted?.should == true
    "ok    ".taint.rstrip.tainted?.should == true
  end
end

describe "String#rstrip!" do
  it "modifies self in place and returns self" do
    a = "  hello  "
    a.rstrip!.equal?(a).should == true
    a.should == "  hello"
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.rstrip!.should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError if self is frozen" do
    "hello".freeze.rstrip! # ok, nothing changed
    "".freeze.rstrip! # ok, nothing changed

    should_raise(TypeError) { "  hello  ".freeze.rstrip! }
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
    "hello".scan(/()/).should == [[""]] * 6
    "hello".scan(/()()/).should == [["", ""]] * 6
    "cruel world".scan(/(...)/).should == [["cru"], ["el "], ["wor"]]
    "cruel world".scan(/(..)(..)/).should == [["cr", "ue"], ["l ", "wo"]]
  end
  
  it "scans for occurrences of the string if pattern is a string" do
    "one two one two".scan('one').should == ["one", "one"]
    "hello.".scan('.').should == ['.']
  end

  it "sets $~ to MatchData of last match and nil when there's none" do
    'hello.'.scan(/.(.)/)
    $~[0].should == 'o.'

    'hello.'.scan(/not/)
    $~.should == nil

    'hello.'.scan('l')
    $~.begin(0).should == 3
    $~[0].should == 'l'

    'hello.'.scan('not')
    $~.should == nil
  end
  
  it "supports \\G which matches the end of the previous match / string start for first match" do
    "one two one two".scan(/\G\w+/).should == ["one"]
    "one two one two".scan(/\G\w+\s*/).should == ["one ", "two ", "one ", "two"]
    "one two one two".scan(/\G\s*\w+/).should == ["one", " two", " one", " two"]
  end

  it "tries to convert pattern to a string via to_str" do
    obj = Object.new
    obj.should_receive(:to_str, :returning => "o")
    "o_o".scan(obj).should == ["o", "o"]
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "-")
    "-_-".scan(obj).should == ["-", "-"]
  end
  
  it "raises a TypeError if pattern isn't a Regexp and can't be converted to a String" do
    should_raise(TypeError) { "cruel world".scan(5) }
    should_raise(TypeError) { "cruel world".scan(:test) }
    should_raise(TypeError) { "cruel world".scan(Object.new) }
  end
  
  # Note: MRI taints for tainted regexp patterns,
  # but not for tainted string patterns.
  # TODO: Report to ruby-core.
  it "taints the match strings if self is tainted" do
    a = "hello hello hello".scan("hello".taint)
    a.each { |m| m.tainted?.should == true }
    
    a = "hello hello hello".taint.scan("hello")
    a.each { |m| m.tainted?.should == true }
    
    a = "hello".scan(/./.taint)
    a.each { |m| m.tainted?.should == true }

    a = "hello".taint.scan(/./)
    a.each { |m| m.tainted?.should == true }    
  end
end

describe "String#scan(pattern) { block }" do
  it "returns self" do
    s = "foo"
    s.scan(/./) {}.equal?(s).should == true
    s.scan(/roar/) {}.equal?(s).should == true
  end
  
  it "passes each match to the block as one argument: an array" do
    a = []
    "cruel world".scan(/\w+/) { |*w| a << w }
    a.should == [["cruel"], ["world"]]
  end
  
  it "passes groups as to the block as one argument: an array" do
    a = []
    "cruel world".scan(/(..)(..)/) { |*w| a << w }
    a.should == [[["cr", "ue"]], [["l ", "wo"]]]
  end
  
  it "sets $~ for access from the block" do
    str = "hello"

    matches = []
    offsets = []
    
    str.scan(/([aeiou])/) do
       md = $~
       md.string.should == str
       matches << md.to_a
       offsets << md.offset(0)
       str
    end
    
    matches.should == [["e", "e"], ["o", "o"]]
    offsets.should == [[1, 2], [4, 5]]

    matches = []
    offsets = []
    
    str.scan("l") do
       md = $~
       md.string.should == str
       matches << md.to_a
       offsets << md.offset(0)
       str
    end
    
    matches.should == [["l"], ["l"]]
    offsets.should == [[2, 3], [3, 4]]
  end
  
  it "restores $~ after leaving the block" do
    [/./, "l"].each do |pattern|
      old_md = nil
      "hello".scan(pattern) do
        old_md = $~
        "ok".match(/./)
        "x"
      end
    
      $~.should == old_md
      $~.string.should == "hello"
    end
  end
  
  it "sets $~ to MatchData of last match and nil when there's none for access from outside" do
    'hello.'.scan('l') { 'x' }
    $~.begin(0).should == 3
    $~[0].should == 'l'

    'hello.'.scan('not') { 'x' }
    $~.should == nil

    'hello.'.scan(/.(.)/) { 'x' }
    $~[0].should == 'o.'

    'hello.'.scan(/not/) { 'x' }
    $~.should == nil
  end
  
  # Note: MRI taints for tainted regexp patterns,
  # but not for tainted string patterns.
  # TODO: Report to ruby-core.
  it "taints the match strings if self is tainted" do
    "hello hello hello".scan("hello".taint) { |m| m.tainted?.should == true }
    "hello hello hello".taint.scan("hello") { |m| m.tainted?.should == true }
    
    "hello".scan(/./.taint) { |m| m.tainted?.should == true }
    "hello".taint.scan(/./) { |m| m.tainted?.should == true }
  end
end

describe "String#size" do
  it "is an alias of String#length" do
    "".length.should == "".size
    "\x00".length.should == "\x00".size
    "one".length.should == "one".size
    "two".length.should == "two".size
    "three".length.should == "three".size
    "four".length.should == "four".size
  end
end

# String#slice is merged with String#[] for spec sanity reasons.

describe "String#slice!(idx)" do
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
  
  it "calls to_int on idx" do
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

describe "String#slice!(idx, length)" do
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

describe "String#slice!(range)" do
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

describe "String#slice!(regexp)" do
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
      str = str.clone
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

describe "String#slice!(regexp, idx)" do
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
      str = str.clone
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

describe "String#slice!(other_str)" do
  it "removes and returns the first occurrence of other_str from self" do
    c = "hello hello"
    c.slice!('llo').should == "llo"
    c.should == "he hello"
  end
  
  it "taints resulting strings when other is tainted" do
    strs = ["", "hello world", "hello"]
    strs += strs.map { |s| s.dup.taint }

    strs.each do |str|
      str = str.clone
      strs.each do |other|
        other = other.clone
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

describe "String#split(string [, limit])" do
  it "returns an array of substrings based on splitting on the given string" do
    "mellow yellow".split("ello").should == ["m", "w y", "w"]
  end
  
  it "suppresses trailing empty fields when limit isn't given or 0" do
    "1,2,,3,4,,".split(',').should == ["1", "2", "", "3", "4"]
    "1,2,,3,4,,".split(',', 0).should == ["1", "2", "", "3", "4"]
    "  a  b  c\nd  ".split("  ").should == ["", "a", "b", "c\nd"]
    "hai".split("hai").should == []
    ",".split(",").should == []
    ",".split(",", 0).should == []
  end

  it "returns an array with one entry if limit is 1: the original string" do
    "hai".split("hai", 1).should == ["hai"]
    "x.y.z".split(".", 1).should == ["x.y.z"]
    "hello world ".split(" ", 1).should == ["hello world "]
    "hi!".split("", 1).should == ["hi!"]
  end

  it "returns at most limit fields when limit > 1" do
    "hai".split("hai", 2).should == ["", ""]

    "1,2,,3,4,,".split(',', 2).should == ["1", "2,,3,4,,"]
    "1,2,,3,4,,".split(',', 3).should == ["1", "2", ",3,4,,"]
    "1,2,,3,4,,".split(',', 4).should == ["1", "2", "", "3,4,,"]
    "1,2,,3,4,,".split(',', 5).should == ["1", "2", "", "3", "4,,"]
    "1,2,,3,4,,".split(',', 6).should == ["1", "2", "", "3", "4", ","]

    "x".split('x', 2).should == ["", ""]
    "xx".split('x', 2).should == ["", "x"]
    "xx".split('x', 3).should == ["", "", ""]
    "xxx".split('x', 2).should == ["", "xx"]
    "xxx".split('x', 3).should == ["", "", "x"]
    "xxx".split('x', 4).should == ["", "", "", ""]
  end
  
  it "doesn't suppress or limit fields when limit is negative" do
    "1,2,,3,4,,".split(',', -1).should == ["1", "2", "", "3", "4", "", ""]
    "1,2,,3,4,,".split(',', -5).should == ["1", "2", "", "3", "4", "", ""]
    "  a  b  c\nd  ".split("  ", -1).should == ["", "a", "b", "c\nd", ""]
    ",".split(",", -1).should == ["", ""]
  end
  
  it "defaults to $; when string isn't given or nil" do
    begin
      old_fs = $;
    
      [",", ":", "", "XY", nil].each do |fs|
        $; = fs
        
        ["x,y,z,,,", "1:2:", "aXYbXYcXY", ""].each do |str|
          expected = str.split(fs || " ")
          
          str.split(nil).should == expected
          str.split.should == expected

          str.split(nil, -1).should == str.split(fs || " ", -1)
          str.split(nil, 0).should == str.split(fs || " ", 0)
          str.split(nil, 2).should == str.split(fs || " ", 2)
        end
      end
    ensure
      $; = old_fs
    end    
  end
    
  it "ignores leading and continuous whitespace when string is a single space" do
    " now's  the time  ".split(' ').should == ["now's", "the", "time"]
    " now's  the time  ".split(' ', -1).should == ["now's", "the", "time", ""]

    "\t\n a\t\tb \n\r\r\nc\v\vd\v ".split(' ').should == ["a", "b", "c", "d"]
    "a\x00a b".split(' ').should == ["a\x00a", "b"]
  end
  
  it "splits between characters when its argument is an empty string" do
    "hi!".split("").should == ["h", "i", "!"]
    "hi!".split("", -1).should == ["h", "i", "!", ""]
    "hi!".split("", 2).should == ["h", "i!"]
  end
  
  it "tries converting its pattern argument to a string via to_str" do
    obj = Object.new
    def obj.to_str() "::" end
    "hello::world".split(obj).should == ["hello", "world"]

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "::")
    "hello::world".split(obj).should == ["hello", "world"]
  end
  
  it "tries converting limit to an integer via to_int" do
    obj = Object.new
    def obj.to_int() 2 end
    "1.2.3.4".split(".", obj).should == ["1", "2.3.4"]

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    "1.2.3.4".split(".", obj).should == ["1", "2.3.4"]
  end
  
  it "doesn't set $~" do
    $~ = nil
    "x.y.z".split(".")
    $~.should == nil
  end
  
  it "returns subclass instances based on self" do
    ["", "x.y.z.", "  x  y  "].each do |str|
      ["", ".", " "].each do |pat|
        [-1, 0, 1, 2].each do |limit|
          MyString.new(str).split(pat, limit).each do |x|
            x.class.should == MyString
          end
          
          str.split(MyString.new(pat), limit).each do |x|
            x.class.should == String 
          end
        end
      end
    end
  end
  
  it "taints the resulting strings if self is tainted" do
    ["", "x.y.z.", "  x  y  "].each do |str|
      ["", ".", " "].each do |pat|
        [-1, 0, 1, 2].each do |limit|
          str.dup.taint.split(pat).each do |x|
            x.tainted?.should == true
          end
          
          str.split(pat.dup.taint).each do |x|
            x.tainted?.should == false
          end
        end
      end
    end    
  end
end

describe "String#split(regexp [, limit])" do
  it "divides self on regexp matches" do
    " now's  the time".split(/ /).should == ["", "now's", "", "the", "time"]
    " x\ny ".split(/ /).should == ["", "x\ny"]
    "1, 2.34,56, 7".split(/,\s*/).should == ["1", "2.34", "56", "7"]
    "1x2X3".split(/x/i).should == ["1", "2", "3"]
  end
  
  it "suppresses trailing empty fields when limit isn't given or 0" do
    "1,2,,3,4,,".split(/,/).should == ["1", "2", "", "3", "4"]
    "1,2,,3,4,,".split(/,/, 0).should == ["1", "2", "", "3", "4"]
    "  a  b  c\nd  ".split(/\s+/).should == ["", "a", "b", "c", "d"]
    "hai".split(/hai/).should == []
    ",".split(/,/).should == []
    ",".split(/,/, 0).should == []
  end

  it "returns an array with one entry if limit is 1: the original string" do
    "hai".split(/hai/, 1).should == ["hai"]
    "xAyBzC".split(/[A-Z]/, 1).should == ["xAyBzC"]
    "hello world ".split(/\s+/, 1).should == ["hello world "]
    "hi!".split(//, 1).should == ["hi!"]
  end

  it "returns at most limit fields when limit > 1" do
    "hai".split(/hai/, 2).should == ["", ""]

    "1,2,,3,4,,".split(/,/, 2).should == ["1", "2,,3,4,,"]
    "1,2,,3,4,,".split(/,/, 3).should == ["1", "2", ",3,4,,"]
    "1,2,,3,4,,".split(/,/, 4).should == ["1", "2", "", "3,4,,"]
    "1,2,,3,4,,".split(/,/, 5).should == ["1", "2", "", "3", "4,,"]
    "1,2,,3,4,,".split(/,/, 6).should == ["1", "2", "", "3", "4", ","]

    "x".split(/x/, 2).should == ["", ""]
    "xx".split(/x/, 2).should == ["", "x"]
    "xx".split(/x/, 3).should == ["", "", ""]
    "xxx".split(/x/, 2).should == ["", "xx"]
    "xxx".split(/x/, 3).should == ["", "", "x"]
    "xxx".split(/x/, 4).should == ["", "", "", ""]
  end
  
  it "doesn't suppress or limit fields when limit is negative" do
    "1,2,,3,4,,".split(/,/, -1).should == ["1", "2", "", "3", "4", "", ""]
    "1,2,,3,4,,".split(/,/, -5).should == ["1", "2", "", "3", "4", "", ""]
    "  a  b  c\nd  ".split(/\s+/, -1).should == ["", "a", "b", "c", "d", ""]
    ",".split(/,/, -1).should == ["", ""]
  end
  
  it "defaults to $; when regexp isn't given or nil" do
    begin
      old_fs = $;
    
      [/,/, /:/, //, /XY/, /./].each do |fs|
        $; = fs
        
        ["x,y,z,,,", "1:2:", "aXYbXYcXY", ""].each do |str|
          expected = str.split(fs)
          
          str.split(nil).should == expected
          str.split.should == expected

          str.split(nil, -1).should == str.split(fs, -1)
          str.split(nil, 0).should == str.split(fs, 0)
          str.split(nil, 2).should == str.split(fs, 2)
        end
      end
    ensure
      $; = old_fs
    end    
  end
  
  it "splits between characters when regexp matches a zero-length string" do
    "hello".split(//).should == ["h", "e", "l", "l", "o"]
    "hello".split(//, -1).should == ["h", "e", "l", "l", "o", ""]
    "hello".split(//, 2).should == ["h", "ello"]
    
    "hi mom".split(/\s*/).should == ["h", "i", "m", "o", "m"]
  end
  
  it "includes all captures in the result array" do
    "hello".split(/(el)/).should == ["h", "el", "lo"]
    "hi!".split(/()/).should == ["h", "", "i", "", "!"]
    "hi!".split(/()/, -1).should == ["h", "", "i", "", "!", "", ""]
    "hello".split(/((el))()/).should == ["h", "el", "el", "", "lo"]
    "AabB".split(/([a-z])+/).should == ["A", "b", "B"]
  end

  it "tries converting limit to an integer via to_int" do
    obj = Object.new
    def obj.to_int() 2 end
    "1.2.3.4".split(".", obj).should == ["1", "2.3.4"]

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 2)
    "1.2.3.4".split(".", obj).should == ["1", "2.3.4"]
  end
  
  it "doesn't set $~" do
    $~ = nil
    "x:y:z".split(/:/)
    $~.should == nil
  end
  
  it "returns subclass instances based on self" do
    ["", "x:y:z:", "  x  y  "].each do |str|
      [//, /:/, /\s+/].each do |pat|
        [-1, 0, 1, 2].each do |limit|
          MyString.new(str).split(pat, limit).each do |x|
            x.class.should == MyString
          end
        end
      end
    end
  end
  
  it "taints the resulting strings if self is tainted" do
    ["", "x:y:z:", "  x  y  "].each do |str|
      [//, /:/, /\s+/].each do |pat|
        [-1, 0, 1, 2].each do |limit|
          str.dup.taint.split(pat).each do |x|
            x.tainted?.should == true
          end
          
          str.split(pat.dup.taint).each do |x|
            x.tainted?.should == false
          end
        end
      end
    end    
  end  
end

describe "String#squeeze(*sets)" do
  it "returns new string where runs of the same character are replaced by a single character when no args are given" do
    "yellow moon".squeeze.should == "yelow mon"
  end
  
  it "only squeezes chars that are in the intersection of all sets given" do
    "woot squeeze cheese".squeeze("eost", "queo").should == "wot squeze chese"
    "  now   is  the".squeeze(" ").should == " now is the"
  end
  
  it "negates sets starting with ^" do
    s = "<<subbookkeeper!!!>>"
    s.squeeze("beko", "^e").should == s.squeeze("bko")
    s.squeeze("^<bek!>").should == s.squeeze("o")
    s.squeeze("^o").should == s.squeeze("<bek!>")
    s.squeeze("^").should == s
    "^__^".squeeze("^^").should == "^_^"
    "((^^__^^))".squeeze("_^").should == "((^_^))"
  end
  
  it "squeezes all chars in a sequence" do
    s = "--subbookkeeper--"
    s.squeeze("\x00-\xFF").should == s.squeeze
    s.squeeze("bk-o").should == s.squeeze("bklmno")
    s.squeeze("b-e").should == s.squeeze("bcde")
    s.squeeze("e-").should == "-subbookkeper-"
    s.squeeze("-e").should == "-subbookkeper-"
    s.squeeze("---").should == "-subbookkeeper-"
    "ook--001122".squeeze("--2").should == "ook-012"
    "ook--(())".squeeze("(--").should == "ook-()"
    s.squeeze("e-b").should == s
    s.squeeze("^e-b").should == s.squeeze
    s.squeeze("^b-e").should == "-subbokeeper-"
    "^^__^^".squeeze("^^-^").should == "^^_^^"
    "^^--^^".squeeze("^---").should == "^--^"
    
    s.squeeze("b-dk-o-").should == "-subokeeper-"
    s.squeeze("-b-dk-o").should == "-subokeeper-"
    s.squeeze("b-d-k-o").should == "-subokeeper-"
    
    s.squeeze("bc-e").should == "--subookkeper--"
    s.squeeze("^bc-e").should == "-subbokeeper-"

    "AABBCCaabbcc[[]]".squeeze("A-a").should == "ABCabbcc[]"
  end
  
  it "taints the result when self is tainted" do
    "hello".taint.squeeze("e").tainted?.should == true
    "hello".taint.squeeze("a-z").tainted?.should == true

    "hello".squeeze("e".taint).tainted?.should == false
    "hello".squeeze("l".taint).tainted?.should == false
  end
  
  it "tries to convert each set arg to a string using to_str" do
    other_string = Object.new
    def other_string.to_str() "lo" end
    
    other_string2 = Object.new
    def other_string2.to_str() "o" end
    
    "hello room".squeeze(other_string, other_string2).should == "hello rom"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "o")
    "hello room".squeeze(obj).should == "hello rom"
  end
  
  it "raises a TypeError when one set arg can't be converted to a string" do
    should_raise(TypeError) { "hello world".squeeze(?o) }
    should_raise(TypeError) { "hello world".squeeze(:o) }
    should_raise(TypeError) { "hello world".squeeze(Object.new) }
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("oh no!!!").squeeze("!").class.should == MyString
  end
end

describe "String#squeeze!([other_strings])" do
  it "modifies self in place and returns self" do
    a = "yellow moon"
    a.squeeze!.equal?(a).should == true
    a.should == "yelow mon"
  end
  
  it "returns nil if no modifications were made" do
    a = "squeeze"
    a.squeeze!("u", "sq").should == nil
    a.squeeze!("q").should == nil
    a.should == "squeeze"
  end

  it "raises a TypeError when self is frozen" do
    a = "yellow moon"
    a.freeze

    should_raise(TypeError) { a.squeeze!("") }
    should_raise(TypeError) { a.squeeze! }
  end
end

describe "String#strip" do
  it "returns a new string with leading and trailing whitespace removed" do
    "   hello   ".strip.should == "hello"
    "   hello world   ".strip.should == "hello world"
    "\tgoodbye\r\v\n".strip.should == "goodbye"
    "  goodbye \000".strip.should == "goodbye"
  end
  
  it "taints the result when self is tainted" do
    "".taint.strip.tainted?.should == true
    "ok".taint.strip.tainted?.should == true
    "  ok  ".taint.strip.tainted?.should == true
  end
end

describe "String#strip!" do
  it "modifies self in place and returns self" do
    a = "   hello   "
    a.strip!.equal?(a).should == true
    a.should == "hello"
  end
  
  it "returns nil if no modifications where made" do
    a = "hello"
    a.strip!.should == nil
    a.should == "hello"
  end

  it "raises a TypeError if self is frozen" do
    "hello".freeze.strip! # ok, nothing changed
    "".freeze.strip! # ok, nothing changed

    should_raise(TypeError) { "  hello  ".freeze.strip! }
  end
end

describe "String#sub(pattern, replacement)" do
  it "returns a copy of self with all occurrences of pattern replaced with replacement" do
    "hello".sub(/[aeiou]/, '*').should == "h*llo"
    "hello".sub(//, ".").should == ".hello"
  end
  
  it "ignores a block if supplied" do
    "food".sub(/f/, "g") { "w" }.should == "good"
  end

  it "supports \\G which matches at the beginning of the string" do
    "hello world!".sub(/\Ghello/, "hi").should == "hi world!"
  end

  it "supports /i for ignoring case" do
    "Hello".sub(/h/i, "j").should == "jello"
    "hello".sub(/H/i, "j").should == "jello"
  end
  
  it "doesn't interpret regexp metacharacters if pattern is a string" do
    "12345".sub('\d', 'a').should == "12345"
    '\d'.sub('\d', 'a').should == "a"
  end
  
  it "replaces \\1 sequences with the regexp's corresponding capture" do
    str = "hello"
    
    str.sub(/([aeiou])/, '<\1>').should == "h<e>llo"
    str.sub(/(.)/, '\1\1').should == "hhello"

    str.sub(/.(.?)/, '<\0>(\1)').should == "<he>(e)llo"

    str.sub(/.(.)+/, '\1').should == "o"

    str = "ABCDEFGHIJKL"
    re = /#{"(.)" * 12}/
    str.sub(re, '\1').should == "A"
    str.sub(re, '\9').should == "I"
    # Only the first 9 captures can be accessed in MRI
    str.sub(re, '\10').should == "A0"
  end

  it "treats \\1 sequences without corresponding captures as empty strings" do
    str = "hello!"
    
    str.sub("", '<\1>').should == "<>hello!"
    str.sub("h", '<\1>').should == "<>ello!"

    str.sub(//, '<\1>').should == "<>hello!"
    str.sub(/./, '\1\2\3').should == "ello!"
    str.sub(/.(.{20})?/, '\1').should == "ello!"
  end

  it "replaces \\& and \\0 with the complete match" do
    str = "hello!"
    
    str.sub("", '<\0>').should == "<>hello!"
    str.sub("", '<\&>').should == "<>hello!"
    str.sub("he", '<\0>').should == "<he>llo!"
    str.sub("he", '<\&>').should == "<he>llo!"
    str.sub("l", '<\0>').should == "he<l>lo!"
    str.sub("l", '<\&>').should == "he<l>lo!"
    
    str.sub(//, '<\0>').should == "<>hello!"
    str.sub(//, '<\&>').should == "<>hello!"
    str.sub(/../, '<\0>').should == "<he>llo!"
    str.sub(/../, '<\&>').should == "<he>llo!"
    str.sub(/(.)./, '<\0>').should == "<he>llo!"
  end

  it "replaces \\` with everything before the current match" do
    str = "hello!"
    
    str.sub("", '<\`>').should == "<>hello!"
    str.sub("h", '<\`>').should == "<>ello!"
    str.sub("l", '<\`>').should == "he<he>lo!"
    str.sub("!", '<\`>').should == "hello<hello>"
    
    str.sub(//, '<\`>').should == "<>hello!"
    str.sub(/..o/, '<\`>').should == "he<he>!"
  end

  it "replaces \\' with everything after the current match" do
    str = "hello!"
    
    str.sub("", '<\\\'>').should == "<hello!>hello!"
    str.sub("h", '<\\\'>').should == "<ello!>ello!"
    str.sub("ll", '<\\\'>').should == "he<o!>o!"
    str.sub("!", '<\\\'>').should == "hello<>"
    
    str.sub(//, '<\\\'>').should == "<hello!>hello!"
    str.sub(/../, '<\\\'>').should == "<llo!>llo!"
  end
  
  it "replaces \\+ with the last paren that actually matched" do
    str = "hello!"
    
    str.sub(/(.)(.)/, '\+').should == "ello!"
    str.sub(/(.)(.)+/, '\+').should == "!"
    str.sub(/(.)()/, '\+').should == "ello!"
    str.sub(/(.)(.{20})?/, '<\+>').should == "<h>ello!"

    str = "ABCDEFGHIJKL"
    re = /#{"(.)" * 12}/
    str.sub(re, '\+').should == "L"
  end

  it "treats \\+ as an empty string if there was no captures" do
    "hello!".sub(/./, '\+').should == "ello!"
  end
  
  it "maps \\\\ in replacement to \\" do
    "hello".sub(/./, '\\\\').should == '\\ello'
  end

  it "leaves unknown \\x escapes in replacement untouched" do
    "hello".sub(/./, '\\x').should == '\\xello'
    "hello".sub(/./, '\\y').should == '\\yello'
  end

  it "leaves \\ at the end of replacement untouched" do
    "hello".sub(/./, 'hah\\').should == 'hah\\ello'
  end

  it "taints the result if the original string or replacement is tainted" do
    hello = "hello"
    hello_t = "hello"
    a = "a"
    a_t = "a"
    empty = ""
    empty_t = ""
    
    hello_t.taint; a_t.taint; empty_t.taint
    
    hello_t.sub(/./, a).tainted?.should == true
    hello_t.sub(/./, empty).tainted?.should == true

    hello.sub(/./, a_t).tainted?.should == true
    hello.sub(/./, empty_t).tainted?.should == true
    hello.sub(//, empty_t).tainted?.should == true
    
    hello.sub(//.taint, "foo").tainted?.should == false
  end

  it "tries to convert pattern to a string using to_str" do
    pattern = Object.new
    def pattern.to_str() "." end
    
    "hello.".sub(pattern, "!").should == "hello!"

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => ".")

    "hello.".sub(obj, "!").should == "hello!"
  end

  it "raises a TypeError when pattern can't be converted to a string" do
    should_raise(TypeError) { "hello".sub(:woot, "x") }
    should_raise(TypeError) { "hello".sub(?e, "x") }
  end

  it "tries to convert replacement to a string using to_str" do
    replacement = Object.new
    def replacement.to_str() "hello_replacement" end
    
    "hello".sub(/hello/, replacement).should == "hello_replacement"
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "ok")
    "hello".sub(/hello/, obj).should == "ok"
  end
  
  it "raises a TypeError when replacement can't be converted to a string" do
    should_raise(TypeError) { "hello".sub(/[aeiou]/, :woot) }
    should_raise(TypeError) { "hello".sub(/[aeiou]/, ?f) }
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("").sub(//, "").class.should == MyString
    MyString.new("").sub(/foo/, "").class.should == MyString
    MyString.new("foo").sub(/foo/, "").class.should == MyString
    MyString.new("foo").sub("foo", "").class.should == MyString
  end
  
  it "sets $~ to MatchData of match and nil when there's none" do
    'hello.'.sub('hello', 'x')
    $~[0].should == 'hello'

    'hello.'.sub('not', 'x')
    $~.should == nil

    'hello.'.sub(/.(.)/, 'x')
    $~[0].should == 'he'

    'hello.'.sub(/not/, 'x')
    $~.should == nil
  end
end

describe "String#sub(pattern) { block }" do
  it "returns a copy of self with the first occurrences of pattern replaced with the block's return value" do
    "hi".sub(/./) { |s| s[0].to_s + ' ' }.should == "104 i"
    "hi!".sub(/(.)(.)/) { |*a| a.inspect }.should == '["hi"]!'
  end
  
  it "sets $~ for access from the block" do
    str = "hello"
    str.sub(/([aeiou])/) { "<#{$~[1]}>" }.should == "h<e>llo"
    str.sub(/([aeiou])/) { "<#{$1}>" }.should == "h<e>llo"
    str.sub("l") { "<#{$~[0]}>" }.should == "he<l>lo"
    
    offsets = []
    
    str.sub(/([aeiou])/) do
       md = $~
       md.string.should == str
       offsets << md.offset(0)
       str
    end.should == "hhellollo"
    
    offsets.should == [[1, 2]]
  end
  
  it "restores $~ after leaving the block" do
    [/./, "l"].each do |pattern|
      old_md = nil
      "hello".sub(pattern) do
        old_md = $~
        "ok".match(/./)
        "x"
      end
    
      $~.should == old_md
      $~.string.should == "hello"
    end
  end
  
  it "sets $~ to MatchData of last match and nil when there's none for access from outside" do
    'hello.'.sub('l') { 'x' }
    $~.begin(0).should == 2
    $~[0].should == 'l'

    'hello.'.sub('not') { 'x' }
    $~.should == nil

    'hello.'.sub(/.(.)/) { 'x' }
    $~[0].should == 'he'

    'hello.'.sub(/not/) { 'x' }
    $~.should == nil
  end
  
  it "doesn't raise a RuntimeError if the string is modified while substituting" do
    str = "hello"
    str.sub(//) { str[0] = 'x' }.should == "xhello"
    str.should == "xello"
  end
  
  it "doesn't interpolate special sequences like \\1 for the block's return value" do
    repl = '\& \0 \1 \` \\\' \+ \\\\ foo'
    "hello".sub(/(.+)/) { repl }.should == repl
  end
  
  it "converts the block's return value to a string using to_s" do
    replacement = Object.new
    def replacement.to_s() "hello_replacement" end
    
    "hello".sub(/hello/) { replacement }.should == "hello_replacement"
    
    obj = Object.new
    class << obj; undef :to_s; end
    obj.should_receive(:method_missing, :with => [:to_s], :returning => "ok")
    
    "hello".sub(/.+/) { obj }.should == "ok"
  end
  
  it "taints the result if the original string or replacement is tainted" do
    hello = "hello"
    hello_t = "hello"
    a = "a"
    a_t = "a"
    empty = ""
    empty_t = ""
    
    hello_t.taint; a_t.taint; empty_t.taint
    
    hello_t.sub(/./) { a }.tainted?.should == true
    hello_t.sub(/./) { empty }.tainted?.should == true

    hello.sub(/./) { a_t }.tainted?.should == true
    hello.sub(/./) { empty_t }.tainted?.should == true
    hello.sub(//) { empty_t }.tainted?.should == true
    
    hello.sub(//.taint) { "foo" }.tainted?.should == false
  end
end

describe "String#sub!(pattern, replacement)" do
  it "modifies self in place and returns self" do
    a = "hello"
    a.sub!(/[aeiou]/, '*').equal?(a).should == true
    a.should == "h*llo"
  end

  it "taints self if replacement is tainted" do
    a = "hello"
    a.sub!(/./.taint, "foo").tainted?.should == false
    a.sub!(/./, "foo".taint).tainted?.should == true
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.sub!(/z/, '*').should == nil
    a.sub!(/z/, 'z').should == nil
    a.should == "hello"
  end
  
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    s.sub!(/ROAR/, "x") # ok
    should_raise(TypeError) { s.sub!(/e/, "e") }
    should_raise(TypeError) { s.sub!(/[aeiou]/, '*') }
  end
end

describe "String#sub!(pattern) { block }" do
  it "modifies self in place and returns self" do
    a = "hello"
    a.sub!(/[aeiou]/) { '*' }.equal?(a).should == true
    a.should == "h*llo"
  end

  it "taints self if block's result is tainted" do
    a = "hello"
    a.sub!(/./.taint) { "foo" }.tainted?.should == false
    a.sub!(/./) { "foo".taint }.tainted?.should == true
  end
  
  it "returns nil if no modifications were made" do
    a = "hello"
    a.sub!(/z/) { '*' }.should == nil
    a.sub!(/z/) { 'z' }.should == nil
    a.should == "hello"
  end

  it "raises a RuntimeError if the string is modified while substituting" do
    str = "hello"
    should_raise(RuntimeError) { str.sub!(//) { str << 'x' } }
  end
  
  # MRI 1.8 raises a RuntimeError here which is inconsistent
  # with the non-block form of sub! (and everything else)
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    s.sub!(/ROAR/) { "x" } # ok
    should_raise(TypeError) { s.sub!(/e/) { "e" } }
    should_raise(TypeError) { s.sub!(/[aeiou]/) { '*' } }
  end
end

describe "String#succ" do
  it "returns an empty string for empty strings" do
    "".succ.should == ""
  end
  
  it "returns the successor by increasing the rightmost alphanumeric (digit => digit, letter => letter with same case)" do
    "abcd".succ.should == "abce"
    "THX1138".succ.should == "THX1139"
    
    "<<koala>>".succ.should == "<<koalb>>"
    "==A??".succ.should == "==B??"
  end
  
  it "increases non-alphanumerics (via ascii rules) if there are no alphanumerics" do
    "***".succ.should == "**+"
    "**`".succ.should == "**a"
  end
  
  it "increases the next best alphanumeric (jumping over non-alphanumerics) if there is a carry" do
    "dz".succ.should == "ea"
    "HZ".succ.should == "IA"
    "49".succ.should == "50"
    
    "izz".succ.should == "jaa"
    "IZZ".succ.should == "JAA"
    "699".succ.should == "700"
    
    "6Z99z99Z".succ.should == "7A00a00A"
    
    "1999zzz".succ.should == "2000aaa"
    "NZ/[]ZZZ9999".succ.should == "OA/[]AAA0000"
  end

  it "increases the next best character if there is a carry for non-alphanumerics" do
    "(\xFF".succ.should == ")\x00"
    "`\xFF".succ.should == "a\x00"
    "<\xFF\xFF".succ.should == "=\x00\x00"
  end

  it "adds an additional character (just left to the last increased one) if there is a carry and no character left to increase" do
    "z".succ.should == "aa"
    "Z".succ.should == "AA"
    "9".succ.should == "10"
    
    "zz".succ.should == "aaa"
    "ZZ".succ.should == "AAA"
    "99".succ.should == "100"

    "9Z99z99Z".succ.should == "10A00a00A"

    "ZZZ9999".succ.should == "AAAA0000"
    "/[]ZZZ9999".succ.should == "/[]AAAA0000"
    "Z/[]ZZZ9999".succ.should == "AA/[]AAA0000"
    
    # non-alphanumeric cases
    "\xFF".succ.should == "\x01\x00"
    "\xFF\xFF".succ.should == "\x01\x00\x00"
  end

  it "returns subclass instances when called on a subclass" do
    MyString.new("").succ.class.should == MyString
    MyString.new("a").succ.class.should == MyString
    MyString.new("z").succ.class.should == MyString
  end
  
  it "taints the result if self is tainted" do
    ["", "a", "z", "Z", "9", "\xFF", "\xFF\xFF"].each do |s|
      s.taint.succ.tainted?.should == true
    end
  end
end

describe "String#succ!" do
  it "is equivalent to succ, but modifies self in place (still returns self)" do
    ["", "abcd", "THX1138"].each do |s|
      r = s.succ
      s.succ!.equal?(s).should == true
      s.should == r
    end
  end
  
  it "raises a TypeError if self is frozen" do
    should_raise(TypeError) { "".freeze.succ! }
    should_raise(TypeError) { "abcd".freeze.succ! }
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
  
  it "taints resulting string when self is tainted" do
    "".taint.swapcase.tainted?.should == true
    "hello".taint.swapcase.tainted?.should == true
  end

  it "is locale insensitive (only upcases a-z and only downcases A-Z)" do
    "ÄÖÜ".swapcase.should == "ÄÖÜ"
    "ärger".swapcase.should == "äRGER"
    "BÄR".swapcase.should == "bÄr"
  end
  
  it "returns subclass instances when called on a subclass" do
    MyString.new("").swapcase.class.should == MyString
    MyString.new("hello").swapcase.class.should == MyString
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
    
    "".swapcase!.should == nil
  end
  
  it "raises a TypeError when self is frozen" do
    ["", "hello"].each do |a|
      a.freeze
      should_raise(TypeError) { a.swapcase! }
    end
  end
end

describe "String#to_f" do
  it "treats leading characters of self as a floating point number" do
   "123.45e1".to_f.should == 1234.5
   "45.67 degrees".to_f.should == 45.67
   "0".to_f.to_s.should == "0.0"
   "123.45e1".to_f.should == 1234.5
   
   ".5".to_f.should == 0.5
   ".5e1".to_f.should == 5.0
   
   "0b5".to_f.should == 0
   "0d5".to_f.should == 0
   "0o5".to_f.should == 0
   "0x5".to_f.should == 0
   
   "NaN".to_f.should == 0
   "Infinity".to_f.should == 0
   "-Infinity".to_f.should == 0
  end

  it "takes an optional sign" do
    "-45.67 degrees".to_f.should == -45.67
    "+45.67 degrees".to_f.should == 45.67
    "-5_5e-5_0".to_f.should == -55e-50
    "-".to_f.to_s.should == "-0.0"
    "-0".to_f.to_s.should == "-0.0"
  end
  
  it "returns 0.0 if the conversion fails" do
    "bad".to_f.should == 0.0
    "thx1138".to_f.should == 0.0
  end
end

describe "String#to_i(base=10)" do
  it "interprets leading characters as a number in the given base" do
    "100110010010".to_i(2).should == 0b100110010010
    "100110201001".to_i(3).should == 186409
    "103110201001".to_i(4).should == 5064769
    "103110241001".to_i(5).should == 55165126
    "153110241001".to_i(6).should == 697341529
    "153160241001".to_i(7).should == 3521513430
    "153160241701".to_i(8).should == 14390739905
    "853160241701".to_i(9).should == 269716550518
    "853160241791".to_i(10).should == 853160241791
    
    "F00D_BE_1337".to_i(16).should == 0xF00D_BE_1337
    "-hello_world".to_i(32).should == -18306744
    "abcXYZ".to_i(36).should == 623741435
    
    ("z" * 24).to_i(36).should == 22452257707354557240087211123792674815

    "5e10".to_i.should == 5
    
    "0b-1".to_i(2).should == 0
    "0d-1".to_i(10).should == 0
    "0o-1".to_i(8).should == 0
    "0x-1".to_i(16).should == 0
  end
  
  it "auto-detects base via base specifiers (default: 10) for base = 0" do
    "01778".to_i(0).should == 0177
    "0b112".to_i(0).should == 0b11
    "0d19A".to_i(0).should == 19
    "0o178".to_i(0).should == 0o17
    "0xFAZ".to_i(0).should == 0xFA
    "1234567890ABC".to_i(0).should == 1234567890

    "-01778".to_i(0).should == -0177
    "-0b112".to_i(0).should == -0b11
    "-0d19A".to_i(0).should == -19
    "-0o178".to_i(0).should == -0o17
    "-0xFAZ".to_i(0).should == -0xFA
    "-1234567890ABC".to_i(0).should == -1234567890
    
    "0b-1".to_i(0).should == 0
    "0d-1".to_i(0).should == 0
    "0o-1".to_i(0).should == 0
    "0x-1".to_i(0).should == 0
  end
  
  it "doesn't handle foreign base specifiers when base is > 0" do
    [2, 3, 4, 8, 10].each do |base|
      "0111".to_i(base).should == "111".to_i(base)
      
      "0b11".to_i(base).should == (base ==  2 ? 0b11 : 0)
      "0d11".to_i(base).should == (base == 10 ? 0d11 : 0)
      "0o11".to_i(base).should == (base ==  8 ? 0o11 : 0)
      "0xFA".to_i(base).should == 0
    end
    
    "0xD00D".to_i(16).should == 0xD00D
    
    "0b11".to_i(16).should == 0xb11
    "0d11".to_i(16).should == 0xd11
    "0o11".to_i(25).should == 15026
    "0x11".to_i(34).should == 38183
  end
  
  it "raises ArgumentError for illegal radices (1, < 0 or > 36)" do
    should_raise(ArgumentError) { "".to_i(1) }
    should_raise(ArgumentError) { "".to_i(-1) }
    should_raise(ArgumentError) { "".to_i(37) }
  end
end

describe "String#to_s" do
  it "returns self" do
    a = "a string"
    a.equal?(a.to_s).should == true
  end
end

describe "String#to_str" do
  it "returns self" do
    a = "a string"
    a.equal?(a.to_s).should == true
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
    should_raise(ArgumentError) { "".to_sym }
    should_raise(ArgumentError) { "foo\x00bar".to_sym }
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

describe "String#unpack(format)" do
  it "returns an array by decoding self according to the format string" do
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

describe "String#upcase" do
  # TODO
end

describe "String#upcase!" do
  # TODO
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