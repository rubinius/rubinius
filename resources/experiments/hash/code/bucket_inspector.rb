class Hash
  class Inspector
    def initialize(h)
      @h = h
    end

    def entries
      @h.__entries__
    end

    def bytes
      bytes =  Rubinius.memory_size @h
      bytes += Rubinius.memory_size entries
      bytes += Rubinius.memory_size @h.instance_variable_get(:@state)

      @h.each_entry do |e|
        bytes += Rubinius.memory_size(e)
        bytes += Rubinius.memory_size(e.key)
        bytes += Rubinius.memory_size(e.value)
      end

      bytes
    end

    def occupancy
      number = 0
      entries.each { |obj| number += 1 if obj }

      number * 100.0 / entries.size
    end

    def depth
      histogram.max { |a, b| a.first <=> b.first }.first
    end

    def histogram
      histo = []
      entries.each do |chain|
        next unless chain

        depth = 0
        while chain
          depth += 1
          chain = chain.link
        end

        if x = histo[depth]
          x[1] += 1
        else
          histo[depth] = [depth, 1]
        end
      end

      histo.compact
    end

    def report
      histo = histogram
      max = histo.max { |a, b| a.last <=> b.last }
      min = histo.min { |a, b| a.last <=> b.last }
      med = histo.sort { |a, b| a.last <=> b.last }[histo.size/2]

      <<-EOR
Hash::Inspector report for #{@h.object_id}

Items:     #{@h.size}
Bytes:     #{bytes}
Depth:     #{depth}
Table:     #{entries.size}
Occupancy: #{"%.2f" % occupancy}%

Depth Stats
  Max:     #{max.first} (#{max.last})
  Min:     #{min.first} (#{min.last})
  Median:  #{med.first} (#{med.last})
  Mean:    #{@h.size / (occupancy / 100 * entries.size).to_i}
      EOR
    end
  end
end
