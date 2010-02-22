require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/eql', __FILE__)

describe "Hash#==" do
  it_behaves_like :hash_eql, :==
  it_behaves_like :hash_eql_additional, :==
  it_behaves_like :hash_eql_additional_more, :==

  it "compares values with == semantics" do
    new_hash("x" => 1.0).should == new_hash("x" => 1)
  end
end
