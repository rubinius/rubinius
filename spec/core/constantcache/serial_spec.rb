require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#scope" do
  before :each do
    @constant_cache_base  = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
    @constant_cache_under = ConstantCacheSpec::ConstantCacheTest.constant_caches[1]
  end

  it "returns an integer for the serial" do
    @constant_cache_base.serial.should be_kind_of(Integer)
    @constant_cache_under.serial.should be_kind_of(Integer)
  end
end
