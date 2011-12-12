ScratchPad.recorded << :con_pre
$con1_ready = true
sleep 0.5
if $con1_raise
  $con1_raise = false
  raise "con1"
end
sleep 0.5
ScratchPad.recorded << :con_post
