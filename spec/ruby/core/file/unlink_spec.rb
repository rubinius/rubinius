require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/unlink'

describe "File.unlink" do
  it_behaves_like(:file_unlink, :unlink)
end
