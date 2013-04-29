require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::MonoInlineCache#hits" do
  before :each do
    MonoInlineCacheSpec::Bar.new.hits_test
    MonoInlineCacheSpec::Bar.new.hits_test
    @mono_inline_cache = MonoInlineCacheSpec::Bar::HitTest.call_sites[0]
  end

  it "has 2 hits after promotion to mono cache and additional call" do
    @mono_inline_cache.hits.should == 2
  end
end
