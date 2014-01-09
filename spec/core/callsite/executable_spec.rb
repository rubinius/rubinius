require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#executable" do
  before :each do
    @call_site = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the CompiledCode instance containing the call site" do
    @call_site.executable.should == CallSiteSpecs::A.executable(:a)
  end
end
