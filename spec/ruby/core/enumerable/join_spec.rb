require File.expand_path('../../../shared/array/join', __FILE__)
require File.expand_path('../../array/fixtures/classes', __FILE__)

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
