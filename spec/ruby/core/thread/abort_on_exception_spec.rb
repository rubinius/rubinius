require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Thread.abort_on_exception" do
  before do
    @abort_on_exception = Thread.abort_on_exception
  end

  after do
     Thread.abort_on_exception = @abort_on_exception
  end

  it "is false by default" do
    Thread.abort_on_exception.should == false
  end

  it "returns true when .abort_on_exception= is passed true" do
    Thread.abort_on_exception = true
    Thread.abort_on_exception.should be_true
  end

  it "raises the exception raised in a Thread in Thread.main when true" do
    script = fixture __FILE__, "abort_on_exception_class.rb"

    ruby_exe(script, :args => "2>&1").split("\n").join.should =~ /RuntimeError/
  end
end

describe "Thread#abort_on_exception" do
  before do
    ThreadSpecs.clear_state
    @thread = Thread.new { Thread.pass until ThreadSpecs.state == :exit }
  end

  after do
    ThreadSpecs.state = :exit
  end

  it "is false by default" do
    @thread.abort_on_exception.should be_false
  end

  it "returns true when #abort_on_exception= is passed true" do
    @thread.abort_on_exception = true
    @thread.abort_on_exception.should be_true
  end

  it "raises the exception raised in a Thread in Thread.main when true" do
    script = fixture __FILE__, "abort_on_exception_instance.rb"

    ruby_exe(script, :args => "2>&1").split("\n").join.should =~ /RuntimeError/
  end
end
