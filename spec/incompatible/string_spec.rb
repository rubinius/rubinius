require File.dirname(__FILE__) + '/../spec_helper'

context "String instance method" do
  specify "rstrip should return a string with all trailing \\000 and whitespace characters removed" do
    example do
      [ " \t\n ".rstrip,
        "\t".rstrip,
        "".rstrip,
        " hello ".rstrip,
        "\tgoodbye\r\n".rstrip,
        "goodbye \000".rstrip,
        "goodbye \000\t \f  \000".rstrip ]
    end.should == ["", "", "", " hello", "\tgoodbye", "goodbye", "goodbye"]
  end

  specify "rstrip! should modify self by removing all trailing \\000 and whitespace characters" do
    example do
      @s = " hello "
      @t = "\tgoodbye\r\n"
      @u = "goodbye \000"
      @v = "goodbye \000 "
      @w = ""
      @z = " \n \000\v\000"
      [ @s.rstrip!,
        @t.rstrip!,
        @u.rstrip!,
        @v.rstrip!,
        @w.rstrip!,
        @z.rstrip! ]
    end.should == [" hello", "\tgoodbye", "goodbye", "goodbye", "", ""]
  end
end
