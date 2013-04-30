require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#name" do
  before :each do
    @constant_cache_base  = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
    @constant_cache_under = ConstantCacheSpec::ConstantCacheTest.constant_caches[1]

  end

  it "has the correct name for the constant cache" do
    @constant_cache_base.name.should == :Rubinius
    @constant_cache_under.name.should == :VariableScope
  end
end
