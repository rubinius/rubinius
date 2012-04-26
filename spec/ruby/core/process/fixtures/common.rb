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
      5.times do
        return true if File.exists? @signal and
                       File.exists? @data and
                       File.size? @data
        sleep 0.1
      end

      return false
    end

    def invoke(behavior, arguments=[])
      args = Marshal.dump(arguments).unpack("H*")
      args << @input << @data << @signal << behavior

      ruby_exe @script, :args => args

      wait_for_daemon

      return unless File.exists? @data

      File.open(@data, "rb") { |f| return f.read.chomp }
    end
  end
end
