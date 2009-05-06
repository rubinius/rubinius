require File.dirname(__FILE__) + '/../../spec_helper'

describe "Hash" do
  it "includes Enumerable" do
    hash_class.include?(Enumerable).should == true
  end
end

describe "Hash#hash" do
  # prior to 1.8.7, there were no sensible Hash#hash defined at all
  ruby_version_is "1.8.7" do
    ruby_bug "#", "1.8.7.10" do
      it "returns a value which doesn't depend on the hash order" do
        new_hash(0=>2, 11=>1).hash.should == new_hash(11=>1, 0=>2).hash
      end
    end
  end

  it "generates a hash for recursive hash structures" do
    h = new_hash
    h[:a] = h
    (h.hash == h[:a].hash).should == true
  end

  ruby_version_is "" .. "1.8.6" do
    it "computes recursive hash keys with identical hashes" do
      h = new_hash
      h[h] = h
      (h.hash == h[h].hash).should == true
    end
  end
end
