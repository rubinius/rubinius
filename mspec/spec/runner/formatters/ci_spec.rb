require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/formatters/ci'

describe CIFormatter do
  before :each do
    @out = CaptureOutput.new
    @formatter = CIFormatter.new(@out)
    @formatter.before_describe "describe"
    @formatter.before_it "it"
    @exception = Exception.new("something bad")
    @formatter.exception @exception
    @execution = SpecExecution.new
    @execution.describe = "describe"
    @execution.it = "it"
    @execution.exception = @exception
  end
  
  it "provides after_it that prints the spec description if an exception occurred" do
    @formatter.after_it("it")
    @out.should == "describe it\n"
  end
  
  it "overrides print_start to not output" do
    @formatter.print_start
    @out.should == ""
  end
end
