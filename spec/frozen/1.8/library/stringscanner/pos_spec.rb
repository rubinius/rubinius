require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/pos.rb'
require 'strscan'

describe "StringScanner#pos" do
  it_behaves_like(:strscan_pos, :pos)
end

describe "StringScanner#pos=" do
  it_behaves_like(:strscan_pos_set, :pos=)
end