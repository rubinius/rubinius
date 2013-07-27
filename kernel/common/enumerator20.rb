# -*- encoding: us-ascii -*-

module Enumerable
  class Enumerator

    class Lazy < self
      def initialize(receiver, size=nil)
        raise ArgumentError, "Lazy#initialize requires a block" unless block_given?
        Rubinius.check_frozen

        super(size) do |yielder|
          receiver.each do |*args|
            yield yielder, *args
          end
        end

        self
      end
      private :initialize
    end

  end
end
