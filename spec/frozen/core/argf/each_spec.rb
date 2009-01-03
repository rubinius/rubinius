require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_line'

describe "ARGF.each" do
  it_behaves_like :argf_each_line, :each
end
