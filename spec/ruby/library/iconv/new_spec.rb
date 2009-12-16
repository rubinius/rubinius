require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/new'
require 'iconv'

describe "Iconv.new" do
  it_behaves_like :iconv_new, :new
end
