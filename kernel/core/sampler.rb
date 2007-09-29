class Sampler
  def initialize(freq=100)
    @frequency = freq
  end
  
  def start
    @start_clock = activate(@frequency)
    nil
  end
    
  def stop
    @results, @last_clock, @interval = terminate()
    nil
  end
  
  def display(out=STDOUT)
    # "Clocks: #{@start_clock} / #{@last_clock}, Interval: #{@interval} useconds"
    calls = Hash.new { |h,k| h[k] = 0 }
    total_slices = 0

    @results.each do |ent|
      if ent.method_module.is_a?(MetaClass)
        name = "#{ent.method_module.attached_instance.inspect}.#{ent.name}"
      else
        name = "#{ent.method_module.name}##{ent.name}"
      end
      # FIXME: calls[name] += 1 always sets calls[name] to 1
      calls[name] = calls[name] + 1
      total_slices += 1
    end

    # FIXME: [[:a,1],[:b,2]].each { |a, b| ... } => a == [:a, 1], b == nil
    out.printf " %% time   slices   name\n"
    calls.sort { |a, b| b[1] <=> a[1] }.each do |entry|
      percent_time = 100.0 * entry[1] / total_slices
      out.printf " %6.2f %8d   %s\n", percent_time, entry[1], entry[0]
    end
    nil
  end
end
