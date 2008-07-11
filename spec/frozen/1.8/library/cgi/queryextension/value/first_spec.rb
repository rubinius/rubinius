require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/shared/first"

describe "CGI::QueryExtension::Value#first" do
  it_behaves_like :cgi_query_extension_value_first, :first
end
