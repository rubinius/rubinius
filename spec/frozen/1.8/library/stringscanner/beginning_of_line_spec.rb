require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/bol.rb'
require 'strscan'

describe "StringScanner#beginning_of_line?" do
  it_behaves_like(:strscan_bol, :beginning_of_line?)
end
