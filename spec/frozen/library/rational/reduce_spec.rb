require File.dirname(__FILE__) + '/../../shared/rational/reduce'

ruby_version_is ""..."1.9" do
  describe "Rational.reduce" do
    it_behaves_like(:rational_reduce, :reduce)
  end

  describe "Rational.reduce when Unify is defined" do
    it_behaves_like(:rational_reduce_unfiy, :reduce)
  end
end
