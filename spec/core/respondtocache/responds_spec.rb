require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::RespondToCache#responds" do
  before :each do
    RespondToCacheSpec::Bar.new.call_site_true
    RespondToCacheSpec::Bar.new.call_site_false

    @respond_to_cache_true = RespondToCacheSpec::Bar::CallSiteTrue.call_sites[0]
    @respond_to_cache_false = RespondToCacheSpec::Bar::CallSiteFalse.call_sites[0]
  end

  it "stores when responds is true" do
    @respond_to_cache_true.responds.should be_true
  end

  it "stores when responds is false" do
    @respond_to_cache_false.responds.should be_false
  end
end
