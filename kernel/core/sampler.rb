class MethodContext
  def normalized_name
    if method_module.is_a?(MetaClass)
      "#{method_module.attached_instance.inspect}.#{name}"
    else
      "#{method_module.name}##{name}"
    end
  end
end

class Sampler
  def initialize(freq=100)
    @frequency = freq
    @show_parent = ENV['PROFILE_FULL']
  end
  
  def start
    @start_clock = activate(@frequency)
    nil
  end
    
  def stop
    @results, @last_clock, @interval = terminate()
    nil
  end
  
  class Call
    def initialize(name)
      @name = name
      @parents = Hash.new { |h,k| h[k] = 0 }
      @slices = 0
    end
    
    def slices=(obj)
      @slices = obj
    end
    
    attr_accessor :parents
    attr_accessor :slices
    attr_accessor :name
  end
  
  def display(out=STDOUT)
    # "Clocks: #{@start_clock} / #{@last_clock}, Interval: #{@interval} useconds"
    calls = Hash.new { |h,k| h[k] = Call.new(k) }
    total_slices = 0

    @results.each do |ent|
      # FIXME: calls[name] += 1 always sets calls[name] to 1
      call = calls[ent.normalized_name]
      call.slices = call.slices + 1
      if @show_parent
        sender = ent.sender
        if sender
          sn = sender.normalized_name
          call.parents[sn] = call.parents[sn] + 1
        end
      end
      total_slices += 1
    end

    # FIXME: [[:a,1],[:b,2]].each { |a, b| ... } => a == [:a, 1], b == nil
    out << "Total slices: #{total_slices}, #{@last_clock - @start_clock} clocks\n"
    out.printf " %% time   slices   name\n"
    calls.sort { |a, b| b[1].slices <=> a[1].slices }.each do |entry|
      percent_time = 100.0 * entry[1].slices / total_slices
      out.printf " %6.2f %8d   %s\n", percent_time, entry[1].slices, entry[0]
      if @show_parent
        top_parents = entry[1].parents.sort { |a,b| b[1] <=> a[1] }
        top_parents[0,3].each do |par|
          out << "                      #{par[0]} (#{par[1]})\n"
        end
      end
      
    end
    nil
  end
end
