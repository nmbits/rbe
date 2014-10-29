
c = RBe::Cpp::Class["Control"]
if c
  c.wrapper = true
  c.mixin = "Invoker"

  # noimp
  %w(
    SetIcon
    SetIconBitmap
    IconBitmap
    Perform
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :noimp, true
    end
  end

  # Hooks
  %w(
    WindowActivated
    AttachedToWindow
    DetachedFromWindow
    AllAttached
    AllDetached
    MessageReceived
    KeyDown
    MouseDown
    MouseUp
    MouseMoved
  ).each do |name|
    c.set_hook name
  end

  # custom hook
  %w(
    GetPreferredSize
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom_hook, true
    end
  end

  # custom
  %w(
    GetPreferredSize
  ).each do |name|
    c.functions(name).each do |f|
      f.set_option :custom, true
    end
  end
end
