require File.dirname(__FILE__) + '/../spec_helper'

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

  it "interpolates the return value of Object#to_s" do
    obj = mock('to_s')
    obj.stub!(:to_s).and_return('42')

    "#{obj}".should == '42'
  end

  it "interpolates the return value of Object#inspect, without ivars, if Object#to_s does not return a String instance" do
    obj = mock('to_s')
    obj.stub!(:to_s).and_return(42)
    s = "#{obj}"[0..-2]

    s.should == obj.inspect[0, s.size]
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
