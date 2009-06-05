require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

module KernelSpecs
  class MethodsUndefd
    def all; end
  end

  class MethodsUndefdParent
    def parent_method; end
  end

  class MethodsUndefdChild < MethodsUndefdParent
    undef_method :parent_method
  end
end

# On Ruby < 1.9 #methods returns an Array of Strings
ruby_version_is ""..."1.9" do
  describe "Kernel#methods" do

    it "picks up methods added via self.meth" do
      KernelSpecs::Methods.methods(false).should include("ichi")
    end

    it "picks up methods added inside 'class << self'" do
      KernelSpecs::Methods.methods(false).should include("san")
    end

    it "picks up private methods added via self.meth" do
      KernelSpecs::Methods.methods(false).should include("shi")
    end

    it "picks up methods added inside 'class << self' after private" do
      KernelSpecs::Methods.methods(false).should include("roku")
    end

    it "doesn't pick up methods added inside 'class << self; private'" do
      KernelSpecs::Methods.methods(false).should_not include("shichi")
    end

    it "returns a list of the names of publicly accessible methods in the object" do
      meths =  KernelSpecs::Methods.methods(false)

      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do|m|
        meths.should include(m)
      end

      KernelSpecs::Methods.new.methods(false).should == []
    end

    it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
      meths = KernelSpecs::Methods.methods(false) & KernelSpecs::Methods.methods
      ["hachi", "ichi", "juu", "juu_ichi", "juu_ni", "roku", "san", "shi"].each do |m|
        meths.should include(m)
      end

      m = KernelSpecs::Methods.new.methods
      m.should include('ku')
      m.should include('ni')
      m.should include('juu_san')
    end

    it "does not include any .undef'd methods" do
      o = KernelSpecs::MethodsUndefd.new

      def o.single; end

      o.methods.should include("all")
      o.methods.should include("single")

      KernelSpecs::MethodsUndefd.send :undef_method, :all

      o.methods.should_not include("all")

      class << o; self; end.send :undef_method, :single

      o.methods.should_not include("single")
    end

    it "does not include any .undef'd methods even if method is inherited" do
      o = KernelSpecs::MethodsUndefdChild.new
      o.methods.should_not include("inh")
    end

  end
end

# On MRI >= 1.9 #methods returns an Array of Symbols.
ruby_version_is "1.9" do
  describe "Kernel#methods" do

    it "picks up methods added via self.meth" do
      KernelSpecs::Methods.methods(false).should include(:ichi)
    end

    it "picks up methods added inside 'class << self'" do
      KernelSpecs::Methods.methods(false).should include(:san)
    end

    it "picks up private methods added via self.meth" do
      KernelSpecs::Methods.methods(false).should include(:shi)
    end

    it "picks up methods added inside 'class << self' after private" do
      KernelSpecs::Methods.methods(false).should include(:roku)
    end

    it "doesn't pick up methods added inside 'class << self; private'" do
      KernelSpecs::Methods.methods(false).should_not include(:shichi)
    end

    it "returns a list of the names of publicly accessible methods in the object" do
      meths =  KernelSpecs::Methods.methods(false)

      [:hachi, :ichi, :juu, :juu_ichi, :juu_ni, :roku, :san, :shi].each do|m|
        meths.should include(m)
      end

      KernelSpecs::Methods.new.methods(false).should == []
    end

    it "returns a list of the names of publicly accessible methods in the object and its ancestors and mixed-in modules" do
      meths = KernelSpecs::Methods.methods(false) & KernelSpecs::Methods.methods
      [:hachi, :ichi, :juu, :juu_ichi, :juu_ni, :roku, :san, :shi].each do |m|
        meths.should include(m)
      end

      m = KernelSpecs::Methods.new.methods
      m.should include(:ku)
      m.should include(:ni)
      m.should include(:juu_san)
    end

    it "does not include any .undef'd methods" do
      o = KernelSpecs::MethodsUndefd.new

      def o.single; end

      o.methods.should include(:all)
      o.methods.should include(:single)

      KernelSpecs::MethodsUndefd.send :undef_method, :all

      o.methods.should_not include(:all)

      class << o; self; end.send :undef_method, :single

      o.methods.should_not include(:single)
    end

    it "does not include any .undef'd methods even if method is inherited" do
      o = KernelSpecs::MethodsUndefdChild.new
      o.methods.should_not include(:inh)
    end

  end
end
