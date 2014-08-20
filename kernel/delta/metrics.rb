module Rubinius
  module Metrics
    def self.data
      @data ||= Data.new
    end

    class Data
      def keys
        Map.keys
      end

      def values
        Values.to_a
      end

      def [](key)
        Values[Map[key]]
      end
    end
  end
end
