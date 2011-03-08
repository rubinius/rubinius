require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Kernel#freeze" do
  # TODO: Fix! You cannot freeze in the main spec process. This interferes
  # with other specs.
  quarantine! do
    it "prevents self from being further modified" do
      o = mock('o')
      o.frozen?.should be_false
      o.freeze
      o.frozen?.should be_true
    end

    it "returns the immediate when called on an immediate" do
      nil.freeze.should be_nil
      true.freeze.should be_true
      false.freeze.should be_false
      1.freeze.should == 1
      :sym.freeze.should == :sym
    end

    ruby_version_is '' ... '1.9' do
      it "has no effect on immediate values" do
        a = nil
        b = true
        c = false
        d = 1
        e = :sym
        a.freeze
        b.freeze
        c.freeze
        d.freeze
        e.freeze
        a.frozen?.should be_false
        b.frozen?.should be_false
        c.frozen?.should be_false
        d.frozen?.should be_false
        e.frozen?.should be_false
      end
    end

    ruby_version_is '1.9' do
      # 1.9 allows immediates to be frozen #1747
      it "freezes immediate values" do
        a = nil
        b = true
        c = false
        d = 1
        e = :sym
        a.freeze
        b.freeze
        c.freeze
        d.freeze
        e.freeze
        a.frozen?.should be_true
        b.frozen?.should be_true
        c.frozen?.should be_true
        d.frozen?.should be_true
        e.frozen?.should be_true
      end
    end

    ruby_version_is "" ... "1.9" do
      it "causes mutative calls to raise TypeError" do
        o = Class.new do
          def mutate; @foo = 1; end
        end.new
        o.freeze
        lambda {o.mutate}.should raise_error(TypeError)
      end
    end

    ruby_version_is "1.9" do
      it "causes mutative calls to raise RuntimeError" do
        o = Class.new do
          def mutate; @foo = 1; end
        end.new
        o.freeze
        lambda {o.mutate}.should raise_error(RuntimeError)
      end
    end
  end
end
