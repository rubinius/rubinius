
describe :complex_multiply_complex, :shared => true do
  it "multiplies according to the usual rule for complex numbers: (a + bi) * (c + di) = ac - bd + (ad + bc)i" do
    (Complex(1, 2) * Complex(10, 20)).should == Complex((1 * 10) - (2 * 20), (1 * 20) + (2 * 10))
    (Complex(1.5, 2.1) * Complex(100.2, -30.3)).should == Complex((1.5 * 100.2) - (2.1 * -30.3), (1.5 * -30.3) + (2.1 * 100.2))
  end
end

describe :complex_multiply_integer, :shared => true do
  it "multiplies both parts of self by the given Integer" do
    (Complex(3, 2) * 50).should == Complex(150, 100)
    (Complex(-3, 2) * 50.5).should == Complex(-151.5, 101)
  end
end

describe :complex_multiply_object, :shared => true do
  it "tries to coerce self into other" do
    value = Complex(3, 9)

    obj = mock("Object")
    obj.should_receive(:coerce).with(value).and_return([2, 5])
    (value * obj).should == 2 * 5
  end
end
