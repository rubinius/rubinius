require File.dirname(__FILE__) + '/../../shared/array/join'
require File.dirname(__FILE__) + '/../array/fixtures/classes'

ruby_version_is "1.9" do
  describe "Enumerable#join" do
    class Enum
      include Enumerable
      def initialize(*args)
        @elements = args
      end
      def each
        @elements.each {|e| yield e}
      end
      def <<(e)
        @elements << e
        self
      end
    end

    it_behaves_like(:array_join, :join, Enum)
  end
end
