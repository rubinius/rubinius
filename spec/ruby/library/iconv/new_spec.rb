require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/new', __FILE__)
require 'iconv'

describe "Iconv.new" do
  it_behaves_like :iconv_new, :new
end
