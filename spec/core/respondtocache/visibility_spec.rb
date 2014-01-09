require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#visibility" do
  it "returns false if #respond_to? does not search for private methods" do
    CallSiteSpecs::RR.new.a
    CallSiteSpecs::RR.call_sites(:a).first.visibility.should be_false
  end

  it "returns true if #respond_to? searches for private methods" do
    CallSiteSpecs::RR.new.b
    CallSiteSpecs::RR.call_sites(:b).first.visibility.should be_true
  end
end
