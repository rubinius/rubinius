require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/tmp'
require 'mspec/matchers/base'
require 'mspec/runner/mspec'

describe MSpec, ".register_files" do
  it "records which spec files to run" do
    MSpec.register_files [:one, :two, :three]
    MSpec.retrieve(:files).should == [:one, :two, :three]
  end
end

describe MSpec, ".register_mode" do
  it "sets execution mode flags" do
    MSpec.register_mode :verify
    MSpec.retrieve(:mode).should == :verify
  end
end

describe MSpec, ".register_tags_patterns" do
  it "records the patterns for generating a tag file from a spec file" do
    MSpec.register_tags_patterns [[/spec\/ruby/, "spec/tags"], [/frozen/, "ruby"]]
    MSpec.retrieve(:tags_patterns).should == [[/spec\/ruby/, "spec/tags"], [/frozen/, "ruby"]]
  end
end

describe MSpec, ".register_exit" do
  before :each do
    MSpec.store :exit, 0
  end

  it "records the exit code" do
    MSpec.exit_code.should == 0
    MSpec.register_exit 1
    MSpec.exit_code.should == 1
  end
end

describe MSpec, ".exit_code" do
  it "retrieves the code set with .register_exit" do
    MSpec.register_exit 99
    MSpec.exit_code.should == 99
  end
end

describe MSpec, ".store" do
  it "records data for MSpec settings" do
    MSpec.store :anything, :value
    MSpec.retrieve(:anything).should == :value
  end
end

describe MSpec, ".retrieve" do
  it "accesses .store'd data" do
    MSpec.register :action, :first
    MSpec.retrieve(:action).should == [:first]
  end
end

describe MSpec, ".randomize" do
  it "sets the flag to randomize spec execution order" do
    MSpec.randomize?.should == false
    MSpec.randomize
    MSpec.randomize?.should == true
    MSpec.randomize false
    MSpec.randomize?.should == false
  end
end

describe MSpec, ".register" do
  it "is the gateway behind the register(symbol, action) facility" do
    MSpec.register :bonus, :first
    MSpec.register :bonus, :second
    MSpec.register :bonus, :second
    MSpec.retrieve(:bonus).should == [:first, :second]
  end
end

describe MSpec, ".unregister" do
  it "is the gateway behind the unregister(symbol, actions) facility" do
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
    ScratchPad.record @exception
  end

  it "rescues any exceptions raised when executing the block argument" do
    MSpec.stack.push @rs
    lambda {
      MSpec.protect("") { raise Exception, "Now you see me..." }
    }.should_not raise_error
  end

  it "records the exception in the current.state object's exceptions" do
    MSpec.stack.push @rs
    MSpec.protect("testing") { raise ScratchPad.recorded }
    @ss.exceptions.should == [["testing", ScratchPad.recorded]]
  end

  it "writes a message to STDERR if current is nil" do
    STDERR.stub!(:write)
    STDERR.should_receive(:write).with("\nAn exception occurred in testing:\nException: \"Sharp!\"\n")
    MSpec.stack.clear
    MSpec.protect("testing") { raise ScratchPad.recorded }
  end

  it "writes a message to STDERR if current.state is nil" do
    STDERR.stub!(:write)
    STDERR.should_receive(:write).with("\nAn exception occurred in testing:\nException: \"Sharp!\"\n")
    @rs.stub!(:state).and_return(nil)
    MSpec.stack.push @rs
    MSpec.protect("testing") { raise ScratchPad.recorded }
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
    ScratchPad.record []
    start_one = mock("one")
    start_one.stub!(:start).and_return { ScratchPad << :one }
    start_two = mock("two")
    start_two.stub!(:start).and_return { ScratchPad << :two }
    MSpec.register :start, start_one
    MSpec.register :start, start_two
  end

  it "does not attempt to run any actions if none have been registered" do
    MSpec.store :finish, nil
    lambda { MSpec.actions :finish }.should_not raise_error
  end

  it "runs each action registered as a start action" do
    MSpec.actions :start
    ScratchPad.recorded.should == [:one, :two]
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
  before :each do
    MSpec.stack.clear
  end

  it "accepts one argument" do
    MSpec.describe(Object) { ScratchPad.record MSpec.current }
    ScratchPad.recorded.should be_kind_of(RunState)
  end

  it "pushes a new RunState instance on the stack" do
    MSpec.describe(Object, "msg") { ScratchPad.record MSpec.current }
    ScratchPad.recorded.should be_kind_of(RunState)
  end

  it "pops the RunState instance off the stack when finished" do
    MSpec.describe(Object, "msg") { ScratchPad.record MSpec.current }
    ScratchPad.recorded.should be_kind_of(RunState)
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
    start.stub!(:start).and_return { ScratchPad.record :start }
    MSpec.register :start, start
    MSpec.process
    ScratchPad.recorded.should == :start
  end

  it "calls all finish actions" do
    finish = mock("finish")
    finish.stub!(:finish).and_return { ScratchPad.record :finish }
    MSpec.register :finish, finish
    MSpec.process
    ScratchPad.recorded.should == :finish
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
    load.stub!(:load).and_return { ScratchPad.record :load }
    MSpec.register :load, load
    MSpec.files
    ScratchPad.recorded.should == :load
  end

  it "shuffles the file list if .randomize? is true" do
    MSpec.randomize
    MSpec.should_receive(:shuffle)
    MSpec.files
    MSpec.randomize false
  end

  it "registers the current file" do
    MSpec.should_receive(:store).with(:file, :one)
    MSpec.should_receive(:store).with(:file, :two)
    MSpec.should_receive(:store).with(:file, :three)
    MSpec.files
  end
end

describe MSpec, ".shuffle" do
  before :each do
    @base = (0..100).to_a
    @list = @base.clone
    MSpec.shuffle @list
  end

  it "does not alter the elements in the list" do
    @base.each do |elt|
      @list.should include(elt)
    end
  end

  it "changes the order of the list" do
    # obviously, this spec has a certain probability
    # of failing. If it fails, run it again.
    @list.should_not == @base
  end
end

describe MSpec, ".tags_file" do
  before :each do
    MSpec.store :file, "path/to/spec/something/some_spec.rb"
    MSpec.store :tags_patterns, nil
  end

  it "returns the default tags file for the current spec file" do
    MSpec.tags_file.should == "path/to/spec/tags/something/some_tags.txt"
  end

  it "returns the tags file for the current spec file with custom tags_patterns" do
    MSpec.register_tags_patterns [[/^(.*)\/spec/, '\1/tags'], [/_spec.rb/, "_tags.txt"]]
    MSpec.tags_file.should == "path/to/tags/something/some_tags.txt"
  end

  it "performs multiple substitutions" do
    MSpec.register_tags_patterns [
      [%r(/spec/something/), "/spec/other/"],
      [%r(/spec/), "/spec/tags/"],
      [/_spec.rb/, "_tags.txt"]
    ]
    MSpec.tags_file.should == "path/to/spec/tags/other/some_tags.txt"
  end

  it "handles cases where no substitution is performed" do
    MSpec.register_tags_patterns [[/nothing/, "something"]]
    MSpec.tags_file.should == "path/to/spec/something/some_spec.rb"
  end
end

describe MSpec, ".read_tags" do
  before :each do
    MSpec.stub!(:tags_file).and_return(File.dirname(__FILE__) + '/tags.txt')
  end

  it "returns a list of tag instances for matching tag names found" do
    one = SpecTag.new "fail(broken):Some#method? works"
    MSpec.read_tags("fail", "pass").should == [one]
  end

  it "returns [] if no tags names match" do
    MSpec.read_tags("super").should == []
  end
end

describe MSpec, ".write_tag" do
  before :each do
    FileUtils.stub!(:mkdir_p)
    MSpec.stub!(:tags_file).and_return(tmp("tags.txt"))
    @tag = SpecTag.new "fail(broken):Some#method works"
  end

  after :all do
    File.delete tmp("tags.txt") rescue nil
  end

  it "writes a tag to the tags file for the current spec file" do
    MSpec.write_tag @tag
    IO.read(tmp("tags.txt")).should == "fail(broken):Some#method works\n"
  end

  it "does not write a duplicate tag" do
    File.open(tmp("tags.txt"), "w") { |f| f.puts @tag }
    MSpec.write_tag @tag
    IO.read(tmp("tags.txt")).should == "fail(broken):Some#method works\n"
  end
end

describe MSpec, ".delete_tag" do
  before :each do
    FileUtils.cp File.dirname(__FILE__) + "/tags.txt", tmp("tags.txt")
    MSpec.stub!(:tags_file).and_return(tmp("tags.txt"))
    @tag = SpecTag.new "fail(Comments don't matter):Some#method? works"
  end

  after :each do
    File.delete tmp("tags.txt") rescue nil
  end

  it "deletes the tag if it exists" do
    MSpec.delete_tag(@tag).should == true
    IO.read(tmp("tags.txt")).should == %[incomplete(20%):The#best method ever
benchmark(0.01825):The#fastest method today
]
  end

  it "does not change the tags file contents if the tag doesn't exist" do
    @tag.tag = "failed"
    MSpec.delete_tag(@tag).should == false
    IO.read(tmp("tags.txt")).should == %[fail(broken):Some#method? works
incomplete(20%):The#best method ever
benchmark(0.01825):The#fastest method today
]
  end

  it "deletes the tag file if it is empty" do
    MSpec.delete_tag(@tag).should == true
    MSpec.delete_tag(SpecTag.new("incomplete:The#best method ever")).should == true
    MSpec.delete_tag(SpecTag.new("benchmark:The#fastest method today")).should == true
    File.exist?(tmp("tags.txt")).should == false
  end
end
