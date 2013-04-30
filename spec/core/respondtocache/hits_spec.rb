require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::RespondToCache#hits" do
  before :each do
    RespondToCacheSpec::Bar.new.hits_test
    RespondToCacheSpec::Bar.new.hits_test

    @respond_to_cache = RespondToCacheSpec::Bar::HitTest.call_sites[0]
  end

  it "has the correct number of hits" do
    @respond_to_cache.hits.should == 2
  end
end
