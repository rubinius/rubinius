require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/pos.rb'
require 'strscan'

describe "StringScanner#pointer" do
  it_behaves_like(:strscan_pos, :pointer)
end

describe "StringScanner#pointer=" do
  it_behaves_like(:strscan_pos_set, :pointer=)
end
