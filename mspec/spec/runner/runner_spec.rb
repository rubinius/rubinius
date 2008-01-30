require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/state'
require File.dirname(__FILE__) + '/../../runner/runner'

describe MSpec do
  it "provides .register_files to record which spec files to run" do
    MSpec.register_files [:one, :two, :three]
    MSpec.retrieve(:files).should == [:one, :two, :three]
  end
  
  it "provides .register_mode for setting execution mode flags" do
    MSpec.register_mode :verify
    MSpec.retrieve(:mode).should == :verify
  end
  
  it "provides .store to store data" do
    MSpec.store :anything, :value
    MSpec.retrieve(:anything).should == :value
  end

  it "provides .retrieve to access .store'd data" do
    MSpec.register :action, :first
    MSpec.retrieve(:action).should == [:first]
  end
  
  it "provides .register as the gateway behind the register(symbol, action) facility" do
    MSpec.register :bonus, :first
    MSpec.register :bonus, :second
    MSpec.retrieve(:bonus).should == [:first, :second]
  end

  it "provides .unregister as the gateway behind the unregister(symbol, actions) facility" do
    MSpec.register :unregister, :first
    MSpec.register :unregister, :second
    MSpec.unregister :unregister, :second
    MSpec.retrieve(:unregister).should == [:first]
  end
end

describe MSpec, ".actions" do
  before :each do
    MSpec.store :start, []
    @record = []
    start_one = mock("one")
    start_one.stub!(:start).and_return { @record << :one }
    start_two = mock("two")
    start_two.stub!(:start).and_return { @record << :two }
    MSpec.register :start, start_one
    MSpec.register :start, start_two
  end
  
  it "does not attempt to run any actions if none have been registered" do
    MSpec.store :finish, nil
    lambda { MSpec.actions :finish }.should_not raise_error
  end
  
  it "runs each action registered as a start action" do
    MSpec.actions :start
    @record.should == [:one, :two]
  end
end

describe MSpec, ".verify_mode?" do
  before :each do
    MSpec.instance_variable_set :@mode, nil
  end
  
  it "returns true if register_mode(:verify) is called" do
    MSpec.verify_mode?.should == false
    MSpec.register_mode :verify
    MSpec.verify_mode?.should == true
  end
end

describe MSpec, ".describe" do
  it "pushes a new RunState instance on the stack" do
    MSpec.stack.clear
    MSpec.describe(Object, "msg") { @record = MSpec.current }
    @record.should be_kind_of(RunState)
  end
  
  it "pops the RunState instance off the stack when finished" do
    MSpec.stack.clear
    MSpec.describe(Object, "msg") { @record = MSpec.current }
    @record.should be_kind_of(RunState)
    MSpec.stack.should == []
  end
end

describe MSpec, ".process" do
  before :each do
    MSpec.stub!(:files)
    MSpec.store :start, []
    MSpec.store :finish, []
  end
  
  it "calls all start actions" do
    start = mock("start")
    start.stub!(:start).and_return { @record = :start }
    MSpec.register :start, start
    MSpec.process
    @record.should == :start
  end
  
  it "calls all finish actions" do
    finish = mock("finish")
    finish.stub!(:finish).and_return { @record = :finish }
    MSpec.register :finish, finish
    MSpec.process
    @record.should == :finish
  end
  
  it "calls the files method" do
    MSpec.should_receive(:files)
    MSpec.process
  end
end

describe MSpec, ".files" do
  before :each do
    MSpec.store :load, []
    MSpec.store :unload, []
    @files = [:one, :two, :three]
    Kernel.stub!(:load)
  end
  
  it "calls load actions before each file" do
    load = mock("load")
    load.stub!(:load).and_return { @record = :load }
    MSpec.register :load, load
    MSpec.files
    @record.should == :load
  end
end
