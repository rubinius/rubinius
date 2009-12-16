require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/fixtures/strings'
require File.dirname(__FILE__) + '/shared/each_document'

describe "YAML.load_documents" do
  it_behaves_like :yaml_each_document, :load_documents
end
