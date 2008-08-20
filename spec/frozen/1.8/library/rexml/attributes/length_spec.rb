require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/length'
require 'rexml/document'

describe "REXML::Attributes#length" do
 it_behaves_like :rexml_attribute_length, :length
end
