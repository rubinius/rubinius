require File.dirname(__FILE__) + '/../../spec_helper'

describe CaptureOutput do
  before :each do
    @out = CaptureOutput.new
    @sep = $\
  end
  
  after :each do
    $\ = @sep
  end

  it "provides a write method" do
    @out.write "this"
    @out.should == "this"
  end
  
  it "concatenates the arguments sent to write" do
    @out.write "flim ", "flam"
    @out.should == "flim flam"
  end
  
  it "provides a print method that appends the default separator" do
    $\ = " [newline] "
    @out.print "hello"
    @out.print "world"
    @out.should == "hello [newline] world [newline] "
  end
end
