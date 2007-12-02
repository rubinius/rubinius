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
  
  it "provides print_failure with an index and SpecExecution instance" do
    @formatter.print_failure(1, @execution)
    @out.should == "describe it\n"
  end

  it "provides a summary" do
    @formatter.summary
    @out.should == "describe it\n"
  end
end
