require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/comparison', __FILE__)
require File.expand_path('../shared/less_than', __FILE__)

describe "Hash#<=" do
  it_behaves_like :hash_comparison, :<=
  it_behaves_like :hash_less_than, :<=

  it "returns true if both hashes are identical" do
    h = new_hash(a: 1, b: 2)
    (h <= h).should be_true
  end
end
