require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/eql'

describe "Hash#==" do
  it_behaves_like :hash_eql, :==
  it_behaves_like :hash_eql_additional, :==
  it_behaves_like :hash_eql_additional_more, :==

  it "compares values with == semantics" do
    new_hash("x" => 1.0).should == new_hash("x" => 1)
  end
end
