require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::RespondToCache#ip" do
  before :each do
    RespondToCacheSpec::Bar.new.call_site_true
    RespondToCacheSpec::Bar.new.call_site_false

    @respond_to_cache = RespondToCacheSpec::Bar::CallSiteTrue.call_sites[0]
  end

  it "sets the correct ip for the call" do
    @respond_to_cache.ip.should == 4
  end

end
