require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/unit'

describe UnitdiffFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = UnitdiffFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
    @formatter.exception @exception
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
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

  it "provides a summary" do
    @formatter.stub!(:print_time).and_return { @out.print "Finished in 33.000000 seconds\n\n" }
    @formatter.summary
    @out.should == "\n\nFinished in 33.000000 seconds\n\n1)\ndescribe it ERROR\nsomething bad: \n\n\n1 example, 0 expectations, 0 failures, 1 error\n"
  end
end
