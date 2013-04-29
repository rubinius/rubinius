require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::MonoInlineCache#name" do
  before :each do
    MonoInlineCacheSpec::Bar.new.call_site_test
    @mono_inline_cache = MonoInlineCacheSpec::Bar::CallSiteTest.call_sites[0]
  end

  it "has the correct name" do
    @mono_inline_cache.name.should == :foo
  end
end
