require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#name" do
  before :each do
    CallSiteSpecs::A.new.nn
    @cache = CallSiteSpecs::A.call_sites(:aa).last
  end

  it "returns the total number of times the call site has executed" do
    @cache.hits.should == 2
  end
end
