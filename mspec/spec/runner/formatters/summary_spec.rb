require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/summary'

describe SummaryFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = SummaryFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
    @formatter.exception @exception
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
  end

  it "only outputs backtraces, time, and count summary" do
    @formatter.stub!(:print_time).and_return { @out.print "Finished in 33.000000 seconds\n\n" }
    @formatter.before_describe "before describe"
    @formatter.before_it "before it"
    @formatter.after_it "after it"
    @formatter.after_describe "after describe"
    @formatter.summary
    @out.should == "\n\n1)\ndescribe it ERROR\nsomething bad: \n\n\nFinished in 33.000000 seconds\n\n2 examples, 0 expectations, 0 failures, 1 error\n"
  end
end
