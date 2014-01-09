require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::CallSite#location" do
  before :each do
    @call_site = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the file and line number of the call site" do
    @call_site.location.should =~ %r{spec/core/fixtures/call_site\.rb:26$}
  end
end
