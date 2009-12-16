require 'erb'
require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/html_escape'

describe "ERB::Util.h" do
  it_behaves_like :erb_util_html_escape, :h
end
