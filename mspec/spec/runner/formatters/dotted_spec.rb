require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/dotted'

describe DottedFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = DottedFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
  end
  
  it "responds to after_it with one argument" do
    @formatter.after_it "after"
  end

  it "provides execution results when after_it is called" do
    @formatter.before_it "it"
    @formatter.after_it "first"
    @out.should == "."
    @formatter.before_it "it"
    @formatter.exception @exception
    @formatter.after_it "second"
    @out.should == ".E"
  end
end
