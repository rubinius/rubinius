require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#method" do
  before :each do
    CallSiteSpecs::A.new.c
    @cache = CallSiteSpecs::A.call_sites(:c).first
  end

  it "returns the CompiledCode instance at the call site" do
    @cache.method.should == CallSiteSpecs::A.executable(:d)
  end
end
