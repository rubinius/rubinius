class Sampler
  def initialize(freq=100)
    @frequency = freq
  end
  
  def start
    @start_clock = activate(@frequency)
  end
    
  def stop
    @results, @last_clock, @interval = terminate()
  end
  
  def display
    puts "Clocks: #{@start_clock} / #{@last_clock}, Interval: #{@interval} useconds"
    @results.each do |ent|
      puts "Object: #{ent.receiver.inspect} / Method: #{ent.method_module}##{ent.name}"
    end
  end
end
