def begin_ensure
  begin
    begin
    ensure
    end
  ensure
  end
end

Bench.run [100_000, 1_000_000, 10_000_000] do |n|
  n.times { begin_ensure }
end
