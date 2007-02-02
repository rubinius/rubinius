module Spec
  module Expectations
    # rspec adds all of these expectations to every Object (and,
    # implicitly, every Class).
    module ObjectExpectations
      
      # Supports the following expectations:
      #   receiver.should == expected #any value
      #     Passes if (receiver == expected)
      #
      #   receiver.should =~ expected #a regexp
      #     Passes if (receiver =~ expected), where expected
      #     is a Regexp.
      #
      # NOTE that this does NOT support receiver.should != expected.
      # Instead, use receiver.should_not == expected
      def should
        Should::Should.new self
      end

      # Supports the following expectations:
      #   receiver.should_not == expected #any value
      #     Passes unless (receiver == expected)
      #
      #   receiver.should_not =~ expected #a regexp
      #     Passes unless (receiver =~ expected), where expected
      #     is a Regexp.
      def should_not
        should.not
      end

      # Passes if receiver.equal?(expected)
      def should_equal(expected)
        should.equal(expected)
      end

      # Passes unless receiver.equal?(expected)
      def should_not_equal(expected)
        should.not.equal(expected)
      end

      # Passes if receiver.eql?(expected)
      def should_eql(expected)
        should.eql(expected)
      end

      # Passes unless receiver.eql?(expected)
      def should_not_eql(expected)
        should.not.eql(expected)
      end

      # Specify that the receiver should have a
      # specified number of items in a named collection. For example:
      #
      #   team.should_have(40).players
      #
      # Passes if team.players.size == 40.
      #
      # Works for collections with size() and/or length() methods.
      def should_have(expected)
        should.have(expected)
      end
      alias_method :should_have_exactly, :should_have

      # Specify that the receiver should have at least a
      # specified number of items in a named collection. For example:
      #
      #   team.should_have_at_least(10).players
      #
      # Passes if team.players.size == 10 (or more)
      #
      # Fails if team.players.size == 9 (or less)
      #
      # Works for collections with size() and/or length() methods.
      def should_have_at_least(expected)
        should.have.at_least(expected)
      end

      # Specify that the receiver should have at most a
      # specified number of items in a named collection. For example:
      #
      #   team.should_have_at_most(10).players
      #
      # Passes if team.players.size == 10 (or less)
      #
      # Fails if team.players.size == 11 (or more)
      #
      # Works for collections with size() and/or length() methods.
      def should_have_at_most(expected)
        should.have.at_most(expected)
      end

      # Passes if receiver.include?(expected)
      def should_include(expected)
        should.include(expected)
      end

      # Passes unless receiver.include?(expected)
      def should_not_include(expected)
        should.not.include(expected)
      end
    
      def should_be(expected = :___no_arg)
        should.be(expected)
      end
      
      def should_not_be(expected = :___no_arg)
        should.not.be(expected)
      end
      
      # Passes if &block returns true
      def should_satisfy(&block)
        should.satisfy(&block)
      end
      
      # Passes unless &block returns true
      def should_not_satisfy(&block)
        should.not.satisfy(&block)
      end
      
      # Passes if receiver is an instance of expected_class
      def should_be_an_instance_of(expected_class)
        should.be.an_instance_of(expected_class)
      end
      alias_method :should_be_instance_of, :should_be_an_instance_of
      
      # Passes unless receiver is an instance of expected_class
      def should_not_be_an_instance_of(expected_class)
        should.not.be.an_instance_of(expected_class)
      end
      
      # Passes if receiver is an instance of either expected_class
      # or a subclass of expected_class
      def should_be_a_kind_of(expected_class)
        should.be.a_kind_of(expected_class)
      end
      alias_method :should_be_kind_of, :should_be_a_kind_of
      
      # Passes unless receiver is an instance of either expected_class
      # or a subclass of expected_class
      def should_not_be_a_kind_of(expected_class)
        should.not.be.a_kind_of(expected_class)
      end
      
      def should_respond_to(message)
        should.respond_to(message)
      end
      
      def should_not_respond_to(message)
        should.not.respond_to(message)
      end
    end
  end
end

class Object
  include Spec::Expectations::ObjectExpectations
  include Spec::Expectations::UnderscoreSugar
end

Object.handle_underscores_for_rspec!
