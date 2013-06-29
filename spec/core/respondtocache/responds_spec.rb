require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::RespondToCache#responds" do
  before :each do
    RespondToCacheSpec::Bar.new.call_site_true
    RespondToCacheSpec::Bar.new.call_site_false
    RespondToCacheSpec::Bar.new.call_site_respond_to_cache
    RespondToCacheSpec::BarMissing.new.call_site_true

    @respond_to_cache_true = RespondToCacheSpec::Bar::CallSiteTrue.call_sites[0]
    @respond_to_cache_false = RespondToCacheSpec::Bar::CallSiteFalse.call_sites[0]
    @respond_to_cache_test = RespondToCacheSpec::Bar::CacheTest.call_sites[0]
    @respond_to_cache_missing = RespondToCacheSpec::BarMissing::CallSiteTrue.call_sites[0]
  end

  it "stores when responds is true" do
    @respond_to_cache_true.responds.should be_true
  end

  it "stores when responds is false" do
    @respond_to_cache_false.responds.should be_false
  end

  it "invalidates the cache when changing the class" do
    @respond_to_cache_test.responds.should be_false
    class RespondToCacheSpec::Bar
      def call_site_respond_to_cache_method
      end
    end
    RespondToCacheSpec::Bar.new.call_site_respond_to_cache
    @respond_to_cache_test = RespondToCacheSpec::Bar::CacheTest.call_sites[0]
    @respond_to_cache_test.responds.should be_true
  end

  ruby_version_is '1.9' do
    it "caches method missing handling" do
      @respond_to_cache_missing.responds.should be_true
    end
  end

end
