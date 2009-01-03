require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/unlink'

describe "File.delete" do
  it_behaves_like(:file_unlink, :delete)
end
