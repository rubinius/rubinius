require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/../../shared/continuation/call'

with_feature :continuation_library do
  require 'continuation'

  describe "Continuation#call" do
    it_behaves_like :continuation_call, :call
  end
end
