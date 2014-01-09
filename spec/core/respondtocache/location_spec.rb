require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#location" do
  before :each do
    CallSiteSpecs::R.new.c

    @cache = CallSiteSpecs::R.call_sites(:c).first
  end

  it "returns the file and line number of the call site" do
    @cache.location.should =~ %r{spec/core/fixtures/call_site\.rb:86$}
  end
end
