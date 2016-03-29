require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#module" do
  before :each do
    CallSiteSpecs::C.new.cc
    cache = CallSiteSpecs::C.constant_caches(:cc)
    @cache_c = cache[0]
    @cache_cc = cache[1]
  end

  it "returns nil for the first Module in the scope chain" do
    @cache_c.module.should be_nil
  end

  it "returns the 'parent' Module in the scope chain" do
    @cache_cc.module.should == CallSiteSpecs::C
  end
end
