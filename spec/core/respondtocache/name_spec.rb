require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::RespondToCache#name" do
  before :each do
    RespondToCacheSpec::Bar.new.call_site_true
    RespondToCacheSpec::Bar.new.call_site_false

    @respond_to_cache = RespondToCacheSpec::Bar::CallSiteTrue.call_sites[0]
  end

  it "has the correct name" do
    @respond_to_cache.name.should == :respond_to?
  end
end
