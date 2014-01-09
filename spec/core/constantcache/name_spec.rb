require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#name" do
  before :each do
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the name of the constant" do
    @cache.name.should == :CC
  end
end
