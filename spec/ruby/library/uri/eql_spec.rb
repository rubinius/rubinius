require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/normalization'
require File.dirname(__FILE__) + '/shared/eql'
require 'uri'

describe "URI#eql?" do
  it_behaves_like :uri_eql, :eql?

  ruby_bug "redmine:2428", "1.8.7" do
    it_behaves_like :uri_eql_against_other_types, :eql?
  end
end