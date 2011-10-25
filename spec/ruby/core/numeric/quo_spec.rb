require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/quo', __FILE__)

describe "Numeric#quo" do
  ruby_version_is ""..."1.9" do
    it_behaves_like :numeric_quo_18, :quo
  end

  ruby_version_is "1.9" do
    it "returns the result of calling self#/ with other" do
      obj = NumericSpecs::Subclass.new
      obj.should_receive(:coerce).twice.and_return([19,19])
      obj.should_receive(:<=>).any_number_of_times.and_return(1)
      obj.should_receive(:/).and_return(20)

      obj.quo(19).should == 20
    end
  end
end
