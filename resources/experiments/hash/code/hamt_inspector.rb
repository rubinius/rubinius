class Hash
  class Inspector
    def initialize(h)
      @h = h
    end

    def table
      @h.instance_variable_get :@table
    end

    def visit(&block)
      return unless table

      table.entries.each do |obj|
        case obj
        when Trie
          yield obj
          visit_trie(obj, &block)
        when Entry
          yield obj
        when List
          List.entries.each { |o| yield o }
        end
      end

      self
    end

    def visit_trie(trie, &block)
      trie.entries.each do |obj|
        case obj
        when Entry
          yield obj
        when Trie
          yield obj
          visit_trie(obj, &block)
        end
      end
    end

    def bytes
      bytes =  Rubinius.memory_size @h
      bytes += Rubinius.memory_size @h.instance_variable_get(:@state)

      if table
        bytes += Rubinius.memory_size table
        bytes += Rubinius.memory_size table.entries
      end

      visit do |obj|
        bytes += Rubinius.memory_size(obj)
        if obj.kind_of? Entry
          bytes += Rubinius.memory_size(obj.key)
          bytes += Rubinius.memory_size(obj.value)
        end
      end

      bytes
    end

    def tries
      number = 0
      visit { |obj| number += 1 if obj.kind_of? Trie }
      number
    end

    def lists
      number = 0
      visit { |obj| number += 1 if obj.kind_of? List }
      number
    end

    def depth_histogram
      histo = []

      visit do |obj|
        if obj.kind_of? Trie
          depth = obj.level + 1

          if x = histo[depth]
            x[1] += 1
          else
            histo[depth] = [depth, 1]
          end
        end
      end

      histo.compact
    end

    def depth
      depth_histogram.max { |a, b| a.first <=> b.first }.first
    end

    def branching_histogram
      histo = Array.new Rubinius::WORDSIZE, 0
      visit { |obj| histo[obj.entries.size-1] += 1 if obj.kind_of? Trie }
      histo.map.with_index { |x, i| [i+1, x] }.select { |x| x.last != 0 }
    end

    def occupancy
      return 0.0 unless table

      number = 0
      table.entries.each { |obj| number += 1 if obj }

      number * 100.0 / table.entries.size
    end

    def report
      d_histo = depth_histogram
      d_max = d_histo.max { |a, b| a.last <=> b.last }
      d_min = d_histo.min { |a, b| a.last <=> b.last }
      d_med = d_histo.sort { |a, b| a.last <=> b.last }[d_histo.size/2]
      d_mean = d_histo.inject(0) { |s, x| s += x.first * x.last } / tries

      b_histo = branching_histogram
      b_max = b_histo.max { |a, b| a.last <=> b.last }
      b_min = b_histo.min { |a, b| a.last <=> b.last }
      b_med = b_histo.sort { |a, b| a.last <=> b.last }[b_histo.size/2]
      b_mean = b_histo.inject(0) { |s, x| s += x.first * x.last } / tries

      <<-EOR
Hash::Inspector report for #{@h.object_id}

Items:     #{@h.size}
Bytes:     #{bytes}
Depth:     #{depth}
Tries:     #{tries}
Occupancy: #{"%.2f" % occupancy}%

Depth Stats
  Max:     #{d_max.first} (#{d_max.last})
  Min:     #{d_min.first} (#{d_min.last})
  Median:  #{d_med.first} (#{d_med.last})
  Mean:    #{d_mean}

Branching Stats
  Max:     #{b_max.first} (#{b_max.last})
  Min:     #{b_min.first} (#{b_min.last})
  Median:  #{b_med.first} (#{b_med.last})
  Mean:    #{b_mean}
      EOR
    end
  end
end
