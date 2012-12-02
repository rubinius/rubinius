module Rubinius
  class Mirror
    class String < Mirror
      subject = ::String

      def character_index(str, start)
        Rubinius.invoke_primitive :string_character_index, @object, str, start
      end
    end
  end
end
