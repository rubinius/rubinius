require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#hits" do
  before :each do
    obj = CallSiteSpecs::A.new
    obj.n
    obj.n

    @cache = CallSiteSpecs::A.call_sites(:n).first
  end

  it "returns the number of times the call site has executed" do
    @cache.hits.should == 2
  end
end
