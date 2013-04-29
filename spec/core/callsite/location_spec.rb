require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CallSite#location" do
  before :each do
    @call_site = CallSiteSpec::CallSiteTest.call_sites[0]
  end

  it "gives the file and line number" do
    @call_site.location.should =~ %r{spec/core/callsite/fixtures/classes\.rb:4$}
  end

end
