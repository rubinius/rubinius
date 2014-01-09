require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#hits" do
  before :each do
    @executable = CallSiteSpecs::A.executable(:c)
  end

  it "returns 0 before the call site has executed" do
    @executable.call_sites.first.hits.should == 0
  end

  it "returns the number of times the call site has executed" do
    obj = CallSiteSpecs::A.new
    obj.c
    obj.c

    # needs to retrieve the new call site because it has been replaced
    @executable.call_sites.first.hits.should == 2
  end
end




