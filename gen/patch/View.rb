
c = ::RBe::Cpp::Class["View"]
if c
  c.wrapper = true

  # hooks
  %w(AttachedToWindow AllAttached DetachedFromWindow AllDetached Draw DrawAfterChildren FrameMoved FrameResized ResizeToPreferred KeyDown KeyUp MessageReceived MouseDown MouseMoved MouseUp Pulse TargettedByScrollView WindowActivated).each do |name|
    c.set_hook name
  end

  %w(GetPreferredSize).each do |name|
    c.set_hook name
    c.functions(name).first.set_option(:custom_hook, true)
  end
end
