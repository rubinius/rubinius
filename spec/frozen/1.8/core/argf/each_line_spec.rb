require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_line'

describe "ARGF.each_line" do
  it_behaves_like :argf_each_line, :each_line
end
