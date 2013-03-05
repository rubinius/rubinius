require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.8.7" do
  describe "Symbol#to_proc" do
    it "returns a new Proc" do
      proc = :to_s.to_proc
      proc.should be_kind_of(Proc)
    end

    it "sends self to arguments passed when calling #call on the Proc" do
      obj = mock("Receiving #to_s")
      obj.should_receive(:to_s).and_return("Received #to_s")
      :to_s.to_proc.call(obj).should == "Received #to_s"
    end

    ruby_version_is "1.9" do
      it "yields to the passed block when calling #call on the Proc" do
        klass = Class.new do
          def m
            yield
          end

          def to_proc
            :m.to_proc.call(self) { :value }
          end
        end

        klass.new.to_proc.should == :value
      end
    end
  end
end
