module Rubinius
  module Metrics
    @enabled = false

    def self.enabled?
      !!@enabled
    end

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

      def to_hash
        Map.map { |k, i| MetricsHash[k] = Values[i] }
        MetricsHash
      end
    end
  end
end
