require File.expand_path('../../spec_helper', __FILE__)

describe "Processing RUBYOPT" do
  before (:each) do
    @rubyopt, ENV["RUBYOPT"] = ENV["RUBYOPT"], nil
    @ruby_flags, ENV["RUBY_FLAGS"] = ENV["RUBY_FLAGS"], nil
  end

  after (:each) do
    ENV["RUBYOPT"] = @rubyopt
    ENV["RUBY_FLAGS"] = @ruby_flags
  end

  it "adds the -I path to $LOAD_PATH" do
    ENV["RUBYOPT"] = "-Ioptrubyspecincl"
    result = ruby_exe("puts $LOAD_PATH.grep(/byspecin/)", :escape => true)
    result.chomp[-15..-1].should == "optrubyspecincl"
  end

  it "sets $DEBUG to true for '-d'" do
    ENV["RUBYOPT"] = '-d'
    command = %[puts "value of $DEBUG is \#{$DEBUG}"]
    result = ruby_exe(command, :escape => true, :args => "2>&1")
    result.should =~ /value of \$DEBUG is true/
  end

  it "prints the version number for '-v'" do
    ENV["RUBYOPT"] = '-v'
    ruby_exe("").chomp.should == RUBY_DESCRIPTION
  end

  it "sets $VERBOSE to true for '-w'" do
    ENV["RUBYOPT"] = '-w'
    ruby_exe("p $VERBOSE", :escape => true).chomp.should == "true"
  end

  it "sets $VERBOSE to true for '-W'" do
    ENV["RUBYOPT"] = '-W'
    ruby_exe("p $VERBOSE", :escape => true).chomp.should == "true"
  end

  it "sets $VERBOSE to nil for '-W0'" do
    ENV["RUBYOPT"] = '-W0'
    ruby_exe("p $VERBOSE", :escape => true).chomp.should == "nil"
  end

  it "sets $VERBOSE to false for '-W1'" do
    ENV["RUBYOPT"] = '-W1'
    ruby_exe("p $VERBOSE", :escape => true).chomp.should == "false"
  end

  it "sets $VERBOSE to true for '-W2'" do
    ENV["RUBYOPT"] = '-W2'
    ruby_exe("p $VERBOSE", :escape => true).chomp.should == "true"
  end

  it "requires the file for '-r'" do
    f = fixture __FILE__, "rubyopt"
    ENV["RUBYOPT"] = "-r#{f}"
    ruby_exe("").should =~ /^rubyopt.rb required/
  end
end
