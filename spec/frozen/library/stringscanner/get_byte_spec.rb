require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/get_byte'
require 'strscan'

describe "StringScanner#get_byte" do
  it_behaves_like :strscan_get_byte, :get_byte
end
