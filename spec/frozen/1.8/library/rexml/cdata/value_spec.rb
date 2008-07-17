require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_s'
require 'rexml/document'

describe "REXML::CData#value" do
  it_behaves_like :rexml_cdata_to_s, :value
end
