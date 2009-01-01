require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/terminate.rb'
require 'strscan'

describe "StringScanner#terminate" do
    it_behaves_like(:strscan_terminate, :terminate)
end
