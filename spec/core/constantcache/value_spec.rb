require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#under" do
  before :each do
    ConstantCacheSpec.new.constant_cache_test
    @constant_cache_base  = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
    @constant_cache_under = ConstantCacheSpec::ConstantCacheTest.constant_caches[1]
  end

  it "returns the cached constant" do
    @constant_cache_base.value.should == Rubinius
    @constant_cache_under.value.should == Rubinius::VariableScope
  end
end
