require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/readlines'

describe "ARGF.readlines" do
  it_behaves_like :argf_readlines, :readlines
end
