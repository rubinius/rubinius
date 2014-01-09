require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#executable" do
  before :each do
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the CompiledCode instance containing the call site" do
    @cache.executable.should == CallSiteSpecs::C.executable(:c)
  end
end
