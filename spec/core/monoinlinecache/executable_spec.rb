require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::MonoInlineCache#executable" do
  before :each do
    @executable = CallSiteSpecs::A.executable(:c)

    CallSiteSpecs::A.new.c
    @cache = CallSiteSpecs::A.call_sites(:c).first
  end

  it "returns the CompiledCode instance containing the call site" do
    @cache.executable.should == @executable
  end
end
