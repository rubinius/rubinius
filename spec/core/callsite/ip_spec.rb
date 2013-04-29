require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CallSite#ip" do
  before :each do
    @call_site = CallSiteSpec::CallSiteTest.call_sites[0]
  end

  it "sets the correct ip for the call" do
    @call_site.ip.should == 3
  end

end
