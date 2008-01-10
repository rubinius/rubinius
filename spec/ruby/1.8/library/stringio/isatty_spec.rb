require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/isatty'

describe "StringIO#tty?" do
  it_behaves_like :stringio_isatty, :isatty
end
