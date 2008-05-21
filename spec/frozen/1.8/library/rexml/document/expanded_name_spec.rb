require 'rexml/document'
require File.dirname(__FILE__) + '/../../../spec_helper'

shared :document_expanded_name do |cmd|
  describe "REXML::Document##{cmd}" do
    it "returns an empty string for root" do # root nodes have no expanded name
      REXML::Document.new.expanded_name.should == ""
    end
  end
end

describe "REXML::Document#expanded_name" do
  it_behaves_like(:document_expanded_name, :expanded_name)
end

describe "REXML::Document#name" do
  it_behaves_like(:document_expanded_name, :name)
end
