require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#name" do
  before :each do
    CallSiteSpecs::A.new.c
    @cache = CallSiteSpecs::A.call_sites(:c).first
  end

  it "returns the name of the method at the call site" do
    @cache.name.should == :d
  end
end
