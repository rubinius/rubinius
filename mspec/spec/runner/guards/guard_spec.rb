require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/../../../runner/guards/guard'

describe SpecGuard, ".register" do
  before :each do
    @tally = mock("tally")
    @tally.stub!(:register)
    TallyAction.stub!(:new).and_return(@tally)
    SpecGuard.instance_variable_set(:@registered, nil)
  end

  it "creates a new TallyAction if one does not exist" do
    TallyAction.should_receive(:new).and_return(@tally)
    @tally.should_receive(:register)
    SpecGuard.register
    SpecGuard.register
  end
  
  it "registers itself with MSpec :finish actions" do
    MSpec.should_receive(:register).with(:finish, SpecGuard)
    SpecGuard.register
  end
end

describe SpecGuard, ".unregister" do
  before :each do
    @tally = mock("tally")
    @tally.stub!(:register)
    TallyAction.stub!(:new).and_return(@tally)
    SpecGuard.instance_variable_set(:@registered, nil)
  end

  it "unregisters its tally action" do
    @tally.should_receive(:unregister)
    SpecGuard.register
    SpecGuard.unregister
  end
end

describe SpecGuard, "#yield?" do
  before :each do
    MSpec.store :mode, nil
    @guard = SpecGuard.new
  end
  
  it "returns true if MSpec.verify_mode? is true" do
    MSpec.should_receive(:verify_mode?).and_return(true)
    @guard.yield?.should == true
  end
  
  it "returns true if MSpec.verify_mode? is true regardless of invert being true" do
    MSpec.should_receive(:verify_mode?).and_return(true)
    @guard.yield?(true).should == true
  end
  
  it "returns true if MSpec.report_mode? is true" do
    MSpec.should_receive(:report_mode?).and_return(true)
    @guard.yield?.should == true
  end
  
  it "returns true if MSpec.report_mode? is true regardless of invert being true" do
    MSpec.should_receive(:report_mode?).and_return(true)
    @guard.yield?(true).should == true
  end
  
  it "returns false if neither report nor verify mode are true" do
    @guard.yield?.should == false
  end
  
  it "returns true if invert is true and neither report nor verify mode are true" do
    @guard.yield?(true).should == true
  end
end

describe SpecGuard, "#===" do
  it "returns true" do
    anything = mock("anything")
    SpecGuard.new.===(anything).should == true
  end
end

describe SpecGuard, "#implementation?" do
end

describe SpecGuard, "#platform?" do
end

describe SpecGuard, "#match?" do
  it "returns true if any initialize arguments match a ruby platform or implementation" do
  end
end

describe SpecGuard, "#unregister" do
  before :each do
    @tally = mock("tally")
    @tally.stub!(:register)
    TallyAction.stub!(:new).and_return(@tally)
    MSpec.stub!(:unregister)
    @guard = SpecGuard.new

    SpecGuard.instance_variable_set(:@registered, nil)
    SpecGuard.register
  end
  
  it "unregisters from MSpec :exclude actions" do
    MSpec.should_receive(:unregister).with(:exclude, @guard)
    @tally.should_receive(:unregister)
    @guard.unregister
  end
  
  it "unregisters from MSpec :after actions" do
    MSpec.should_receive(:unregister).with(:after, @guard)
    @tally.should_receive(:unregister)
    @guard.unregister
  end
  
  it "invokes the class's unregister method" do
    SpecGuard.should_receive(:unregister)
    @guard.unregister
  end
end
