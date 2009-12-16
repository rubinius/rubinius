# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/codepoints'

# See redmine #1667
describe "StringIO#codepoints" do
  ruby_version_is "1.9.2" do
    it_behaves_like(:stringio_codepoints, :codepoints)
  end
end
