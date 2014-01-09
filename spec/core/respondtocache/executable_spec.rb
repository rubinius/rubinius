require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#executable" do
  before :each do
    CallSiteSpecs::R.new.c
    @cache = CallSiteSpecs::R.call_sites(:c).first
  end

  it "returns the CompiledCode instance containing the call site" do
    @cache.executable.should == CallSiteSpecs::R.executable(:c)
  end
end
