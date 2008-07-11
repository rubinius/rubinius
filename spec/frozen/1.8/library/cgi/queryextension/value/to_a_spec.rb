require File.dirname(__FILE__) + '/../../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/shared/to_a"

describe "CGI::QueryExtension::Value#to_a" do
  it_behaves_like :cgi_query_extension_value_to_a, :to_a
end
