require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "Rubinius::CompiledCode#call_sites" do
  before :each do
    @call_sites = CompiledCodeSpecs.executable(:call_site_test).call_sites
  end

  it "returns all the call sites in the method" do
    @call_sites.size.should == 2
  end

  it "returns the first call site" do
    @call_sites.first.name.should == :length
  end

  it "returns the second call site" do
    @call_sites.last.name.should == :size
  end
end
