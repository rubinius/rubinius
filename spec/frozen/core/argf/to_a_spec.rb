require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/readlines'

describe "ARGF.to_a" do
  it_behaves_like :argf_readlines, :to_a
end
