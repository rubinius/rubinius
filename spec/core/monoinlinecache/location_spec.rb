require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#location" do
  before :each do
    CallSiteSpecs::A.new.c
    @cache = CallSiteSpecs::A.call_sites(:c).first
  end

  it "has the correct location" do
    @cache.location.should =~ %r{spec/core/fixtures/call_site\.rb:34$}
  end
end
