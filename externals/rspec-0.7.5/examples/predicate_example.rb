require File.dirname(__FILE__) + '/spec_helper'

class BddFramework
  def intuitive?
    true
  end
  
  def adopted_quickly?
    true
  end
end

context "BDD framework" do

  setup do
    @bdd_framework = BddFramework.new
  end

  specify "should be adopted quickly" do
    @bdd_framework.should_be_adopted_quickly
  end
  
  specify "should be intuitive" do
    @bdd_framework.should_be_intuitive
  end

end