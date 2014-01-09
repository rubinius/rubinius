require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#executable" do
  before :each do
    CallSiteSpecs::A.new.p
    @cache = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the CompiledCode instance containing the call site" do
    @cache.executable.should == CallSiteSpecs::A.executable(:a)
  end
end
