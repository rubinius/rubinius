require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../mspec'

describe MSpec, ".protect" do
  before :each do
    @ss = mock('SpecState')
    @ss.stub!(:exceptions).and_return([])
    @rs = mock('RunState')
    @rs.stub!(:state).and_return(@ss)
    @exception = Exception.new("Sharp!")
  end
  
  it "rescues any exceptions raised when executing the block argument" do
    MSpec.stack.push @rs
    lambda {
      MSpec.protect("") { raise Exception, "Now you see me..." }
    }.should_not raise_error
  end
  
  it "records the exception in the current.state object's exceptions" do
    MSpec.stack.push @rs
    MSpec.protect("testing") { raise @exception }
    @ss.exceptions.should == [["testing", @exception]]
  end
  
  it "writes a message to STDERR if current is nil" do
    STDERR.should_receive(:write).with("An exception occurred in testing: Exception: Sharp!")
    MSpec.stack.clear
    MSpec.protect("testing") { raise @exception}
  end
  
  it "writes a message to STDERR if current.state is nil" do
    STDERR.should_receive(:write).with("An exception occurred in testing: Exception: Sharp!")
    @rs.stub!(:state).and_return(nil)
    MSpec.stack.push @rs
    MSpec.protect("testing") { raise @exception}
  end
end

describe MSpec, ".stack" do
  it "returns an array" do
    MSpec.stack.should be_kind_of(Array)
  end
end

describe MSpec, ".current" do
  it "returns the top of the execution stack" do
    MSpec.stack.clear
    MSpec.stack.push :a
    MSpec.stack.push :b
    MSpec.current.should == :b
  end
end

describe MSpec, ".verify_mode?" do
  it "returns false" do
    MSpec.verify_mode?.should == false
  end
end
