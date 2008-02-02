require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../runner/mspec'

describe MSpec do
  it "provides .register_files to record which spec files to run" do
    MSpec.register_files [:one, :two, :three]
    MSpec.retrieve(:files).should == [:one, :two, :three]
  end
  
  it "provides .register_mode for setting execution mode flags" do
    MSpec.register_mode :verify
    MSpec.retrieve(:mode).should == :verify
  end
  
  it "provides .register_tags_path to record the path to tag files" do
    MSpec.register_tags_path "path/to/tags"
    MSpec.retrieve(:tags_path).should == "path/to/tags"
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
    MSpec.store :mode, nil
  end
  
  it "returns true if register_mode(:verify) is called" do
    MSpec.verify_mode?.should == false
    MSpec.register_mode :verify
    MSpec.verify_mode?.should == true
  end
end

describe MSpec, ".report_mode?" do
  before :each do
    MSpec.store :mode, nil
  end
  
  it "returns true if register_mode(:report) is called" do
    MSpec.report_mode?.should == false
    MSpec.register_mode :report
    MSpec.report_mode?.should == true
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
    MSpec.register_files [:one, :two, :three]
    Kernel.stub!(:load)
  end
  
  it "calls load actions before each file" do
    load = mock("load")
    load.stub!(:load).and_return { @record = :load }
    MSpec.register :load, load
    MSpec.files
    @record.should == :load
  end
  
  it "registers the current file" do
    MSpec.should_receive(:store).with(:file, :one)
    MSpec.should_receive(:store).with(:file, :two)
    MSpec.should_receive(:store).with(:file, :three)
    MSpec.files
  end
end

describe MSpec, ".tags_path" do
  before :each do
    MSpec.store :tags_path, nil
  end
  
  it "returns '.tags' if no tags path has been registered" do
    MSpec.tags_path.should == ".tags"
  end
  
  it "returns the registered tags path" do
    MSpec.register_tags_path "/path/to/tags"
    MSpec.tags_path.should == "/path/to/tags"
  end
end

describe MSpec, ".tags_file" do
  before :each do
    MSpec.store :file, "/path/to/spec/something/some_spec.rb"
    MSpec.store :tags_path, nil
  end
  
  it "returns the tags file for the current spec file" do
    MSpec.tags_file.should == "/path/to/spec/something/.tags/some_tags.txt"
  end
  
  it "returns the tags file for the current spec file with custom tags_path" do
    MSpec.register_tags_path "/path/to/tags"
    MSpec.tags_file.should == "/path/to/tags/something/some_tags.txt"
  end
end

describe MSpec, ".read_tags" do
  before :each do
    MSpec.stub!(:tags_file).and_return(File.dirname(__FILE__) + '/tags.txt')
  end
  
  it "returns a list of tag instances for matching tag names found" do
    one = SpecTag.new "fail(broken):Some#method works"
    MSpec.read_tags("fail", "pass").should == [one]
  end
  
  it "returns [] if no tags names match" do
    MSpec.read_tags("super").should == []
  end
end

describe MSpec, ".write_tag" do
  before :each do
    MSpec.stub!(:tags_file).and_return("/tmp/tags.txt")
    @tag = SpecTag.new "fail(broken):Some#method works"
  end
  
  after :all do
    File.delete "/tmp/tags.txt" rescue nil
  end
  
  it "writes a tag to the tags file for the current spec file" do
    MSpec.write_tag @tag
    IO.read("/tmp/tags.txt").should == "fail(broken):Some#method works\n"
  end
  
  it "does not write a duplicate tag" do
    File.open("/tmp/tags.txt", "w") { |f| f.puts @tag }
    MSpec.write_tag @tag
    IO.read("/tmp/tags.txt").should == "fail(broken):Some#method works\n"
  end
end
