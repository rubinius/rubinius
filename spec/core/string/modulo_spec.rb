require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#%" do
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

      lambda { "" % [1, 2, 3]   }.should raise_error(ArgumentError)
      lambda { "%s" % [1, 2, 3] }.should raise_error(ArgumentError)
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
    ("%\0x hello" % []).should == "%x hello"
  end
  
  it "replaces trailing absolute argument specifier without type with percent sign" do
    ("hello %1$" % "foo").should == "hello %"
  end
  
  it "raises an ArgumentError when given invalid argument specifiers" do
    lambda { "%1" % [] }.should raise_error(ArgumentError)
    lambda { "%+" % [] }.should raise_error(ArgumentError)
    lambda { "%-" % [] }.should raise_error(ArgumentError)
    lambda { "%#" % [] }.should raise_error(ArgumentError)
    lambda { "%0" % [] }.should raise_error(ArgumentError)
    lambda { "%*" % [] }.should raise_error(ArgumentError)
    lambda { "%." % [] }.should raise_error(ArgumentError)
    lambda { "%_" % [] }.should raise_error(ArgumentError)
    lambda { "%0$s" % "x"              }.should raise_error(ArgumentError)
    lambda { "%*0$s" % [5, "x"]        }.should raise_error(ArgumentError)
    lambda { "%*1$.*0$1$s" % [1, 2, 3] }.should raise_error(ArgumentError)
    
    # Commented these out because they were incorrect behavior in MRI
    
    # Star precision before star width:
    # raise_error(ArgumentError) { "%.**d" % [5, 10, 1] }

    # Precision before flags and width:
    # raise_error(ArgumentError) { "%.5+05d" % 5 }
    # raise_error(ArgumentError) { "%.5 5d" % 5 }

    # Overriding a star width with a numeric one:
    # raise_error(ArgumentError) { "%*1s" % [5, 1] }

    # Width before flags:
    # raise_error(ArgumentError) { "%5+0d" % 1 }
    # raise_error(ArgumentError) { "%5 0d" % 1 }

    # Specifying width multiple times:
    # raise_error(ArgumentError) { "%50+30+20+10+5d" % 5 }
    # raise_error(ArgumentError) { "%50 30 20 10 5d" % 5 }

    # Specifying the precision multiple times with negative star arguments:
    # raise_error(ArgumentError) { "%.*.*.*.*f" % [-1, -1, -1, 5, 1] }
  end

  it "raises an ArgumentError when multiple positional argument tokens are given for one format specifier" do
    lambda { "%1$1$s" % "foo" }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when multiple width star tokens are given for one format specifier" do
    lambda { "%**s" % [5, 5, 5] }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when a width star token is seen after a width token" do
    lambda { "%5*s" % [5, 5] }.should raise_error(ArgumentError)
  end

  it "raises an ArgumentError when multiple precision tokens are given" do
    lambda { "%.5.5s" % 5      }.should raise_error(ArgumentError)
    lambda { "%.5.*s" % [5, 5] }.should raise_error(ArgumentError)
    lambda { "%.*.5s" % [5, 5] }.should raise_error(ArgumentError)
  end
  
  it "raises an ArgumentError when there are less arguments than format specifiers" do
    ("foo" % []).should == "foo"
    lambda { "%s" % []     }.should raise_error(ArgumentError)
    lambda { "%s %s" % [1] }.should raise_error(ArgumentError)
  end
  
  it "raises an ArgumentError when absolute and relative argument numbers are mixed" do
    lambda { "%s %1$s" % "foo" }.should raise_error(ArgumentError)
    lambda { "%1$s %s" % "foo" }.should raise_error(ArgumentError)

    lambda { "%s %2$s" % ["foo", "bar"] }.should raise_error(ArgumentError)
    lambda { "%2$s %s" % ["foo", "bar"] }.should raise_error(ArgumentError)

    lambda { "%*2$s" % [5, 5, 5]     }.should raise_error(ArgumentError)
    lambda { "%*.*2$s" % [5, 5, 5]   }.should raise_error(ArgumentError)
    lambda { "%*2$.*2$s" % [5, 5, 5] }.should raise_error(ArgumentError)
    lambda { "%*.*2$s" % [5, 5, 5]   }.should raise_error(ArgumentError)
  end
  
  it "allows reuse of the one argument multiple via absolute argument numbers" do
    ("%1$s %1$s" % "foo").should == "foo foo"
    ("%1$s %2$s %1$s %2$s" % ["foo", "bar"]).should == "foo bar foo bar"
  end
  
  it "always interprets an array argument as a list of argument parameters" do
    labmda { "%p" % [] }.should raise_error(ArgumentError)
    ("%p" % [1]).should == "1"
    ("%p %p" % [1, 2]).should == "1 2"
  end

  it "always interprets an array subclass argument as a list of argument parameters" do
    lambda { "%p" % MyArray[] }.should raise_error(ArgumentError)
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
    w.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => true)
    w.should_receive(:method_missing, :with => [:to_int], :returning => 10)
    p = Object.new
    p.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => true)
    p.should_receive(:method_missing, :with => [:to_int], :returning => 5)

    ("%*.*f" % [w, p, 1]).should == "   1.00000"
  end
  
  it "doesn't call to_ary on its argument" do
    obj = Object.new
    def obj.to_ary() [1, 2] end
    def obj.to_s() "obj" end
    lambda { "%s %s" % obj }.should raise_error(ArgumentError)
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
    
    lambda { "%c" % Object }.should raise_error(TypeError)
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
    obj.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => true)
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
    def obj.inspect() "obj" end
    ("%p" % obj).should == "obj"

    # undef is not working
    # obj = Object.new
    # class << obj; undef :inspect; end
    # def obj.method_missing(*args) "obj" end
    # ("%p" % obj).should == "obj"    
  end
  
  it "taints result for %p when argument.inspect is tainted" do
    obj = Object.new
    def obj.inspect() "x".taint end
    
    ("%p" % obj).tainted?.should == true
    
    obj = Object.new; obj.taint
    def obj.inspect() "x" end
    
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
    def obj.to_s() "obj" end
    
    ("%s" % obj).should == "obj"

    # undef doesn't work
    # obj = Object.new
    # class << obj; undef :to_s; end
    # def obj.method_missing(*args) "obj" end
    # 
    # ("%s" % obj).should == "obj"
  end
  
  it "taints result for %s when argument is tainted" do
    ("%s" % "x".taint).tainted?.should == true
    ("%s" % Object.new.taint).tainted?.should == true
    ("%s" % 5.0.taint).tainted?.should == true
  end

  # MRI crashes on this one.
  # See http://groups.google.com/group/ruby-core-google/t/c285c18cd94c216d
  # failure :mri do
  #   it "raises ArgumentError for huge precisions for %s" do
  #     raise_error(ArgumentError) do
  #       "%.25555555555555555555555555555555555555s" % "hello world"
  #     end
  #   end
  # end
  
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
  end
  
  compliant :rubinius do
    # Something's odd for MRI here. For details see
    # http://groups.google.com/group/ruby-core-google/msg/408e2ebc8426f449

    it "supports negative bignums" do
      ("%u" % -(2 ** 64 + 5)).should == "..79228162495817593519834398715"
    end
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
      
      raise_error(ArgumentError) { format % "" }
      raise_error(ArgumentError) { format % "x" }
      raise_error(ArgumentError) { format % "5x" }
      raise_error(ArgumentError) { format % "08" }
      raise_error(ArgumentError) { format % "0b2" }
      raise_error(ArgumentError) { format % "123__456" }
      
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
      obj.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_int], :returning => 65)
      (format % obj).should == (format % 65)

      obj = Object.new
      obj.should_receive(:respond_to?, :count => :any, :with => [:to_int], :returning => false)
      obj.should_receive(:respond_to?, :count => :any, :with => [:to_i], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_i], :returning => 65)
      (format % obj).should == (format % 65)
      
      obj = Object.new
      def obj.respond_to?(arg) true if [:to_i, :to_int].include?(arg) end
      def obj.method_missing(name, *args)
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
      # Something is fucked up with this spec:
      # it works just fine in individual mode, but not when run as part of a group
      (format % "10_1_0.5_5_5").should == (format % 1010.555)
      
      (format % "0777").should == (format % 777)

      raise_error(ArgumentError) { format % "" }
      raise_error(ArgumentError) { format % "x" }
      raise_error(ArgumentError) { format % "." }
      raise_error(ArgumentError) { format % "10." }
      raise_error(ArgumentError) { format % "5x" }
      raise_error(ArgumentError) { format % "0xA" }
      raise_error(ArgumentError) { format % "0b1" }
      raise_error(ArgumentError) { format % "10e10.5" }
      raise_error(ArgumentError) { format % "10__10" }
      raise_error(ArgumentError) { format % "10.10__10" }
      
      obj = Object.new
      obj.should_receive(:to_f, :returning => 5.0)
      (format % obj).should == (format % 5.0)

      obj = Object.new
      obj.should_receive(:respond_to?, :count => :any, :with => [:to_f], :returning => true)
      obj.should_receive(:method_missing, :with => [:to_f], :returning => 3.14)
      (format % obj).should == (format % 3.14)
    end
    
    it "doesn't taint the result for #{format} when argument is tainted" do
      (format % "5".taint).tainted?.should == false
    end
  end
end
