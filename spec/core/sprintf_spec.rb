require File.dirname(__FILE__) + '/../spec_helper'

# tr_s, tr_s!, unpack, upcase, upcase!, upto

context "Sprintf::Parser instance method" do

  setup do
    @src = code do
       @helper = Sprintf::Parser.new("foo wow!", [1,2,3])
    end
  end
  
  specify "initilize object" do
    example(@src) do
      [
        @helper[],
        @helper.cursor,
        @helper.mark,
        @helper.get_argument(0),
        @helper.get_argument(1),
        @helper.get_argument(2)
      ]
    end.should == [ ?f, 0, 0, 1, 2, 3 ]
  end
  
  specify "[] relative string character access" do
    example(@src) do
     @a = []
     @a << @helper[]
     @a << @helper[1]
     @helper.next
     @a << @helper[]
     @a << @helper[3]
     @a
    end.should == [ ?f, ?o, ?o, ?w ]
  end
  
  specify "cursor returns the absolute string position" do
    example(@src) do
      @a = []
      @a << @helper.cursor
      @helper.next
      @a << @helper.cursor
    end.should == [ 0, 1]
  end
  
  specify "next increments the absolute string position" do
    example(@src) do
      @a = []
      @a << @helper.cursor
      @helper.next
      @helper.next
      @a << @helper.cursor
    end.should == [ 0, 2 ]
  end
  
  specify "end_of_string? returns boolean indicating cursor position is off the end of the string" do
    example(@src) do
      @a = []
      while @helper.[] != ?!
        @helper.next
        @a << @helper.end_of_string?
      end
      @a << @helper.end_of_string?
      @helper.next
      @a << @helper.end_of_string?
    end.should == [ false, false, false, false, false, false, false, false, true ]
  end
  
  specify "index returns the character position (or nil) of the searched character" do
    example(@src) do
      @a = []
      @a << @helper.index(?w)
      @a << @helper.cursor
      @helper.rewind
      @a << @helper.index(?&)
      @a << @helper.cursor
      @a << @helper.end_of_string?
    end.should == [ 4, 4, nil, 8, true ]
  end

  specify "mark & drop_mark_point records the current cursor point" do
    example(@src) do
      @a = []
      @a << @helper.cursor
      @a << @helper.mark
      @helper.next
      @helper.next
      @a << @helper.cursor
      @a << @helper.mark
      @helper.drop_mark_point
      @a << @helper.mark
    end.should == [0, 0, 2, 0, 2]
  end
  
  specify "get_mark_to_cursor returns the marked string segment" do
    example(@src) do
      @a = []
      @a << @helper.cursor
      @helper.next
      @helper.drop_mark_point
      @helper.next
      @a << @helper.get_mark_to_cursor
    end.should == [ 0, "o" ]
  end
  
  specify "get_argument returns an items from the argument list" do
    example(@src) do
      @a = []
      @a << @helper.get_argument(0)
      @a << @helper.get_argument(1)
      @a << @helper.get_argument(2)
    end.should == [ 1, 2, 3 ]

    example(@src) do
      @helper.get_argument(4)
    end.should_raise(ArgumentError)
  end
  
  specify "get_next_argument returns the next item from the argument list" do
    example(@src) do
      @a = []
      @a << @helper.get_next_argument
      @a << @helper.get_next_argument
      @a << @helper.get_next_argument
    end.should == [ 1, 2, 3 ]

    example(@scr) do
      @helper.get_next_argument
      @helper.get_next_argument
      @helper.get_next_argument
      @helper.get_next_argument
    end.should_raise(ArgumentError)
  end
  
  specify "get_number return a numerical value from a number string at the cursor position" do
    example do
      @a = []
      @helper = Sprintf::Parser.new("12345 %ABCD %789", 1)
      @a << @helper.get_number
      @a << @helper.cursor
      @helper.index(?%)
      @helper.next
      @a << @helper.get_number
      @a << @helper.cursor
      @helper.index(?%)
      @helper.next
      @a << @helper[]
      begin
        @a << @helper.get_number
      rescue ArgumentError 
        @a << $!.message
      end
      @a
    end.should == [ 12345, 5, 0, 7, ?7, "malformed format string - %.[0-9]" ]
  end
  
  specify "get_argument_value return an argument value from a *nn$ or * string" do
    example do
      @a = []
      @helper = Sprintf::Parser.new("*2$ %* %*789", [1, 2, 3])
      @a << @helper.get_argument_value
      @helper.index(?%)
      @helper.next
      @a << @helper.get_argument_value
      @a << @helper.cursor
      @helper.index(?%)
      @helper.next
      begin
        @a << @helper.get_argument_value
      rescue ArgumentError
        @a << $!.message
      end
    end.should == [ 2, 2, 6, "malformed format string - %.[0-9]" ]
  end
  
end

context "Sprintf::Formatter instance method" do
  setup do
    @src = code do
      @formatter = Sprintf::Formatter.new
    end
  end
  
  specify "flags & flags= returns/sets the flag attribute" do
    example(@src) do
      @formatter.flags = 1
      @formatter.flags
    end.should == 1
  end

  specify "value & value= returns/sets the value attribute" do
    example(@src) do
      @a = []
      @formatter.value = 2
      @a << @formatter.value
      begin
        # value can only be set once
        @formatter.value = 2
        @a << "No Error"
      rescue ArgumentError
        @a << $!.message
      end
     end.should == [ 2, 'value given twice ' ]
  end

  specify "type & type= returns/sets the type attribute" do
    example(@src) do
      @a = []
      @formatter.type = 2
      @a << @formatter.type
      begin
        # type can only be set once
        @formatter.type = 2
        @a << 'No Error'
      rescue ArgumentError
        @a << $!.message
      end
    end.should == [ 2, 'type given twice ' ]
  end

  specify "width & width= returns/sets the width attribute" do
    example(@src) do
      @a = []
      @formatter.width = 2
      @a << @formatter.width
      begin
        # width can only be set once
        @formatter.width = 2
        @a << 'No Error'
      rescue ArgumentError
        @a << $!.message
      end
    end.should == [ 2, 'width given twice ' ]
  end

  specify "precision & precision= returns/sets the precision attribute" do
    example(@src) do
      @a = []
      @formatter.precision = 2
      @a << @formatter.precision
      begin
        # precision can only be set once
        @formatter.precision = 2
        @a << 'No Error'
      rescue ArgumentError
        @a << $!.message
      end
    end.should == [ 2, 'precision given twice' ]
  end

  specify "fill returns the string padded to width" do
    example(@src) do
      @a = []
      @formatter.width = 10
      @a << @formatter.fill("hello")
      @formatter.flags = Sprintf::Formatter::PAD_LJUSTIFY
      @a << @formatter.fill("hello")
      @a << @formatter.fill("12345678901")
    end.should == [ "     hello", "hello     ", "12345678901" ]
  end
  
  specify "truncate returns the string truncated to precision" do
    example(@src) do
      @a = []
      @a << @formatter.truncate("hello")
      @formatter.precision = 3
      @a << @formatter.truncate("hello")
    end.should == [ "hello", "hel" ]
  end
  
  specify "radix returns the radix appended to the string" do
    example(@src) do
      @a = []
      @formatter.type = ?b
      @a << @formatter.radix("1")
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @a << @formatter.radix("1")
    end.should == [ "1", "0b1" ]
  end
  
  specify "radix returns the uppercase binary radix appended to the string" do
    example(@src) do
      @formatter.type = ?B
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @formatter.radix("1")
    end.should == "0B1"
  end
  
  specify "radix returns the octal radix appended to the string" do
    example(@src) do
      @formatter.type = ?o
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @formatter.radix("1")
    end.should == "01"
  end

  specify "radix returns the hexadecimal radix appended to the string" do
    example(@src) do
      @formatter.type = ?x
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @formatter.radix("1")
    end.should == "0x1"
  end
  
  specify "radix returns the uppercase hexadecimal radix appended to the string" do
    example(@src) do
      @formatter.type = ?X
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @formatter.radix("1")
    end.should == "0X1"
  end
  
  specify "radix returns appends no radix for decimal and floating point" do
    example(@src) do
      @formatter.type = ?d
      @formatter.flags = Sprintf::Formatter::PAD_RADIX
      @formatter.radix("1")
    end.should == "1"
  end
  
  specify "onespad returns string left padded with digit relevant to the number base" do
    example(@src) do
      @a = []
      @a << @formatter.onespad("0", 2)
      @formatter.flags = Sprintf::Formatter::PAD_ZERO
      @formatter.width = 10
      @a << @formatter.onespad("0", 2)
      @formatter.precision = 5
      @a << @formatter.onespad("0", 2)
    end.should == [ "0", "1111111110", "11110" ]
  end
  
  specify "onespad returns octal string left padded with max octal digit" do
    example(@src) do
      @formatter.precision = 5
      @formatter.onespad("0", 8)
    end.should == "77770"
  end
  
  specify "onespad returns decimal string left padded with max decimal digit" do
    example(@src) do
      @formatter.precision = 5
      @formatter.onespad("0", 10)
    end.should == "....0"
  end
  
  specify "onespad returns hex string left padded with max hex digit" do
    example(@src) do
      @formatter.precision = 5
      @formatter.onespad("0", 16)
    end.should == "ffff0"
  end
  
  specify "zeropad returns str left padded with 0" do
    example(@src) do
      @a = []
      @a <<  @formatter.zeropad("aaaa", 4)
      @a << @formatter.zeropad("aaaa", 5)
    end.should == [ "aaaa", "0aaaa" ]
  end
  
  specify "sign returns signed string" do
    example(@src) do
      @a = []
      @a << @formatter.sign("1", 1)
      @formatter.flags = Sprintf::Formatter::PAD_PLUS
      @a << @formatter.sign("1", 1)
      @formatter.flags = Sprintf::Formatter::PAD_SPACE
      @a << @formatter.sign("1", 1)
      @a << @formatter.sign("-1", -1)
    end.should == [ "1", "+1", " 1", "-1" ]
  end
  
end

context "Sprintf class method" do

  specify "format with multiple expressions" do
    example do
      Sprintf::Parser.format("%b %x %d %s", [10, 10, 10, 10])
    end.should == "1010 a 10 10"
  end
  
  specify "format with expressions mid string" do
    example do
      Sprintf::Parser.format("hello %s!", "world")
    end.should == "hello world!"
  end
  
  specify "format correctly parses %%" do
    example do
      Sprintf::Parser.format("%d%% %s", [10, "of chickens!"])
    end.should == "10% of chickens!"
  end
  
  specify "format binary values should return a string resulting from applying the format" do
    example { Sprintf::Parser.format("%b", 10) }.should == "1010"
    example { Sprintf::Parser.format("% b", 10)}.should == " 1010"
    example { Sprintf::Parser.format("%1$b",[10, 20]) }.should == "1010"
    example { Sprintf::Parser.format("%#b",10) }.should == "0b1010"
    example { Sprintf::Parser.format("%+b",10) }.should == "+1010"
    example { Sprintf::Parser.format("%-9b",10) }.should == "1010     "
    example { Sprintf::Parser.format("%05b",10) }.should == "01010"
    example { Sprintf::Parser.format("%*b",[10, 6]) }.should == "       110"
  end
  
  specify "format character values should return a string resulting from applying the format" do
    example do
     [
      Sprintf::Parser.format("%c",10),
      Sprintf::Parser.format("%2$c",[10, 11, 14]),
      Sprintf::Parser.format("%-4c",10),
      Sprintf::Parser.format("%*c",[10, 3])
     ]
    end.should == [ "\n", "\v", "\n   ", "         \003" ]
  end
  
  specify "format decimal values should return a string resulting from applying the format" do
   example do
    [
      Sprintf::Parser.format("%d",10),
      Sprintf::Parser.format("% d",10),
      Sprintf::Parser.format("%1$d",[10, 20]),
      Sprintf::Parser.format("%+d",10),
      Sprintf::Parser.format("%-7d",10),
      Sprintf::Parser.format("%04d",10),
      Sprintf::Parser.format("%*d",[10, 4])
    ]
   end.should == [ "10", " 10", "10", "+10", "10     ", "0010", "         4" ]
  end
  
  specify "format float values Initify and Nan" do
    example { Sprintf::Parser.format("%E",1e1020) }.should == "Inf"
    example { Sprintf::Parser.format("%E",-1e1020) }.should == "-Inf"
    example { Sprintf::Parser.format("%-10E",1e1020) }.should == "Inf       "
    example { Sprintf::Parser.format("%-010E",1e1020) }.should == "Inf       "
    example { Sprintf::Parser.format("%010E",1e1020) }.should == "0000000Inf"
    example { Sprintf::Parser.format("%+E",1e1020) }.should == "+Inf"
    example { Sprintf::Parser.format("% E",1e1020) }.should == " Inf"
    example {  Sprintf::Parser.format("%E",1e400) }.should == "Nan"
    example { Sprintf::Parser.format("%E",-1e400) }.should == "Nan"
  end
  
  specify "format float (E) values should return a string resulting from applying the format" do
    example do
     [
      Sprintf::Parser.format("%E",10),
      Sprintf::Parser.format("% E",10),
      Sprintf::Parser.format("%1$E",10),
      Sprintf::Parser.format("%#E",10),
      Sprintf::Parser.format("%+E",10),
      Sprintf::Parser.format("%-7E",10),
      Sprintf::Parser.format("%05E",10),
      Sprintf::Parser.format("%*E",[10, 9])
     ]
    end.should == [ "1.000000E+01", "1.000000E+01", "1.000000E+01", "1.000000E+01", "+1.000000E+01", "1.000000E+01", "1.000000E+01", "9.000000E+00" ]
  end

  specify "format float (e) values should return a string resulting from applying the format" do  
    example do
     [
      Sprintf::Parser.format("%e",10),
      Sprintf::Parser.format("% e",10),
      Sprintf::Parser.format("%1$e",10),
      Sprintf::Parser.format("%#e",10),
      Sprintf::Parser.format("%+e",10),
      Sprintf::Parser.format("%-7e",10),
      Sprintf::Parser.format("%05e",10),
      Sprintf::Parser.format("%*e",[10, 9])
     ]
    end.should == [ "1.000000e+01", "1.000000e+01", "1.000000e+01", "1.000000e+01", "+1.000000e+01", "1.000000e+01", "1.000000e+01", "9.000000e+00" ]
  end
  
  specify "format float (f) values should return a string resulting from applying the format" do
    example do
    [  
    Sprintf::Parser.format("%f",10),
    Sprintf::Parser.format("% f",10),
    Sprintf::Parser.format("%1$f",10),
    Sprintf::Parser.format("%#f",10),
    Sprintf::Parser.format("%+f",10),
    Sprintf::Parser.format("%-7f",10),
    Sprintf::Parser.format("%05f",10),
    Sprintf::Parser.format("%*f",[10, 9]),
    ]
    end.should == [ "10.000000", " 10.000000", "10.000000", "10.000000", "+10.000000", "10.000000", "10.000000", "  9.000000" ]
  end
  
  specify "format float (G) values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%G",10),
    Sprintf::Parser.format("% G",10),
    Sprintf::Parser.format("%1$G",10),
    Sprintf::Parser.format("%#G",10),
    Sprintf::Parser.format("%+G",10),
    Sprintf::Parser.format("%-7G",10),
    Sprintf::Parser.format("%05G",10),
    Sprintf::Parser.format("%*G",[10, 9])
    ]
    end.should == ["10", " 10", "10", "10.0000", "+10", "10     ", "00010", "         9" ]
  end
  
  specify "format float (g) values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%g",10),
    Sprintf::Parser.format("% g",10),
    Sprintf::Parser.format("%1$g",10),
    Sprintf::Parser.format("%#g",10),
    Sprintf::Parser.format("%+g",10),
    Sprintf::Parser.format("%-7g",10),
    Sprintf::Parser.format("%05g",10),
    Sprintf::Parser.format("%*g",[10, 9])
    ]
    end.should == ["10", " 10", "10", "10.0000", "+10", "10     ", "00010", "         9" ]
  end
  
  specify "format integer values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%i",10),
    Sprintf::Parser.format("% i",10),
    Sprintf::Parser.format("%1$i",[10, 20]),
    Sprintf::Parser.format("%+i",10),
    Sprintf::Parser.format("%-7i",10),
    Sprintf::Parser.format("%04i",10),
    Sprintf::Parser.format("%*i",[10, 4])
    ]
    end.should == [ "10", " 10", "10", "+10", "10     ", "0010", "         4" ]
  end
  
  specify "format octal values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%o",10),
    Sprintf::Parser.format("% o",10),
    Sprintf::Parser.format("%1$o",[10, 20]),
    Sprintf::Parser.format("%#o",10),
    Sprintf::Parser.format("%+o",10),
    Sprintf::Parser.format("%-9o",10),
    Sprintf::Parser.format("%05o",10),
    Sprintf::Parser.format("%*o",[10, 6])
    ]
    end.should == [ "12", " 12", "12", "012", "+12", "12       ", "00012", "         6" ]
  end
  
  specify "format inspect values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%p",10),
    Sprintf::Parser.format("%1$p",[10, 5]),
    Sprintf::Parser.format("%-22p",10),
    Sprintf::Parser.format("%*p",[10, 10])
    ]
    end.should == ["10", "10", "10                    ", "        10"] 
  end
  
  specify "format string values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%s",10),
    Sprintf::Parser.format("%1$s",[10, 8]),
    Sprintf::Parser.format("%-5s",10),
    Sprintf::Parser.format("%*s",[10, 9])
    ]
    end.should == [ "10", "10", "10   ", "         9" ]
  end
  
  specify "format unsigned values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%u",10),
    Sprintf::Parser.format("% u",10),
    Sprintf::Parser.format("%1$u",[10, 20]),
    Sprintf::Parser.format("%+u",10),
    Sprintf::Parser.format("%-7u",10),
    Sprintf::Parser.format("%04u",10),
    Sprintf::Parser.format("%*u",[10, 4])
    ]
    end.should == [ "10", " 10", "10", "+10", "10     ", "0010", "         4" ]
  end
  
  specify "format hex (X) values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%X",10),
    Sprintf::Parser.format("% X",10),
    Sprintf::Parser.format("%1$X",[10, 20]),
    Sprintf::Parser.format("%#X",10),
    Sprintf::Parser.format("%+X",10),
    Sprintf::Parser.format("%-9X",10),
    Sprintf::Parser.format("%05X",10),
    Sprintf::Parser.format("%*X",[10, 6])
    ]
    end.should == ["A", " A", "A", "0XA", "+A", "A        ",  "0000A", "         6"]
  end
  
  specify "format hex (x) values should return a string resulting from applying the format" do
    example do
    [
    Sprintf::Parser.format("%x",10),
    Sprintf::Parser.format("% x",10),
    Sprintf::Parser.format("%1$x",[10, 20]),
    Sprintf::Parser.format("%#x",10),
    Sprintf::Parser.format("%+x",10),
    Sprintf::Parser.format("%-9x",10),
    Sprintf::Parser.format("%05x",10),
    Sprintf::Parser.format("%*x",[10, 6])
    ]
    end.should == [ "a", " a", "a", "0xa", "+a", "a        ", "0000a", "         6" ]
  end
  
end

