require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CallSite#name" do
  before :each do
    @call_site = CallSiteSpec::CallSiteTest.call_sites[0]
  end

  it "has no hits before any call" do
    @call_site.hits.should == 0
  end

  it "has a single hit after one call" do
    CallSiteSpec.new.call_site_test
    # needs to retrieve the new call site because
    # it has been replaced
    call_site = CallSiteSpec::CallSiteTest.call_sites[0]
    call_site.hits.should == 1
  end
end




