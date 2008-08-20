require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/add'
require 'rexml/document'

describe "REXML::Attributes#add" do
 it_behaves_like :rexml_attribute_add, :add
end
