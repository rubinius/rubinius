require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#scope" do
  before :each do
    ConstantCacheSpec.new.constant_cache_test
    @constant_cache_base  = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
    @constant_cache_under = ConstantCacheSpec::ConstantCacheTest.constant_caches[1]
  end

  it "has a scope after the method is called" do
    @constant_cache_base.scope.should be_an_instance_of(Rubinius::ConstantScope)
    @constant_cache_under.scope.should be_an_instance_of(Rubinius::ConstantScope)
  end
end
