require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#receiver_class" do
  before :each do
    CallSiteSpecs::A.new.c
    @cache = CallSiteSpecs::A.call_sites(:c).first
  end

  it "returns the class of the receiver at the call site" do
    @cache.receiver_class.should == CallSiteSpecs::A
  end
end
