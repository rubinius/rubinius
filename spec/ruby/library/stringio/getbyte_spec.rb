require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + '/shared/getc'

ruby_version_is "1.8.7" do
  describe "StringIO#getbyte" do
    it_behaves_like :stringio_getc, :getbyte
  end

  describe "StringIO#getbyte when self is not readable" do
    it_behaves_like :stringio_getc_not_readable, :getbyte
  end
end
