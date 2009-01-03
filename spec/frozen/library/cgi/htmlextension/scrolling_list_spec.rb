require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/shared/popup_menu"

describe "CGI::HtmlExtension#scrolling_list" do
  it_behaves_like :cgi_htmlextension_popup_menu, :scrolling_list
end
