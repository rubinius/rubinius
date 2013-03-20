module Coverage
  @coverage_tool = nil

  def self.start
    require 'tooling/coverage'

    @coverage_tool = Rubinius::Coverage.new
    @coverage_tool.start
  end

  def self.result
    return unless @coverage_tool

    return {} unless map = @coverage_tool.stop.results

    kernel = File.dirname Rubinius::KERNEL_PATH

    coverage = Hash.new { |h, k| h[k] = [] }

    map.each do |id, attr|
      counts = attr[:counts]
      code = attr[:code]
      next unless code

      file = code.file.to_s
      next if file[0] == ?(
      file = File.join kernel, file unless file[0] == ?/

      code.lines.to_a.drop(1).each_slice(2) do |line, _|
        next unless line > 0
        next if coverage[file][line - 1]

        coverage[file][line - 1] = 0
      end

      counts.each do |ip, count|
        line = code.line_from_ip(ip)
        next unless line > 0

        coverage[file][line - 1] = count
      end
    end

    coverage.each do |file, counts|
      next unless File.exists? file

      lines = File.open(file, "r") { |f| f.lines.count }
      next unless lines > 0

      counts[lines - 1] = nil unless counts.size == lines
    end

    coverage
  end
end
