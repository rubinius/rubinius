require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#warn" do
  before :each do
    @before_verbose = $VERBOSE
    @before_separator = $/
  end

  after :each do
    $VERBOSE = @before_verbose
    $/ = @before_separator
  end

  it "is a private method" do
    Kernel.should have_private_instance_method(:warn)
  end

  it "calls #write on $stderr if $VERBOSE is true" do
    lambda {
      $VERBOSE = true
      warn("this is some simple text")
    }.should output(nil, "this is some simple text\n")
  end

  it "calls #write on $stderr if $VERBOSE is false" do
    lambda {
      $VERBOSE = false
      warn("this is some simple text")
    }.should output(nil, "this is some simple text\n")
  end

  it "does not call #write on $stderr if $VERBOSE is nil" do
    lambda {
      $VERBOSE = nil
      warn("this is some simple text")
    }.should output(nil, "")
  end

  it "writes the default record separator and NOT $/ to $stderr after the warning message" do
    lambda {
      $VERBOSE = true
      $/ = 'rs'
      warn("")
    }.should output(nil, /\n/)
  end
end
