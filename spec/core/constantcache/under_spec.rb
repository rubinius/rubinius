require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#under" do
  before :each do
    ConstantCacheSpec.new.constant_cache_test
    @constant_cache_base  = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
    @constant_cache_under = ConstantCacheSpec::ConstantCacheTest.constant_caches[1]
  end

  it "has no under value for a top level constant" do
    @constant_cache_base.under.should be_nil
  end

  it "has the correct under value for a scoped constant" do
    @constant_cache_under.under.should == Rubinius
  end
end
