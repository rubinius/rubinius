require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CallSite#executable" do
  before :each do
    @call_site = CallSiteSpec::CallSiteTest.call_sites[0]
  end

  it "has the correct executable for the call site" do
    @call_site.executable.should == CallSiteSpec::CallSiteTest
  end
end



