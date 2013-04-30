require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#executable" do
  before :each do
    @constant_cache = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
  end

  it "has the correct executable for the constant cache" do
    @constant_cache.executable.should == ConstantCacheSpec::ConstantCacheTest
  end
end
