require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/tell'

describe "StringIO#tell" do
  it_behaves_like :stringio_tell, :tell
end
