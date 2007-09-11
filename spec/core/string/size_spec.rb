require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'
require File.dirname(__FILE__) + '/length_spec'

describe "String#size" do
  it_behaves_like(@string_length, :size)
end
