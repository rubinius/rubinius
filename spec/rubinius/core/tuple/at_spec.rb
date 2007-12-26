require File.dirname(__FILE__) + '/../../../spec_helper'
require File.dirname(__FILE__) + '/shared/at'

extension :rubinius do
  describe "Tuple#at" do
    it_behaves_like :tuple_at, :at
  end
end
