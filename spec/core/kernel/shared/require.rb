describe :rbx_kernel_require, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "loads a .rbc file directly" do
    @object.require(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "adds the .rb name to $LOADED_FEATURES when loading a .rbc file" do
    @object.require(@rbc).should be_true
    $LOADED_FEATURES.should == [@rb]
  end

  it "saves a .rbc file when loading a .rb file" do
    rm_r @rbc
    @object.require(@rb).should be_true
    File.exists?(@rbc).should be_true
  end

  it "loads a .rbc file if it is newer than the related .rb file" do
    touch(@rb) { |f| f.puts "ScratchPad << :not_loaded" }

    now = Time.now
    File.utime now, now, @rbc

    @object.require(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "loads a .rbc file even if the related .rb file is missing" do
    rm_r @rb
    @object.require(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end
end

describe :rbx_kernel_require_recursive, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture("recursive_fixture.rb") do |rb, rbc, f|
      f.puts "ScratchPad << :loaded"
      f.puts "#{@method} #{rbc.inspect}"
    end
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "does not recursively load a .rbc file that requires itself" do
    @object.require(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end
end

describe :rbx_kernel_require_rba_relative, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup

    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
    @rba = CodeLoadingSpecs.rba_fixture File.basename(@rbc), @rbc
    $LOAD_PATH << @rba

    rm_r @rb, @rbc
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rba
  end

  it "loads a .rb file from a .rba file that appears in $LOAD_PATH" do
    @object.require("load_fixture.rb").should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "loads a .rbc file from a .rba file that appears in $LOAD_PATH" do
    @object.require("load_fixture.rbc").should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "adds a .rb file loaded from a .rba file to $LOADED_FEATURES" do
    @object.require("load_fixture.rb").should be_true
    $LOADED_FEATURES.should == ["load_fixture.rb"]
  end

  it "only loads a .rb file from a .rba file once" do
    @object.require("load_fixture.rb").should be_true
    ScratchPad.recorded.should == [:loaded]

    ScratchPad.clear
    @object.require("load_fixture.rb").should be_false
    ScratchPad.recorded.should be_nil
  end
end
