require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/comparison', __FILE__)
require File.expand_path('../shared/greater_than', __FILE__)

describe "Hash#>" do
  it_behaves_like :hash_comparison, :>
  it_behaves_like :hash_greater_than, :>

  it "returns false if both hashes are identical" do
    h = new_hash(a: 1, b: 2)
    (h > h).should be_false
  end
end
