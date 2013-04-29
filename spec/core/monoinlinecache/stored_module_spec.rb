require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::MonoInlineCache#stored_module" do
  before :each do
    MonoInlineCacheSpec::Bar.new.call_site_test
    @mono_inline_cache = MonoInlineCacheSpec::Bar::CallSiteTest.call_sites[0]
  end

  it "has the correct stored module" do
    @mono_inline_cache.stored_module.should == MonoInlineCacheSpec::Foo
  end
end
