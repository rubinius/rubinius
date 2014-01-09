require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#name" do
  before :each do
    @call_site = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the name of the call site" do
    @call_site.name.should == :c
  end
end
