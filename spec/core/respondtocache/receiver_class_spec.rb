require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#receiver_class" do
  before :each do
    CallSiteSpecs::R.new.c

    @cache = CallSiteSpecs::R.call_sites(:c).first
  end

  it "returns the class of the receiver at the call site" do
    @cache.receiver_class.should == CallSiteSpecs::R
  end
end
