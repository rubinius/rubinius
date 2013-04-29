require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::MonoInlineCache#location" do
  before :each do
    MonoInlineCacheSpec::Bar.new.call_site_test
    @mono_inline_cache = MonoInlineCacheSpec::Bar::CallSiteTest.call_sites[0]
  end

  it "has the correct location" do
    @mono_inline_cache.location.should =~ %r{spec/core/monoinlinecache/fixtures/classes\.rb:10$}
  end
end
