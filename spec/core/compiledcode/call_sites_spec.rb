require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CompiledCode#call_sites" do
  it "finds the correct number of call sites" do
    call_sites =  CompiledCodeSpec::CallSiteTest.call_sites
    call_sites.size.should == 1
  end

  it "finds the correct first call site" do
    call_site = CompiledCodeSpec::CallSiteTest.call_sites[0]
    call_site.name.should == :length
    call_site.executable.should == CompiledCodeSpec::CallSiteTest
  end
end

