require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash" do
  it "includes Enumerable" do
    Hash.include?(Enumerable).should == true
  end
end

describe "Hash#hash" do
  # prior to 1.8.7, there were no sensible Hash#hash defined at all
  ruby_version_is "1.8.7" do
    ruby_bug "#", "1.8.7.10" do
      it "returns a value which doesn't depend on the hash order" do
        {0=>2, 11=>1}.hash.should == {11=>1, 0=>2}.hash
      end
    end
  end
end
