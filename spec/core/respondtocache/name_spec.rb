require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#name" do
  before :each do
    CallSiteSpecs::R.new.c

    @cache = CallSiteSpecs::R.call_sites(:c).first
  end

  it "returns the name of the method at the call site" do
    @cache.name.should == :respond_to?
  end
end
