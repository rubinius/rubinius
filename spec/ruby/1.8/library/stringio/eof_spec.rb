require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/eof'

describe "StringIO#eof?" do
  it_behaves_like :stringio_eof, :eof?
end

describe "StringIO#eof" do
  it_behaves_like :stringio_eof, :eof
end
