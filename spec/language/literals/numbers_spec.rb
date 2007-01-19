require File.dirname(__FILE__) + '/../../spec_helper'

context "Ruby numbers in various ways" do

  specify "the standard way" do
    example do
      435
    end.should == 435
  end

  specify "with underscore separations" do
    example do
      4_35
    end.should == 435
  end

  specify "with some decimals" do
    example do
      4.35
    end.should == 4.35
  end

  # TODO : find a better description
  specify "using the e expression" do
    example do
      1.2e-3
    end.should == 0.0012
  end

  specify "the hexdecimal notation" do
    example do
      0xffff
    end.should == 65535
  end

  specify "the binary notation" do
    example do
      0b01011
    end.should == 11
  end

  specify "octal representation" do
    example do
      0377
    end.should == 255
  end

  specify "character to numeric shortcut" do
    example do
      ?z
    end.should == 122
  end

  specify "character with control character to numeric shortcut" do
    # Control-Z
    example do
      ?\C-z
    end.should == 26

    # Meta-Z
    example do
      ?\M-z
    end.should == 250

    # Meta-Control-Z
    example do
      ?\M-\C-z
    end.should == 154
  end

end
