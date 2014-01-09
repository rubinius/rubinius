require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#serial" do
  before :each do
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns an Integer" do
    @cache.serial.should be_kind_of(Integer)
  end
end
