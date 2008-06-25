require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/common'
require File.dirname(__FILE__) + '/each_document_spec'

describe "YAML::load_documents" do
  it_behaves_like(:each_document, "YAML", :each_document)
end

