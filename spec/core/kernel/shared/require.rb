describe :rbx_kernel_require, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "adds the .rb name to $LOADED_FEATURES when loading a .rbc file" do
    @object.require(@rbc).should be_true
    $LOADED_FEATURES.should == [@rb]
  end
end

describe :rbx_kernel_require_recursive, :shared => true do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture("recursive_fixture.rb") do |rb, rbc, f|
      f.puts "ScratchPad << :loaded"
      f.puts "require #{rbc.inspect}"
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

# TODO: require from rba with absolute paths
