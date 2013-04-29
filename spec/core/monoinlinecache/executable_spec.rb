require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::MonoInlineCache#executable" do
  before :each do
    MonoInlineCacheSpec::Bar.new.call_site_test
    @mono_inline_cache = MonoInlineCacheSpec::Bar::CallSiteTest.call_sites[0]
  end

  it "has the correct executable" do
    @mono_inline_cache.executable.should == MonoInlineCacheSpec::Bar::CallSiteTest
  end
end
