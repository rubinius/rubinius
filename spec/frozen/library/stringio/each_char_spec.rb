require File.dirname(__FILE__) + '/../../spec_helper'
require 'stringio'
require File.dirname(__FILE__) + "/shared/each_char"

ruby_version_is "1.8.7" do
  describe "StringIO#each_char" do
    it_behaves_like :stringio_each_char, :each_char
  end

  describe "StringIO#each_char when self is not readable" do
    it_behaves_like :stringio_each_char_not_readable, :chars
  end
end