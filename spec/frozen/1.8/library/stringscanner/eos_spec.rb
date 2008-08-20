require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/eos.rb'
require 'strscan'

describe "StringScanner#eos?" do
  it_behaves_like(:strscan_eos, :eos?)
end
