require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#scope" do
  before :each do
    CallSiteSpecs::C.new.c
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the LexicalScope instance for the cache's scope" do
    @cache.lexical_scope.should be_an_instance_of(Rubinius::LexicalScope)
  end
end
