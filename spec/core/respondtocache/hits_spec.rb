require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#hits" do
  before :each do
    obj = CallSiteSpecs::R.new
    obj.n
    obj.n

    @cache = CallSiteSpecs::R.call_sites(:n).first
  end

  it "returns the number of times the call site has executed" do
    @cache.hits.should == 2
  end
end
