require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../ruby/fixtures/code_loading'
require File.dirname(__FILE__) + '/../../fixtures/code_loading'

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
