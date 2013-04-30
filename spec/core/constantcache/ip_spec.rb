require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#ip" do
  before :each do
    @constant_cache = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
  end

  it "has the correct ip for the constant cache" do
    @constant_cache.ip.should == 0
  end
end
