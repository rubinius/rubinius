require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/shared/has_key"

describe "CGI::QueryExtension#has_key?" do
  it_behaves_like :cgi_query_extension_has_key_p, :has_key?
end
