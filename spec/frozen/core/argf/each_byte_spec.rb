require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/each_byte'

describe "ARGF.each_byte" do
  it_behaves_like :argf_each_byte, :each_byte
end
