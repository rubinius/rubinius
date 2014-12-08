module Rubinius
  class FSEvent
    attr_accessor :fileno
    attr_accessor :path

    def self.allocate
      Rubinius.primitive :fsevent_allocate
      raise PrimitiveFailure, "Rubinius::FSEvent.allocate failed"
    end

    def watch_file(fd, path)
      Rubinius.primitive :fsevent_watch_file
      raise PrimitiveFailure, "Rubinius::FSEvent#watch_file failed"
    end

    def wait_for_event
      Rubinius.primitive :fsevent_wait_for_event
      raise PrimitiveFailure, "Rubinius::FSEvent#wait_for_event failed"
    end

    def inspect
      str = "#<#{self.class}:0x#{self.__id__.to_s(16)}"

      str << " fileno=#{fileno}" if fileno
      str << " path=#{path}" if path
      str << ">"

      Rubinius::Type.infect str, self

      str
    end
  end
end
