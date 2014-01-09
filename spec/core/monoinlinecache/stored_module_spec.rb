require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#stored_module" do
  before :each do
    CallSiteSpecs::A.new.d
    @cache = CallSiteSpecs::A.call_sites(:d).first
  end

  it "returns the Module containing the cached method" do
    @cache.stored_module.should == CallSiteSpecs::N
  end
end
