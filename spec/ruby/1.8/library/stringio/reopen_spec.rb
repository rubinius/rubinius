require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

# NOTE: Some reopen specs disabled due to MRI bugs. See:
# http://rubyforge.org/tracker/index.php?func=detail&aid=13919&group_id=426&atid=1698
# for details.
describe "StringIO#reopen" do
  before(:each) do
    @io = StringIO.new('hello','a')
  end

  # TODO: find out if this is really a bug
  ruby_bug do
    it "reopens a stream when given a String argument" do
      @io.reopen('goodbye').should == @io
      @io.string.should == 'goodbye'
      @io << 'x'
      @io.string.should == 'xoodbye'
    end

    it "reopens a stream in append mode when flagged as such" do
      @io.reopen('goodbye', 'a').should == @io
      @io.string.should == 'goodbye'
      @io << 'x'
      @io.string.should == 'goodbyex'
    end

    it "reopens and truncate when reopened in write mode" do
      @io.reopen('goodbye', 'wb').should == @io
      @io.string.should == ''
      @io << 'x'
      @io.string.should == 'x'
    end

    it "truncates the given string, not a copy" do
      str = 'goodbye'
      @io.reopen(str, 'w')
      @io.string.should == ''
      str.should == ''
    end
  end

  compliant_on :ruby, :jruby do
    it "denies access to prevent truncation of a frozen string" do
      @io = StringIO.new("ice")
      lambda { @io.reopen("burn".freeze, 'w') }.should raise_error(Errno::EACCES)
      lambda { @io.reopen("burn".freeze, 'a') }.should raise_error(Errno::EACCES)
    end

    it "does not raise IOError if a frozen string is passed in read mode" do
      @io.reopen("burn".freeze, 'r')
      @io.string.should == "burn"
    end
  end

  # MRI refuses to convert objects that support to_str, JRuby and Rubinius can
  deviates_on(:jruby, :rubinius) do
    it "calls to_str on the first argument if it is not a String" do
      obj = mock('reopen')
      def obj.to_str; "reopen"; end
      @io.reopen(obj)
      @io.string.should == "reopen"
    end
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { @io.reopen(obj) }.should raise_error(TypeError)
  end
  
  it "reopens a stream when given a new StringIO object" do
    @io.close
    nio = StringIO.new('goodbye')
    @io.reopen(nio)
    @io.closed?.should == false
    @io.string.should == 'goodbye'
  end
end
