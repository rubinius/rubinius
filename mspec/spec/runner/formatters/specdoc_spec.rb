require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/specdoc'

describe SpecdocFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = SpecdocFormatter.new(@out)
    @formatter.before_describe "describe"
    @exception = Exception.new("something bad")
  end
  
  it "responds to before_describe with one argument" do
    @formatter.before_describe "before"
  end
  
  it "responds to print_describe with no arguments" do
    @formatter.print_describe
    @out.should == "\ndescribe\n"
  end
  
  it "responds to before_it with one argument" do
    @formatter.before_it "it"
    @out.should == "\ndescribe\n- it"
  end
  
  it "responds to after_it with one argument" do
    @formatter.after_it "after"
    @out.should == "\n"
  end
  
  it "responds to exception with one argument" do
    @formatter.exception @exception
    @out.should == " (ERROR - 1)"
  end
end
