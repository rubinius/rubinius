require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/fixtures/strings'
require File.dirname(__FILE__) + '/shared/each_document'

describe "YAML#each_document" do
  it_behaves_like :yaml_each_document, :each_document
end
