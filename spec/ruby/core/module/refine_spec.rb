require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "2.0.0" do
  describe "Module#refine" do
    it "runs its block in an anonymous Module" do
      selves = nil
      mod = Module.new do
	outer_self = self
	refine String do
	  selves = [outer_self, self]
	end
      end

      selves[0].should == mod
      selves[0].should_not == selves[1]
      selves[1].should be_kind_of(Module)
    end

    it "uses the same anonymous module for future refines of the same class" do
      selves = []
      mod = Module.new do
        selves << self
        refine String do
          selves << self
        end
      end

      mod.module_eval do
        refine String do
          selves << self
        end
      end

      selves[1].should == selves[2]
    end

    it "adds methods defined in its block to the anon module's public instance methods" do
      inner_self = nil
      mod = Module.new do
        refine String do
          def blah
            "blah"
          end
          inner_self = self
        end
      end

      inner_self.public_instance_methods.should include(:blah)
    end

    it "raises ArgumentError if not passed an argument" do
      lambda do
        Module.new do
          refine {}
        end
      end.should raise_error(ArgumentError)
    end

    it "raises TypeError if not passed a class or module" do
      lambda do
        Module.new do
          refine('foo') {}
        end
      end.should raise_error(TypeError)
    end

    it "raises ArgumentError if not given a block" do
      lambda do
        Module.new do
          refine String
        end
      end.should raise_error(ArgumentError)
    end

    it "applies refinements to calls in the refine block" do
      result = nil
      Module.new do
        refine(String) do
          def foo; 'foo'; end
          result = 'hello'.foo
        end
      end
      result.should == 'foo'
    end

    it "doesn't apply refinements outside the refine block" do
      Module.new do
        refine(String) {def foo; 'foo'; end}
        lambda do
          'hello'.foo
        end
        should raise_error(NoMethodError)
      end
    end

    it "does not apply refinements to external scopes not using the module" do
      Module.new do
        refine(String) {def foo; 'foo'; end}
      end

      lambda {'hello'.foo}.should raise_error(NoMethodError)
    end
  end
end
