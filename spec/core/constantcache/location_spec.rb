require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::ConstantCache#location" do
  before :each do
    @cache = CallSiteSpecs::C.constant_caches(:c).first
  end

  it "returns the file and line number of the call site" do
    @cache.location.should =~ %r{spec/core/fixtures/call_site\.rb:74$}
  end
end
