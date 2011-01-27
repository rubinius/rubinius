require File.expand_path('../../spec_helper', __FILE__)

# Thanks http://www.zenspider.com/Languages/Ruby/QuickRef.html

describe "Ruby character strings" do

  before(:each) do
    @ip = 'xxx' # used for interpolation
  end

  it "don't get interpolated when put in single quotes" do
    '#{@ip}'.should == '#{@ip}'
  end

  it 'get interpolated with #{} when put in double quotes' do
    "#{@ip}".should == 'xxx'
  end

  it "interpolate instance variables just with the # character" do
    "#@ip".should == 'xxx'
  end

  it "interpolate global variables just with the # character" do
    $ip = 'xxx'
    "#$ip".should == 'xxx'
  end

  it "interpolate class variables just with the # character" do
    @@ip = 'xxx'
    "#@@ip".should == 'xxx'
  end

  it "allow underscore as part of a variable name in a simple interpolation" do
    @my_ip = 'xxx'
    "#@my_ip".should == 'xxx'
  end

  it "have characters [.(=?!# end simple # interpolation" do
    "#@ip[".should == 'xxx['
    "#@ip.".should == 'xxx.'
    "#@ip(".should == 'xxx('
    "#@ip=".should == 'xxx='
    "#@ip?".should == 'xxx?'
    "#@ip!".should == 'xxx!'
    "#@ip#@ip".should == 'xxxxxx'
  end

  it "allow using non-alnum characters as string delimiters" do
    %(hey #{@ip}).should == "hey xxx"
    %[hey #{@ip}].should == "hey xxx"
    %{hey #{@ip}}.should == "hey xxx"
    %<hey #{@ip}>.should == "hey xxx"
    %!hey #{@ip}!.should == "hey xxx"
    %@hey #{@ip}@.should == "hey xxx"
    %#hey hey#.should == "hey hey"
    %%hey #{@ip}%.should == "hey xxx"
    %^hey #{@ip}^.should == "hey xxx"
    %&hey #{@ip}&.should == "hey xxx"
    %*hey #{@ip}*.should == "hey xxx"
    %-hey #{@ip}-.should == "hey xxx"
    %_hey #{@ip}_.should == "hey xxx"
    %=hey #{@ip}=.should == "hey xxx"
    %+hey #{@ip}+.should == "hey xxx"
    %~hey #{@ip}~.should == "hey xxx"
    %:hey #{@ip}:.should == "hey xxx"
    %;hey #{@ip};.should == "hey xxx"
    %"hey #{@ip}".should == "hey xxx"
    %|hey #{@ip}|.should == "hey xxx"
    %?hey #{@ip}?.should == "hey xxx"
    %/hey #{@ip}/.should == "hey xxx"
    %,hey #{@ip},.should == "hey xxx"
    %.hey #{@ip}..should == "hey xxx"

    # surprised? huh
    %'hey #{@ip}'.should == "hey xxx"
    %\hey #{@ip}\.should == "hey xxx"
    %`hey #{@ip}`.should == "hey xxx"
    %$hey #{@ip}$.should == "hey xxx"
  end

  it "using percent with 'q', stopping interpolation" do
    %q(#{@ip}).should == '#{@ip}'
  end

  it "using percent with 'Q' to interpolate" do
    %Q(#{@ip}).should == 'xxx'
  end

  # The backslashes :
  #
  # \t (tab), \n (newline), \r (carriage return), \f (form feed), \b
  # (backspace), \a (bell), \e (escape), \s (whitespace), \nnn (octal),
  # \xnn (hexadecimal), \cx (control x), \C-x (control x), \M-x (meta x),
  # \M-\C-x (meta control x)

  it "backslashes follow the same rules as interpolation" do
    "\t\n\r\f\b\a\e\s\075\x62\cx".should == "\t\n\r\f\b\a\e =b\030"
    '\t\n\r\f\b\a\e =b\030'.should == "\\t\\n\\r\\f\\b\\a\\e =b\\030"
  end

  it "allow HEREDOC with <<identifier, interpolated" do
    s = <<HERE
foo bar#{@ip}
HERE
    s.should == "foo barxxx\n"
  end

  it 'allow HEREDOC with <<"identifier", interpolated' do
    s = <<"HERE"
foo bar#{@ip}
HERE
    s.should == "foo barxxx\n"
  end

  it "allow HEREDOC with <<'identifier', no interpolation" do
    s = <<'HERE'
foo bar#{@ip}
HERE
    s.should == 'foo bar#{@ip}' + "\n"
  end

  it "allow HEREDOC with <<-identifier, allowing to indent identifier, interpolated" do
    s = <<-HERE
    foo bar#{@ip}
    HERE

    s.should == "    foo barxxx\n"
  end

  it 'allow HEREDOC with <<-"identifier", allowing to indent identifier, interpolated' do
    s = <<-"HERE"
    foo bar#{@ip}
    HERE

    s.should == "    foo barxxx\n"
  end

  it "allow HEREDOC with <<-'identifier', allowing to indent identifier, no interpolation" do
    s = <<-'HERE'
    foo bar#{@ip}
    HERE

    s.should == '    foo bar#{@ip}' + "\n"
  end

  it "call #to_s when the object is not a String" do
    obj = mock('to_s')
    obj.stub!(:to_s).and_return('42')

    "#{obj}".should == '42'
  end

  it "call #to_s as a private method" do
    obj = mock('to_s')
    obj.stub!(:to_s).and_return('42')

    class << obj
      private :to_s
    end

    "#{obj}".should == '42'
  end

  it "uses an internal representation when #to_s doesn't return a String" do
    obj = mock('to_s')
    obj.stub!(:to_s).and_return(42)

    # See rubyspec commit 787c132d by yugui. There is value in
    # ensuring that this behavior works. So rather than removing
    # this spec completely, the only thing that can be asserted
    # is that if you interpolate an object that fails to return
    # a String, you will still get a String and not raise an
    # exception.
    "#{obj}".should be_an_instance_of(String)
  end

  it "allow a dynamic string to parse a nested do...end block as an argument to a call without parens, interpolated" do
    s = eval 'eval "#{proc do; 1; end.call}"'
    s.should == 1
  end

  ruby_version_is '1.9' do
    it "are produced from character shortcuts" do
      ?z.should == 'z'
    end

    it "are produced from control character shortcuts" do
      # Control-Z
      ?\C-z.should == "\x1A"

      # Meta-Z
      ?\M-z.should == "\xFA"

      # Meta-Control-Z
      ?\M-\C-z.should == "\x9A"
    end
  end
end
