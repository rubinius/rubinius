require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../../../ruby/fixtures/code_loading', __FILE__)
require File.expand_path('../../../fixtures/code_loading', __FILE__)

describe "Rubinius::CodeLoader#require_compiled" do
  before :each do
    CodeLoadingSpecs.spec_setup
    @rb, @rbc = CodeLoadingSpecs.rbc_fixture "load_fixture.rb"
    @loader = Rubinius::CodeLoader.new @rbc
  end

  after :each do
    CodeLoadingSpecs.spec_cleanup
    rm_r @rb, @rbc
  end

  it "loads a .rbc file directly" do
    @loader.require_compiled(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "loads a .rbc file even if the related .rb file is missing" do
    rm_r @rb
    @loader.require_compiled(@rbc).should be_true
    ScratchPad.recorded.should == [:loaded]
  end

  it "adds the .rb name to $LOADED_FEATURES when loading a .rbc file" do
    @loader.require_compiled(@rbc).should be_true
    $LOADED_FEATURES.should == [@rb]
  end
end
