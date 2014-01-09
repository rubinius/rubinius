require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#under" do
  before :each do
    CallSiteSpecs::C.new.c
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the cached value of the constant" do
    @cache.value.should == 1
  end
end
