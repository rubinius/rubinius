require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CallSite#name" do
  before :each do
    @call_site = CallSiteSpec::CallSiteTest.call_sites[0]
  end

  it "has the correct name for the call site" do
    @call_site.name.should == :length
  end
end


