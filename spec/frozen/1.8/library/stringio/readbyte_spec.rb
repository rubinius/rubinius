require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + "/shared/readchar"

ruby_version_is "1.8.7" do
  describe "StringIO#readbyte" do
    it_behaves_like :stringio_readchar, :readchar
  end
end