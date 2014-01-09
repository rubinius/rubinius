require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#scope" do
  before :each do
    CallSiteSpecs::C.new.c
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the ConstantScope instance for the cache's scope" do
    @cache.scope.should be_an_instance_of(Rubinius::ConstantScope)
  end
end
