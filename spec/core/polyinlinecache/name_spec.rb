require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#name" do
  before :each do
    CallSiteSpecs::A.new.p
    @cache = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the name of method at the call site" do
    @cache.name.should == :c
  end
end
