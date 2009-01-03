require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/bol.rb'
require 'strscan'

describe "StringScanner#bol?" do
  it_behaves_like(:strscan_bol, :bol?)
end
