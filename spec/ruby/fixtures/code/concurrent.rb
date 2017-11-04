sleep rand

Rubinius.synchronize(Rubinius) do
  ScratchPad.recorded << :concurrent
end
