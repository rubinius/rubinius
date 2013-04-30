require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::ConstantCache#location" do
  before :each do
    @constant_cache = ConstantCacheSpec::ConstantCacheTest.constant_caches[0]
  end

  it "has the correct location for the constant cache" do
    @constant_cache.location.should =~ %r{spec/core/constantcache/fixtures/classes\.rb:4$}
  end
end
