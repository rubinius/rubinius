require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/put'

extension :rubinius do
  describe "Tuple#put" do
    it_behaves_like :tuple_put, :put
  end
end
