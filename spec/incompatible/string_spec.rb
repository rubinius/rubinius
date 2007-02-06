require File.dirname(__FILE__) + '/../spec_helper'

context "String instance method" do
  specify "lstrip should return a string with all leading \\000 and whitespace characters removed" do
    example do
      [ "".lstrip,
        " hello ".lstrip,
        "\000  hello ".lstrip,
        "\000\t \000hello ".lstrip,
        "hello".lstrip ]
    end.should == ["", "hello ", "hello ", "hello ", "hello"]
  end

  specify "lstrip! should modify self by removing all leading \\000 and whitespace characters" do
    example do
      @s = "\n\t This \000"
      @t = "\000 another one"
      @u = "\000  \000\t\v\000two  "
      [@s.lstrip!, @t.lstrip!, @u.lstrip!]
    end.should == ["This \000", "another one", "two  "]
  end
  
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
    end.should == [" hello", "\tgoodbye", "goodbye", "goodbye", nil, ""]
  end
end
