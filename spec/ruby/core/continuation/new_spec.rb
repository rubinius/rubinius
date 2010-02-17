require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/continuation/new'

with_feature :continuation do
  describe "Continuation.new" do
    it_behaves_like :continuation_new, :new
  end
end
