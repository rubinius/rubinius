require File.dirname(__FILE__) + '/../../../spec_helper'
require 'cgi'
require File.dirname(__FILE__) + "/fixtures/common"
require File.dirname(__FILE__) + "/shared/popup_menu"

describe "CGI::HtmlExtension#popup_menu" do
  it_behaves_like :cgi_htmlextension_popup_menu, :popup_menu
end
