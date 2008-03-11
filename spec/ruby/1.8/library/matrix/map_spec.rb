require File.dirname(__FILE__) + '/shared/collect'

describe "Matrix#map" do
    it_behaves_like(:collect, :map)
end