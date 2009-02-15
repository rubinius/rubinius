def rescue_loop(n)
  n.times do
    begin
    rescue
    end
  end
end

Bench.run [1_000_000] do |n|
  rescue_loop(1_000_000)
end
