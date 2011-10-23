require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../shared/extract', __FILE__)
require 'uri'

describe "URI.extract" do
  it_behaves_like :uri_extract, :extract, URI
end
