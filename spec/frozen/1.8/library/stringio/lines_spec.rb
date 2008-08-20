require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + '/shared/each'

ruby_version_is "1.8.7" do
  describe "StringIO#lines" do
    it_behaves_like :stringio_each, :lines
  end
end
