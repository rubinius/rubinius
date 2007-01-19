require File.dirname(__FILE__) + '/../../spec_helper'

# Thanks http://www.zenspider.com/Languages/Ruby/QuickRef.html

# TODO : HEREDOC
# <<identifier   - interpolated, goes until identifier
# <<"identifier" - same thing
# <<'identifier' - no interpolation
# <<-identifier  - you can indent the identifier by using "-" in front
#
context "Ruby character strings in various ways" do

  setup do
    @ip = 'xxx' # used for interpolation
  end

  specify "with no interpolation" do
    example do
      '#{@ip}'
    end.should == '#{@ip}'
  end

  specify 'interpolation is used with #{your_var}' do
    example do
      "#{@ip}"
    end.should == 'xxx'
  end

  # TODO : Add specs that determine the end of the variable
  specify 'instance variables can also be interpolated just with the # character' do
    example do
      "#@ip"
    end.should == 'xxx'
  end

  # NOTE : What chars are allowed to delimit a string ?
  specify "using percent and different characters to delimit a string" do
    example do
      %(hey hey)
    end.should == "hey hey"

    example do
      %[hey hey]
    end.should == "hey hey"

    example do
      %{hey hey}
    end.should == "hey hey"

    example do
      %@hey hey@
    end.should == "hey hey"

    example do
      %!hey hey!
    end.should == "hey hey"
  end

  specify "using percent with 'q' should stop interpolation" do
    example do
      %q(#{@ip})
    end.should == '#{@ip}'
  end

  # NOTE : I'm not sure why this is needed. IMHO it's redundant with %().
  specify "using percent with 'Q' should force interpolation" do
    example do
      %Q(#{@ip})
    end.should == 'xxx'
  end

  # The backslashes :
  #
  # \t (tab), \n (newline), \r (carriage return), \f (form feed), \b
  # (backspace), \a (bell), \e (escape), \s (whitespace), \nnn (octal),
  # \xnn (hexadecimal), \cx (control x), \C-x (control x), \M-x (meta x),
  # \M-\C-x (meta control x)
  
  specify "backslashes follow the same rules as interpolation" do
    example do
      "\t\n\r\f\b\a\e\s\075\x62\cx"
    end.should == "\t\n\r\f\b\a\e =b\030"
    example do
      '\t\n\r\f\b\a\e =b\030'
    end.should == "\\t\\n\\r\\f\\b\\a\\e =b\\030"
  end

end
