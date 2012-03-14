module ProcessSpecs
  class Daemonizer
    attr_reader :input, :data, :signal

    def initialize
      @script = fixture __FILE__, "daemon.rb"
      @input = tmp("process_daemon_input_file")
      @data = tmp("process_daemon_data_file")
      @signal = tmp("process_daemon_signal_file")
      @args = []
    end

    def wait_for_daemon
      while true
        return if File.exists? @signal
        sleep 0.1
      end
    end

    def invoke(behavior, arguments=[])
      case arguments.size
      when 0, 1
        args = arguments.inspect
      when 2
        args = "[#{arguments.first.inspect},#{arguments.last.inspect}]"
      end
      args << " #{@input} #{@data} #{@signal} #{behavior}"

      ruby_exe @script, :args => args

      wait_for_daemon

      return unless File.exists? @data

      File.open(@data, "rb") { |f| return f.gets.chomp }
    end
  end
end
