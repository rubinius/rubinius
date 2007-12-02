require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/immediate'

describe ImmediateFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = ImmediateFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
    @formatter.exception @exception
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
  end
  
  it "provides after_it with one argument that prints immediately" do
    @formatter.after_it "after"
    @out.should == "F\ndescribe it FAILED\nsomething bad: \n\n\n"
  end
  
  it "provides a summary" do
    @formatter.summary
    @out.should == "\n\n1 examples, 1 failures\n"
  end
end
