require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#ip" do
  before :each do
    CallSiteSpecs::A.new.p
    @cache = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the IP of the call site" do
    @cache.ip.should == 2
  end
end

