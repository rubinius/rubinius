require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/dup_clone'

describe "Object#dup" do
  it_behaves_like :object_dup_clone, :dup
end
