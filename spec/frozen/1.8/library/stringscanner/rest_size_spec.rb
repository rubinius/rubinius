require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/rest_size.rb'
require 'strscan'

describe "StringScanner#rest_size" do
  it_behaves_like(:strscan_rest_size, :rest_size)
end