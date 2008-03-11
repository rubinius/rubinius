require File.dirname(__FILE__) + '/shared/collect'

describe "Matrix#collect" do
    it_behaves_like(:collect, :collect)
end