require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::PolyInlineCache#location" do
  before :each do
    CallSiteSpecs::A.new.p
    @cache = CallSiteSpecs::A.call_sites(:a).first
  end

  it "returns the file and line number of the call site" do
    @cache.location.should =~ %r{spec/core/fixtures/call_site\.rb:26$}
  end
end
