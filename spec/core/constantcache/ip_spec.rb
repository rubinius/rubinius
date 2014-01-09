require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#ip" do
  before :each do
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the IP of the call site" do
    @cache.ip.should == 0
  end
end
