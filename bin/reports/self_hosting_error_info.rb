require 'timeout'

BYTECODE = File.expand_path(File.dirname(__FILE__) + '/../../lib/bytecode')
CORE = File.expand_path(File.dirname(__FILE__) + '/../../kernel/core')
BOOTSTRAP = File.expand_path(File.dirname(__FILE__) + '/../../kernel/bootstrap')
files  = Dir[BYTECODE + '/*.rb']
files += Dir[BOOTSTRAP + '/*.rb']
files += Dir[CORE + '/*.rb']

results = files.map do |path|
  begin
    command = "shotgun/rubinius #{path}"
    output = Timeout::timeout(15) do
      `(#{command}) 2>&1`
    end
    [path, output]
  rescue Timeout::Error
    [path, 'Timed out (infinite loop?)']
  end
end

puts Marshal.dump(results)
