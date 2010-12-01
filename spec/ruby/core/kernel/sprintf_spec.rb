require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#sprintf" do
  it "is a private method" do
    Kernel.should have_private_instance_method(:sprintf)
  end
  
  it "treats nil arguments as zero-width strings in %s slots" do
    sprintf("%s%d%s%s", nil, 4, 'a', 'b').should == '4ab'
  end

  ruby_version_is ""..."1.9" do
    it "treats nil arguments as zeroes in %d slots" do
      sprintf("%d%d%s%s", nil, 4, 'a', 'b').should == '04ab'
    end
  end

  it "passes some tests for positive %x" do
    sprintf("%x", 123).should == "7b"
    sprintf("%0x", 123).should == "7b"
    sprintf("% x", 123).should == " 7b"
    sprintf("%+x", 123).should == "+7b"
    sprintf("%+0x", 123).should == "+7b"
    sprintf("%+ x", 123).should == "+7b"
    sprintf("% 0x", 123).should == " 7b"

    sprintf("%#x", 123).should == "0x7b"
    sprintf("%#0x", 123).should == "0x7b"
    sprintf("%# x", 123).should == " 0x7b"
    sprintf("%#+x", 123).should == "+0x7b"
    sprintf("%#+0x", 123).should == "+0x7b"
    sprintf("%#+ x", 123).should == "+0x7b"
    sprintf("%# 0x", 123).should == " 0x7b"

    sprintf("%8x", 123).should == "      7b"
    sprintf("%08x", 123).should == "0000007b"
    sprintf("% 8x", 123).should == "      7b"
    sprintf("%+8x", 123).should == "     +7b"
    sprintf("%+08x", 123).should == "+000007b"
    sprintf("%+ 8x", 123).should == "     +7b"
    sprintf("% 08x", 123).should == " 000007b"

    sprintf("%#8x", 123).should == "    0x7b"
    sprintf("%#08x", 123).should == "0x00007b"
    sprintf("%# 8x", 123).should == "    0x7b"
    sprintf("%#+8x", 123).should == "   +0x7b"
    sprintf("%#+08x", 123).should == "+0x0007b"
    sprintf("%#+ 8x", 123).should == "   +0x7b"
    sprintf("%# 08x", 123).should == " 0x0007b"

    sprintf("%8.10x", 123).should == "000000007b"
    sprintf("%08.10x", 123).should == "000000007b"
    sprintf("% 8.10x", 123).should == " 000000007b"
    sprintf("%+8.10x", 123).should == "+000000007b"
    sprintf("%+08.10x", 123).should == "+000000007b"
    sprintf("%+ 8.10x", 123).should == "+000000007b"
    sprintf("% 08.10x", 123).should == " 000000007b"

    sprintf("%10.8x", 123).should == "  0000007b"
    sprintf("%010.8x", 123).should == "  0000007b"
    sprintf("% 10.8x", 123).should == "  0000007b"
    sprintf("%+10.8x", 123).should == " +0000007b"
    sprintf("%+010.8x", 123).should == " +0000007b"
    sprintf("%+ 10.8x", 123).should == " +0000007b"
    sprintf("% 010.8x", 123).should == "  0000007b"
  end

  it "passes some tests for negative %x" do
    sprintf("%x", -123).should == "..f85"
    sprintf("%0x", -123).should == "f85"
    sprintf("% x", -123).should == "-7b"
    sprintf("%+x", -123).should == "-7b"
    sprintf("%+0x", -123).should == "-7b"
    sprintf("%+ x", -123).should == "-7b"
    sprintf("% 0x", -123).should == "-7b"

    sprintf("%#x", -123).should == "0x..f85"
    sprintf("%#0x", -123).should == "0xf85"
    sprintf("%# x", -123).should == "-0x7b"
    sprintf("%#+x", -123).should == "-0x7b"
    sprintf("%#+0x", -123).should == "-0x7b"
    sprintf("%#+ x", -123).should == "-0x7b"
    sprintf("%# 0x", -123).should == "-0x7b"

    sprintf("%8x", -123).should == "   ..f85"
    sprintf("%08x", -123).should == "ffffff85"
    sprintf("% 8x", -123).should == "     -7b"
    sprintf("%+8x", -123).should == "     -7b"
    sprintf("%+08x", -123).should == "-000007b"
    sprintf("%+ 8x", -123).should == "     -7b"
    sprintf("% 08x", -123).should == "-000007b"

    sprintf("%#8x", -123).should == " 0x..f85"
    sprintf("%#08x", -123).should == "0xffff85"
    sprintf("%# 8x", -123).should == "   -0x7b"
    sprintf("%#+8x", -123).should == "   -0x7b"
    sprintf("%#+08x", -123).should == "-0x0007b"
    sprintf("%#+ 8x", -123).should == "   -0x7b"
    sprintf("%# 08x", -123).should == "-0x0007b"

    sprintf("%8.10x", -123).should == "ffffffff85"
    sprintf("%08.10x", -123).should == "ffffffff85"
    sprintf("% 8.10x", -123).should == "-000000007b"
    sprintf("%+8.10x", -123).should == "-000000007b"
    sprintf("%+08.10x", -123).should == "-000000007b"
    sprintf("%+ 8.10x", -123).should == "-000000007b"
    sprintf("% 08.10x", -123).should == "-000000007b"

    sprintf("%10.8x", -123).should == "  ffffff85"
    sprintf("%010.8x", -123).should == "  ffffff85"
    sprintf("% 10.8x", -123).should == " -0000007b"
    sprintf("%+10.8x", -123).should == " -0000007b"
    sprintf("%+010.8x", -123).should == " -0000007b"
    sprintf("%+ 10.8x", -123).should == " -0000007b"
    sprintf("% 010.8x", -123).should == " -0000007b"
  end

  it "passes some tests for negative %u" do
    sprintf("% u", -123).should == "-123"
    sprintf("%+u", -123).should == "-123"
    sprintf("%+0u", -123).should == "-123"
    sprintf("%+ u", -123).should == "-123"
    sprintf("% 0u", -123).should == "-123"

    sprintf("%# u", -123).should == "-123"
    sprintf("%#+u", -123).should == "-123"
    sprintf("%#+0u", -123).should == "-123"
    sprintf("%#+ u", -123).should == "-123"
    sprintf("%# 0u", -123).should == "-123"

    sprintf("% 8u", -123).should == "    -123"
    sprintf("%+8u", -123).should == "    -123"
    sprintf("%+08u", -123).should == "-0000123"
    sprintf("%+ 8u", -123).should == "    -123"
    sprintf("% 08u", -123).should == "-0000123"

    sprintf("%# 8u", -123).should == "    -123"
    sprintf("%#+8u", -123).should == "    -123"
    sprintf("%#+08u", -123).should == "-0000123"
    sprintf("%#+ 8u", -123).should == "    -123"
    sprintf("%# 08u", -123).should == "-0000123"


    platform_is :wordsize => 32 do
      sprintf("%u", -123).should == "..4294967173"
      sprintf("%0u", -123).should == "4294967173"
      sprintf("%#u", -123).should == "..4294967173"
      sprintf("%#0u", -123).should == "4294967173"
      sprintf("%8u", -123).should == "..4294967173"
      sprintf("%08u", -123).should == "4294967173"
      sprintf("%#8u", -123).should == "..4294967173"
      sprintf("%#08u", -123).should == "4294967173"

      sprintf("%30u", -123).should == "                  ..4294967173"
      sprintf("%030u", -123).should == "....................4294967173"

      sprintf("%#30u", -123).should == "                  ..4294967173"
      sprintf("%#030u", -123).should == "....................4294967173"

      sprintf("%24.30u", -123).should == "....................4294967173"
      sprintf("%024.30u", -123).should == "....................4294967173"

      sprintf("%#24.30u", -123).should == "....................4294967173"
      sprintf("%#024.30u", -123).should == "....................4294967173"


      sprintf("%30.24u", -123).should == "      ..............4294967173"
      sprintf("%030.24u", -123).should == "      ..............4294967173"

      sprintf("%#30.24u", -123).should == "      ..............4294967173"
      sprintf("%#030.24u", -123).should == "      ..............4294967173"
    end

    platform_is :wordsize => 64 do
      sprintf("%u", -123).should == "..18446744073709551493"
      sprintf("%0u", -123).should == "18446744073709551493"
      sprintf("%#u", -123).should == "..18446744073709551493"
      sprintf("%#0u", -123).should == "18446744073709551493"
      sprintf("%8u", -123).should == "..18446744073709551493"
      sprintf("%08u", -123).should == "18446744073709551493"
      sprintf("%#8u", -123).should == "..18446744073709551493"
      sprintf("%#08u", -123).should == "18446744073709551493"

      sprintf("%30u", -123).should == "        ..18446744073709551493"
      sprintf("%030u", -123).should == "..........18446744073709551493"

      sprintf("%#30u", -123).should == "        ..18446744073709551493"
      sprintf("%#030u", -123).should == "..........18446744073709551493"

      sprintf("%24.30u", -123).should == "..........18446744073709551493"
      sprintf("%024.30u", -123).should == "..........18446744073709551493"

      sprintf("%#24.30u", -123).should == "..........18446744073709551493"
      sprintf("%#024.30u", -123).should == "..........18446744073709551493"


      sprintf("%30.24u", -123).should == "      ....18446744073709551493"
      sprintf("%030.24u", -123).should == "      ....18446744073709551493"

      sprintf("%#30.24u", -123).should == "      ....18446744073709551493"
      sprintf("%#030.24u", -123).should == "      ....18446744073709551493"
    end
  end

  it "passes some tests for positive %u" do
    sprintf("%30u", 123).should == "                           123"
    sprintf("%030u", 123).should == "000000000000000000000000000123"

    sprintf("%#30u", 123).should == "                           123"
    sprintf("%#030u", 123).should == "000000000000000000000000000123"


    sprintf("%24.30u", 123).should == "000000000000000000000000000123"
    sprintf("%024.30u", 123).should == "000000000000000000000000000123"

    sprintf("%#24.30u", 123).should == "000000000000000000000000000123"
    sprintf("%#024.30u", 123).should == "000000000000000000000000000123"


    sprintf("%30.24u", 123).should == "      000000000000000000000123"
    sprintf("%030.24u", 123).should == "      000000000000000000000123"

    sprintf("%#30.24u", 123).should == "      000000000000000000000123"
    sprintf("%#030.24u", 123).should == "      000000000000000000000123"
  end

  it "passes kstephens's tests" do
    sprintf("%*1$.*2$3$d", 10, 5, 1).should == "     00001"
    sprintf("%.7b", -5).should == "1111011"
    sprintf("%.7B", -5).should == "1111011"
    sprintf("%b", 0).should == "0"
    sprintf("%B", 0).should == "0"
    sprintf("%b", -5).should == "..1011"
    sprintf("%B", -5).should == "..1011"
    sprintf("%0b", -5).should == "1011"
    sprintf("%0x", -125).should == "f83"
    sprintf("%+b", -5).should == "-101"
    sprintf("%+b", 10).should == "+1010"
    sprintf("%+b", 0).should == "+0"
    sprintf("%+o", -5).should == "-5"
    sprintf("%+o", 10).should == "+12"
    sprintf("%+o", 0).should == "+0"
    sprintf("%+d", -5).should == "-5"
    sprintf("%+d", 10).should == "+10"
    sprintf("%+d", 0).should == "+0"
    sprintf("%+x", -15).should == "-f"
    sprintf("%+x", 100).should == "+64"
    sprintf("%+x", 0).should == "+0"
    sprintf("%+X", -15).should == "-F"
    sprintf("%+X", 100).should == "+64"
    sprintf("%+X", 0).should == "+0"
    sprintf("=%02X", 1).should == "=01"
    sprintf("%+03d", 0).should == "+00"
    sprintf("%+03d", 5).should == "+05"
    sprintf("%+03d", -5).should == "-05"
    sprintf("%+03d", 12).should == "+12"
    sprintf("%+03d", -12).should == "-12"
    sprintf("%+03d", 123).should == "+123"
    sprintf("%+03d", -123).should == "-123"
  end
end

describe "Kernel.sprintf" do
  it "needs to be reviewed for spec completeness"
end
