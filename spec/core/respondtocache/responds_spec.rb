require File.expand_path('../../fixtures/call_site.rb', __FILE__)

describe "Rubinius::RespondToCache#responds" do
  it "returns true when #respond_to? returns true" do
    CallSiteSpecs::R.new.c
    CallSiteSpecs::R.call_sites(:c).first.responds.should be_true
  end

  it "returns false when #respond_to? returns false" do
    CallSiteSpecs::R.new.d
    CallSiteSpecs::R.call_sites(:d).first.responds.should be_false
  end

  it "resets the cache when the method's class is changed" do
    CallSiteSpecs::R.new.dd
    CallSiteSpecs::R.call_sites(:dd).first.responds.should be_false

    class CallSiteSpecs::R
      def xx
      end
    end

    CallSiteSpecs::R.new.dd
    CallSiteSpecs::R.call_sites(:dd).first.responds.should be_true
  end

  it "caches the value of #respond_to_missing?" do
    CallSiteSpecs::RR.new.a
    CallSiteSpecs::RR.call_sites(:a).first.responds.should be_true
  end
end
