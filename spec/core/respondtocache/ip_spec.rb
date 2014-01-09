require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#ip" do
  before :each do
    CallSiteSpecs::R.new.c

    @cache = CallSiteSpecs::R.call_sites(:c).first
  end

  it "returns the IP of the call site" do
    @cache.ip.should == 4
  end
end
