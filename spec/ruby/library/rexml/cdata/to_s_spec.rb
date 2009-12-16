require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/to_s'
require 'rexml/document'

describe "REXML::CData#to_s" do
  it_behaves_like :rexml_cdata_to_s, :to_s
end
