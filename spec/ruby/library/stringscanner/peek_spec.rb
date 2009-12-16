require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/peek.rb'
require 'strscan'

describe "StringScanner#peek" do
  it_behaves_like(:strscan_peek, :peek)
end

