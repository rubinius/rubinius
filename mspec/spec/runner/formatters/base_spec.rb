require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/base'
require File.dirname(__FILE__) + '/../../../expectations'

describe SpecExecution do
  before :each do
    @execution = SpecExecution.new
  end
  
  it "provides a setter for 'exception'" do
    @execution.exception = :exception
  end
  
  it "provides a getter for 'exception'" do
    @execution.exception = :exception
    @execution.exception.should == :exception
  end
  
  it "provides a setter for 'describe' that converts the argument with #to_s" do
    value = 'describe'
    value.should_receive(:to_s)
    @execution.describe = value
  end
  
  it "provides a getter for 'describe'" do
    @execution.describe = :describe
    @execution.describe.should == 'describe'
  end
  
  it "provides a setter for 'it' that converts the argument with #to_s" do
    value = 'it'
    value.should_receive(:to_s)
    @execution.it = value
  end
  
  it "provides a getter for 'it'" do
    @execution.it = :it
    @execution.it.should == 'it'
  end
end

describe SpecTally do
  before :each do
    @tally = SpecTally.new
  end
  
  it "initializes all counters to 0" do
    @tally.examples.should == 0
    @tally.expectations.should == 0
    @tally.failures.should == 0
    @tally.errors.should == 0
  end
  
  it "increments the examples count with each call to #example" do
    @tally.example
    @tally.example
    @tally.examples.should == 2
  end
  
  it "increments the expectations count with each call to #expectation" do
    @tally.expectation
    @tally.expectation
    @tally.expectations.should == 2
  end
  
  it "increments the failures count with each call to #failure" do
    @tally.failure
    @tally.failure
    @tally.failures.should == 2
  end
  
  it "increments the errors count with each call to #error" do
    @tally.error
    @tally.error
    @tally.errors.should == 2
  end
end

describe BaseFormatter, "interface" do
  before :each do
    @out = CaptureOutput.new
    @formatter = BaseFormatter.new(@out)
  end
  
  it "uses STDOUT as the default output object" do
    BaseFormatter.new.out.should == STDOUT
  end
  
  it "accepts an output object" do
    BaseFormatter.new(@out)
  end
  
  it "provides a setter for 'out'" do
    @formatter.out = @out
  end
  
  it "provides a getter for 'out'" do
    @formatter.out.should == @out
  end
  
  it "provides a getter for 'tally'" do
    @formatter.tally.should be_kind_of(SpecTally)
  end
  
  it "provides #current that returns the currently executing spec information object" do
    @formatter.current.should be_an_instance_of(SpecExecution)
  end
  
  it "responds to before_describe with one argument" do
    @formatter.before_describe "before"
  end
  
  it "responds to after_describe with one argument" do
    @formatter.after_describe "after"
  end
  
  it "responds to before_it with one argument" do
    @formatter.before_it "before"
  end
  
  it "responds to after_it with one argument" do
    @formatter.after_it "after"
  end
  
  it "responds to exception with one argument" do
    @formatter.before_it "before"
    @formatter.exception "exception"
  end
  
  it "provides a setter for 'summarized'" do
    @formatter.summarized = true
  end
  
  it "responds to start_timer" do
    @formatter.start_timer
  end
  
  it "responds to stop_timer that returns a string representing elapsed time" do
    Time.stub!(:now).and_return(Time.parse('10/18/2007 5:43:18'))
    @formatter.start_timer
    Time.stub!(:now).and_return(Time.parse('10/18/2007 5:43:51'))
    @formatter.stop_timer.should == "33.000000"
  end
end

describe BaseFormatter, "operation" do
  before :each do
    @out = CaptureOutput.new
    @formatter = BaseFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
    @formatter.exception @exception
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
  end
  
  it "provides failure that returns true if exeception is a ExpectationNotMetError" do
    @formatter.failure?(ExpectationNotMetError.new("unmet expectation")).should == true
    @formatter.failure?(Exception.new("error")).should == false
  end

  it "provides print_start" do
    @formatter.print_start
    @out.should == "Started\n"
  end
  
  it "provides print_time" do
    @formatter.stub!(:stop_timer).and_return("33.000000")
    @formatter.print_time
    @out.should == "Finished in 33.000000 seconds\n\n"
  end

  it "provides print_failure with an index and SpecExecution instance" do
    @formatter.print_failure(1, @execution)
    @execution.exception = ExpectationNotMetError.new("failure")
    @formatter.print_failure(2, @execution)
    @out.should == "1)\ndescribe it ERROR\n2)\ndescribe it FAILED\n"
  end
  
  it "provides print_exception with an Exception instance" do
    @formatter.print_backtrace(@exception)
    @out.should == "something bad: \n\n\n"
  end
  
  it "provides print_summary" do
    @formatter.print_summary
    @out.should == "1 example, 0 expectations, 0 failures, 1 error\n"
  end
  
  it "provides a summary" do
    @formatter.stub!(:print_time).and_return { @out.print "Finished in 33.000000 seconds\n\n" }
    @formatter.summary
    @out.should == "\n\n1)\ndescribe it ERROR\nsomething bad: \n\n\nFinished in 33.000000 seconds\n\n1 example, 0 expectations, 0 failures, 1 error\n"
  end
end
