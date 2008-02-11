require 'erb'
require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/url_encode'

describe "ERB::Util.u" do
  it_behaves_like :erb_util_url_encode, :u
end

