require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/fileno'

describe "ARGF.to_i" do
  it_behaves_like :argf_fileno, :to_i
end
