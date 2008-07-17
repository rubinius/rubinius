require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/shared/to_a"

describe "CGI::QueryExtension::Value#to_a" do
  it_behaves_like :cgi_query_extension_value_to_a, :to_a
end

describe "CGI::QueryExtension::Value#to_a when alternative values for self are set" do
  it_behaves_like :cgi_query_extension_value_to_a_alternate_self, :to_a
end
