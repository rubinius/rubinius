require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#ip" do
  before :each do
    @call_site = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the IP of the call site" do
    @call_site.ip.should == 2
  end
end
