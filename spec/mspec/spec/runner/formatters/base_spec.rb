require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/base'

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
  
  it "provides print_failure with an index and SpecExecution instance" do
    @formatter.print_failure(1, @execution)
    @out.should == "1)\ndescribe it FAILED\n"
  end
  
  it "provides print_exception with an Exception instance" do
    @formatter.print_backtrace(@exception)
    @out.should == "something bad: \n\n\n"
  end
  
  it "provides print_summary" do
    @formatter.print_summary
    @out.should == "1 examples, 1 failures\n"
  end
  
  it "provides a summary" do
    @formatter.summary
    @out.should == "\n\n1)\ndescribe it FAILED\nsomething bad: \n\n\n1 examples, 1 failures\n"
  end
end
